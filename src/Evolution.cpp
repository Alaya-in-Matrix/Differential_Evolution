#include <iostream>
#include <cstdio>
#include <vector>
#include <functional>
#include <algorithm>
#include <utility>
#include <random>
#include <cassert>
#include <cmath>
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
                    , DE_Stragety stragety
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
    , _strategy(stragety)
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
string DESolver::_show_strategy() const noexcept
{
    string stragety_str;
    switch (_strategy)
    {
    case BEST_1_BIN:
        stragety_str = "DE/Best/1/Bin";
        break;
    case RAND_1_BIN:
        stragety_str = "DE/Rand/1/Bin";
        break;
    case TARGET_TO_BEST_1_BIN:
        stragety_str = "DE/Target-to-Best/1/Bin";
        break;
    case TARGET_TO_RAND_1_BIN:
        stragety_str = "DE/Target-to-Rand/1/Bin";
        break;
    default:
        cerr << "Unsupported stragety: " << _show_strategy() << endl;
        exit(EXIT_FAILURE);
    }
    return stragety_str;
}

// Generate a mutation base vector according to `_strategy`
// return the base_vector and an index
// why index is needed:
//   mutation: ux = base_x + F * (solution[r1] + [r2])
//   where (index, r1, r2) should be mutual exclusive
// if base vector is not in solution, set index to `_init_num`
pair<size_t, vector<double>> DESolver::_mutation_base(const Vec2D& parents) const noexcept
{
    vector<double> base_vector(_init_num, 0);
    size_t idx = _init_num;
    uniform_int_distribution<size_t> i_distr(0, _init_num - 1);
    switch (_strategy)
    {
    case BEST_1_BIN:
        idx         = _find_best(parents);
        base_vector = parents[idx];
        break;
    case RAND_1_BIN:
        idx         = i_distr(_engine);
        base_vector = parents[idx];
        break;
    default:
        cerr << "Unsupported stragety: " << _show_strategy() << endl;
        exit(EXIT_FAILURE);
    }
    return make_pair(idx, base_vector);
}
vector<vector<double>> DESolver::_mutation(const Vec2D& solutions) const noexcept
{
    assert(solutions.size() == _init_num);
    pair<size_t, vector<double>> base_p = _mutation_base(solutions);
    uniform_int_distribution<size_t> i_distr(0, solutions.size() - 1);
    normal_distribution<double> f_distr(_fmu, _fsigma);

    size_t base_idx         = base_p.first;
    vector<double> base_vec = base_p.second;

    Vec2D v;
    v.reserve(solutions.size());
    for (size_t i = 0; i < solutions.size(); ++i)
    {
        double f  = f_distr(_engine);
        size_t r1 = random_exclusive<size_t>(i_distr, vector<size_t> {base_idx});
        size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t> {base_idx, r1});
        const auto& x_r1 = solutions[r1];
        const auto& x_r2 = solutions[r2];
        v.push_back(vector<double>(_para_num));
        for (size_t j = 0; j < _para_num; ++j)
        {
            double tmp = base_vec[j] + f * (x_r1[j] - x_r2[j]);
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
    // static penalty
    return p1.first + p1.second <= p2.first + p2.second;
}
size_t DESolver::_find_best(const Vec2D& solutions) const noexcept
{
    auto min_iter = min_element(_results.begin(), _results.end(), [&](const pair<double, double>& p1, const pair<double, double>& p2) -> bool
    {
        return _better(p1, p2);
    });
    assert(min_iter != _results.end());
    return distance(_results.begin(), min_iter);
}
void DESolver::_report_best() const noexcept
{
    size_t best_idx = _find_best(_candidates);
    const auto& best = _results[best_idx];
    printf("Current Best idx: %ld, Fom: %g, Constraint Violation: %g\n", best_idx, best.first, best.second);
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

    _report_best();
    for (unsigned int i = 0; i < _iter_num; ++i)
    {
        auto v      = _mutation(_candidates); // 会做返回值优化吧
        auto u      = _crossover(_candidates, v);
        _selection(_candidates, u);
        _report_best();
    }
    size_t best_idx = _find_best(_candidates);
    return _candidates[best_idx];
}
void EpsilonDE::_report_best() const noexcept
{
    size_t best_idx = _find_best(_candidates);
    const auto& best = _results[best_idx];
    printf("Epsilon: %g, Current Best idx: %ld, Fom: %g, Constraint Violation: %g\n", epsilon_level, best_idx, best.first, best.second);
}
vector<double> EpsilonDE::solver()
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
    init_epsilon();
    _report_best();
    for (unsigned int i = 0; i < _iter_num; ++i)
    {
        auto v      = _mutation(_candidates); // 会做返回值优化吧
        auto u      = _crossover(_candidates, v);
        _selection(_candidates, u);
        _report_best();

        update_epsilon();
        ++curr_gen;
    }
    size_t best_idx = _find_best(_candidates);
    return _candidates[best_idx];
}
void EpsilonDE::init_epsilon()
{
    assert(_results.size() == _init_num);
    vector<double> c_violation;
    for (auto rp : _results)
        c_violation.push_back(rp.second);
    sort(c_violation.begin(), c_violation.end());
    // At least on non-fail point should be sampled
    if (std::isinf(c_violation[0]))
    {
        cerr << "All initial sampling faile(constraint violation is infinity)" << endl;
        cerr << "You may need to check your objective function or run this program again" << endl;
        exit(EXIT_FAILURE);
    }
    size_t idx = (size_t)floor(theta * c_violation.size());
    if (std::isinf(c_violation[idx]) && idx > 0)
    {
        size_t tmp_idx = idx;
        while (std::isinf(c_violation[tmp_idx]) && tmp_idx > 0)
            --tmp_idx;
        idx = (0 + tmp_idx) / 2; // use median
    }
    epsilon_0     = c_violation.at(idx);
    epsilon_level = epsilon_0;
}

void EpsilonDE::update_epsilon()
{
    if (curr_gen > tc)
        epsilon_level = 0;
    else
        epsilon_level = epsilon_0 * pow(1 - (double)curr_gen / (double)tc, cp);
}
bool EpsilonDE::_better(const pair<double, double>& p1, const pair<double, double>& p2) const noexcept
{
    if ((p1.second <= epsilon_level && p2.second <= epsilon_level) || p1.second == p2.second)
    {
        return p1.first <= p2.first;
    }
    else
    {
        return p1.second <= p2.second;
    }
}
bool FeasibilityRuleDE::_better(const pair<double, double>& p1, const pair<double, double>& p2) const noexcept
{
    // Feasibility Rules
    // Advantage: no additional parameters added, avoid manually setting penalty factor
    if (p1.second == 0 && p2.second == 0)       // if both solutions are feasible, compare objective function
    {
        return p1.first <= p2.first;
    }
    else if (p1.second <= 0 && p2.second > 0)   // feasible solution is superior to infeasible solution
    {
        return true;
    }
    else if (p1.second > 0 && p2.second <= 0)
    {
        return false;
    }
    else                                        // if both solutions are infeasible, compare constraint violation
    {
        return p1.second <= p2.second;
    }
}
