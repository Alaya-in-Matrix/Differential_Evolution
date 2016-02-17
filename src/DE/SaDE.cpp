#include "DE/SaDE.h"
#include <vector>
#include <unordered_map>
#include <deque>
#include <string>
#include <random>
using namespace std;
static mt19937_64 engine(random_device{}());
SaDE::SaDE(Objective f, const Ranges& r, size_t np, size_t max_iter, size_t lp,
           SelectionStrategy ss, unordered_map<string, double> extra)
    : DE(f, r, nullptr, nullptr, nullptr, 0, 0, np, max_iter, extra),
      _fmu(0.75),
      _fsigma(0.25),
      _crmu(0.8),
      _crsigma(0.0),
      _lp(lp), 
      _strategy_pool(_init_strategy()), 
      _strategy_prob(_init_strategy_prob()), 
      _mem_success(deque<vector<size_t>>(_lp, vector<size_t>(_strategy_pool.size(), 0))), 
      _mem_failure(deque<vector<size_t>>(_lp, vector<size_t>(_strategy_pool.size(), 0)))
{
    _selector = set_selector(ss, extra);
}
double SaDE::f() const noexcept
{
    return normal_distribution<double>(_fmu, _fsigma)(engine);
}
double SaDE::cr() const noexcept
{
    return normal_distribution<double>(_crmu, _crsigma)(engine);
}
vector<SaDE::Strategy> SaDE::_init_strategy() const noexcept
{
    return vector<Strategy> {
        Strategy(new Mutator_Rand_1, new Crossover_Bin), 
        Strategy(new Mutator_RandToBest_1, new Crossover_Bin), 
        Strategy(new Mutator_Rand_2, new Crossover_Bin), 
        Strategy(new Mutator_CurrentToRand_1, new Crossover_Exp)
    };
}
vector<double> SaDE::_init_strategy_prob() const noexcept
{
    const size_t num_strategy = _strategy_pool.size();
    return vector<double>(num_strategy, 1.0 / (double)(num_strategy));
}
Solution SaDE::solver()
{
    init();
    for (_curr_gen = 1; _curr_gen < _max_iter; ++_curr_gen)
    {
        vector<Solution> trials;
        vector<Strategy> s_vec;
        trials.reserve(_np);
        s_vec.reserve(_np);
        for (size_t i = 0; i < _np; ++i)
            s_vec.push_back(_select_strategy(_strategy_prob, _strategy_pool));
        for (size_t i = 0; i < _np; ++i)
        {
            const Solution& target = _population[i];
            const Solution doner = s_vec[i].mutator->mutation_solution(*this, i);
            trials.push_back(s_vec[i].crossover->crossover_solution(*this, target, doner));
        }
        vector<Evaluated> trial_results(_np);
#pragma omp parallel for
        // OpenMP 2.0 doesn't allow unsigned for loop index!
        // But it seems VS2015 is still using this version
        for (int p_idx = 0; p_idx < static_cast<int>(_population.size()); ++p_idx)
        {
            trial_results[p_idx] = _func(p_idx, trials[p_idx]);
        }
        auto new_result = _selector->select(*this, _population, trials, _results, trial_results);
        _update_memory_prob(_results, new_result.first);
        _results        = new_result.first;
        _population     = new_result.second;
        report_best();
    }
    size_t best_idx = find_best();
    return _population[best_idx];
}
void SaDE::_update_memory_prob(const std::vector<Evaluated>& old_result,
                               const std::vector<Evaluated>& new_result) noexcept
{
}
