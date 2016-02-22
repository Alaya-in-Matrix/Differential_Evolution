#include "DE/DEOrigin.h"
#include "util.h"
#include "global.h"
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <random>
#include <omp.h>
#include <cassert>
#include <numeric>
#include <string>
using namespace std;
DE::DE(Objective func, const Ranges& rg, MutationStrategy ms,
       CrossoverStrategy cs, SelectionStrategy ss, double f, double cr,
       size_t np, size_t max_iter, unordered_map<string, double> extra)
    : _func(func),
      _ranges(rg),
      _f(f),
      _cr(cr),
      _np(np),
      _dim(rg.size()),
      _max_iter(max_iter),
      _extra_conf(extra),
      _curr_gen(0),
      _use_built_in_strategy(true)
{
    _mutator   = set_mutator(ms, extra);
    _crossover = set_crossover(cs, extra);
    _selector  = set_selector(ss, extra);
}
DE::DE(Objective func, const Ranges& rg, IMutator* m, ICrossover* c,
       ISelector* s, double f, double cr, size_t np, size_t max_iter,
       unordered_map<string, double> extra)
    : _func(func),
      _ranges(rg),
      _f(f),
      _cr(cr),
      _np(np),
      _dim(rg.size()),
      _max_iter(max_iter),
      _extra_conf(extra),
      _curr_gen(0),
      _mutator(m),
      _crossover(c),
      _selector(s),
      _use_built_in_strategy(false)
{
}
Solution DE::solver()
{
    init();
    for (_curr_gen = 1; _curr_gen < _max_iter; ++_curr_gen)
    {
        auto doners = _mutator->mutation(*this);
        auto trials = _crossover->crossover(*this, _population, doners);
        vector<Evaluated> trial_results(_np);
#pragma omp parallel for
		// OpenMP 2.0 doesn't allow unsigned for loop index!
        for (int p_idx = 0; p_idx < (int)_population.size(); ++p_idx)
        {
            trial_results[p_idx] = _func(p_idx, trials[p_idx]);
        }
        auto new_result = _selector->select(*this, _population, trials,
                                            _results, trial_results);
        copy(new_result.first.begin(), new_result.first.end(),
             _results.begin());
        copy(new_result.second.begin(), new_result.second.end(),
             _population.begin());
        report_best();
    }
    size_t best_idx = find_best();
    return _population[best_idx];
}
DE::~DE()
{
    if (_use_built_in_strategy)
    {
        assert(_mutator != nullptr && _crossover != nullptr && _selector != nullptr);
        delete _mutator;
        delete _crossover;
        delete _selector;
    }
}
IMutator* DE::set_mutator(MutationStrategy ms, const unordered_map<string, double>&) const noexcept
{
    IMutator* mutator;
    switch (ms)
    {
        case Rand1:
            mutator = new Mutator_Rand_1;
            break;
        case Rand2:
            mutator = new Mutator_Rand_2;
            break;
        case Best1:
            mutator = new Mutator_Best_1;
            break;
        case Best2:
            mutator = new Mutator_Best_2;
            break;
        case CurrentToRand1:
            mutator = new Mutator_CurrentToRand_1;
            break;
        case RandToBest1:
            mutator = new Mutator_RandToBest_1;
            break;
        case RandToBest2:
            mutator = new Mutator_RandToBest_2;
            break;
        default:
            mutator = nullptr;
            cerr << "Unrecognoized Mutation Strategy" << endl;
            exit(EXIT_FAILURE);
    }
    return mutator;
}
ICrossover* DE::set_crossover(CrossoverStrategy cs, const unordered_map<string, double>&) const noexcept
{
    ICrossover* crossover;
    if (cs == CrossoverStrategy::Bin)
        crossover = new Crossover_Bin;
    else if (cs == CrossoverStrategy::Exp)
        crossover = new Crossover_Exp;
    else
    {
        crossover = nullptr;
        cerr << "Unrecognoized Crossover Strategy" << endl;
        exit(EXIT_FAILURE);
    }
    return crossover;
}
ISelector* DE::set_selector(SelectionStrategy ss, const unordered_map<string, double>& config) const noexcept
{
    ISelector* selector;
    if (ss == SelectionStrategy::StaticPenalty)
        selector = new Selector_StaticPenalty;
    else if (ss == SelectionStrategy::FeasibilityRule)
        selector = new Selector_FeasibilityRule;
    else if (ss == SelectionStrategy::Epsilon)
    {
        for (string name : vector<string>{"theta", "tc", "cp"})
        {
            if (config.find(name) == config.end())
            {
                cerr << "Can't find " << name << " in extra confg" << endl;
                exit(EXIT_FAILURE);
            }
        }
        double theta = config.find("theta")->second;
        double cp    = config.find("cp")->second;
        size_t tc    = (size_t)config.find("tc")->second;
        selector = new Selector_Epsilon(theta, cp, tc);
    }
    else
    {
        selector = nullptr;
        cerr << "Unrecognoized Selection Strategy" << endl;
        exit(EXIT_FAILURE);
    }
    return selector;
}
void DE::init()
{
    // rate of populations with non-infinity constraint violationss
    _population = vector<Solution>(_np, vector<double>(_dim, 0));
    _results = vector<Evaluated>(_np);
    size_t min_valid_num =
        _extra_conf.find("min_valid_num") == _extra_conf.end()
            ? 1
            : (size_t)_extra_conf.find("min_valid_num")->second;
    vector<bool> valid(_np, false);
    size_t num_valid = 0;
    do
    {
#pragma omp parallel for reduction(+ : num_valid)
		// OpenMP 2.0 doesn't allow unsigned for-loop index
        for (int i = 0; i < (int)_np; ++i)
        {
            if (!valid[i])
            {
#pragma omp critical
                {
                    for (size_t j = 0; j < _dim; ++j)
                    {
                        double lb = _ranges.at(j).first;
                        double ub = _ranges.at(j).second;
                        uniform_real_distribution<double> distr(lb, ub);
                        _population[i][j] = distr(engine);
                    }
                }
                _results[i] = _func(i, _population[i]);
                vector<double> vio_vec = _results[i].second;
                auto inf_pred = [](const double x) -> bool
                {
                    return std::isinf(x);
                };
                bool valid_flag = find_if(vio_vec.begin(), vio_vec.end(),
                                          inf_pred) == vio_vec.end();
                valid[i] = valid_flag;
                num_valid += valid_flag ? 1 : 0;
            }
        }
        cout << "num_valid: " << num_valid
             << ", min_valid_num: " << min_valid_num << endl;
    } while (num_valid < min_valid_num);
}
size_t DE::find_best() const noexcept
{
    // If we need to avoid unnecessary `find_best` calling, 
    // below code could be used:
    //     static size_t curr_gen = 0;
    //     static size_t best_idx = 0;
    //     if (curr_gen < _curr_gen)
    //     {
    //         auto min_iter = min_element(_results.begin(), _results.end(),
    //                                     [&](const Evaluated& e1, const Evaluated& e2) -> bool
    //                                     {
    //                                         return _selector->better(e1, e2);
    //                                     });
    //         curr_gen = _curr_gen;
    //         best_idx = distance(_results.begin(), min_iter);
    //     }
    //     return best_idx;
    auto min_iter =
        min_element(_results.begin(), _results.end(),
                    [&](const Evaluated& e1, const Evaluated& e2) -> bool
                    {
                        return _selector->better(e1, e2);
                    });
    return distance(_results.begin(), min_iter);
}
void DE::report_best() const noexcept
{
    size_t best_idx = find_best();
    const Evaluated& best_result = _results[best_idx];
    const double violation = accumulate(best_result.second.begin(), best_result.second.end(), 0.0);
    double total_violation = 0;
    for(auto r : _results)
    {
        total_violation += accumulate(r.second.begin(), r.second.end(), 0.0);
    }
    cout << "Best idx: " << best_idx << ", Best FOM: " << best_result.first
         << ", Constraint Violation: " << violation 
         << ", Average Constraint Violation: " << total_violation / _results.size() << endl;
}
