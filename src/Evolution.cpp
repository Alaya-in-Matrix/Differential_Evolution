#include <cstdio>
#include <vector>
#include <functional>
#include <algorithm>
#include <utility>
#include <random>
#include <cassert>
#include <numeric>
#include <limits>
#include <omp.h>

#include "Evolution.h"
#include "util.h"
using namespace std;
mt19937_64 _engine(random_device{}());
DESolver::DESolver( function <pair<double, double>(unsigned int idx, const vector<double>&)> f
                    , RangeVec rg
                    , unsigned int iter_num
                    , unsigned int para_num
                    , unsigned int init_num
                    , double cr
                    , double fmu
                    , double fsigma
                  )
    : _func(f)
    , _ranges(rg)
    , _iter_num(iter_num)
    , _para_num(para_num)
    , _init_num(init_num)
    , _cr(cr)
    , _fmu(fmu)
    , _fsigma(fsigma)
{
    assert(rg.size() == _para_num);

    _candidates.reserve(_init_num);
    _results.reserve(_init_num);
    for (unsigned int i = 0; i < _init_num; ++i)
    {
        _candidates.push_back(vector<double>());
        _candidates[i].reserve(_para_num);
        for (const auto& p : _ranges)
        {
            assert(p.first < p.second);
            uniform_real_distribution<double> distr(p.first, p.second);
            _candidates[i].push_back(distr(_engine));
        }
    }
}

// 为何返回值类型不可以用"Vec2D"，参数类型却可以？
vector<vector<double>> DESolver::_mutation(const Vec2D& solutions) const noexcept
{
    size_t best_idx = _find_best(solutions);
    uniform_int_distribution<size_t> i_distr(0, solutions.size() - 1); // 其实 solutions.size() == _init_num?
    normal_distribution<double> f_distr(_fmu, _fsigma);
    const auto& x_best = solutions[best_idx];

    Vec2D v;
    v.reserve(solutions.size());
    for (size_t i = 0; i < solutions.size(); ++i)
    {
        double f  = f_distr(_engine);
        size_t r1 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx});
        size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx, r1});
        const auto& x_r1 = solutions[r1];
        const auto& x_r2 = solutions[r2];
        v.push_back(vector<double>(_para_num));
        for (size_t j = 0; j < _para_num; ++j)
        {
            double tmp = x_best[j] + f * (x_r1[j] - x_r2[j]);
            double lb  = _ranges[j].first;
            double ub  = _ranges[j].second;
            // Random reinitiate
            // perhaps I need to set the distr as a member of DESolver ?
            v[i][j] = lb <= tmp && tmp <= ub ? tmp : uniform_real_distribution<double>(lb, ub)(_engine);
        }
    }
    return v;
}
vector<vector<double>> DESolver::_crossover(const Vec2D& x, const Vec2D& v) const noexcept
{
    assert(x.size() == v.size());
    size_t solution_num = x.size();
    uniform_real_distribution<double>        distr_randij(0, 1);
    uniform_int_distribution<unsigned int>   distr_randn(0, _para_num - 1);

    Vec2D u(x.size());
    for (size_t i = 0; i < solution_num; ++i)
    {
        assert(x[i].size() == _para_num);
        assert(v[i].size() == _para_num);

        u[i] = vector<double>(_para_num);
        auto randn = distr_randn(_engine);
        for (unsigned int j = 0; j < _para_num; ++j)
        {
            double randij = distr_randij(_engine);
            u[i][j] = (randij < _cr || j == randn) ? v[i][j] : x[i][j];
        }
    }
    return u;
}

void DESolver::_selection(const Vec2D& x, const Vec2D& u) noexcept
{
    assert(x.size() == u.size());
    Vec2D s(x.size());
    #pragma omp parallel for schedule(dynamic, 2)
    for (size_t i = 0; i < x.size(); ++i)
    {
        assert(x[i].size() == _para_num);
        assert(u[i].size() == _para_num);

        pair<double, double> u_evaled = _func(i, u[i]);
        if (_better(u_evaled, _results[i]))
        {
            _candidates[i] = u[i];
            _results[i]    = u_evaled;
        }
    }
}
bool DESolver::_better(const pair<double, double>& p1, const pair<double, double>& p2) const noexcept
{
    // // Feasibility Rules
    // if(p1.second == 0 && p2.second == 0)
    // {
    //     return p1.first <= p1.first;
    // }
    // else if(p1.second <= 0 && p2.second > 0)
    // {
    //     return true;
    // }
    // else if(p1.second > 0 && p2.second <= 0)
    // {
    //     return false;
    // }
    // else 
    // {
    //     return p1.second <= p2.second;
    // }

    // static penalty
    return p1.first + p1.second <= p2.first + p2.second;
}
size_t DESolver::_find_best(const Vec2D& solutions) const noexcept
{
    // size_t best_idx = distance(_results.begin(), min_element(_results.begin(), _results.end(), [&](const pair<double, double>& p1, const pair<double, double>& p2)->bool
    // {
    //     return _better(p1, p2);
    // }));
    size_t best_idx  = 0;
    auto best_result = _results[0];
    for(size_t i = 0; i < _results.size(); ++i)
    {
        if(_better(_results[i], best_result))
        {
            best_idx    = i;
            best_result = _results[i];
        }
    }
    printf("Current Best: idx = %ld, fom = %g, _constraint_violation = %g, fom + c_violation = %g\n", best_idx, best_result.first, best_result.second, best_result.first + best_result.second);
    fflush(stdout);
    return best_idx;
}
vector<double> DESolver::solver()
{
    _candidates.clear();
    _candidates.reserve(_init_num);
    const double inf = numeric_limits<double>::infinity();
    _results = vector<pair<double, double>>(_init_num, make_pair(inf, inf));
    for (unsigned int i = 0; i < _init_num; ++i)
    {
        _candidates.push_back(vector<double>(_para_num));
        for (unsigned int j = 0; j < _para_num; ++j)
        {
            double lb = _ranges[j].first;
            double ub = _ranges[j].second;
            uniform_real_distribution<double> distr(lb, ub);
            _candidates[i][j] = distr(_engine);
        }
    }
    #pragma omp parallel for schedule(dynamic, 2)
    for (unsigned int i = 0; i < _init_num; ++i)
    {
        _results[i] = _func(i, _candidates[i]);
    }

    for (unsigned int i = 0; i < _iter_num; ++i)
    {
        auto v      = _mutation(_candidates); // 会做返回值优化吧
        auto u      = _crossover(_candidates, v);
        _selection(_candidates, u);
    }
    size_t best_idx = _find_best(_candidates);
    return _candidates[best_idx];
}
