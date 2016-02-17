#pragma once
#include "DEOrigin.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <deque>
#include <utility>
class SaDE : public DE
{
protected:
    double _fmu;
    double _fsigma;
    double _crmu;
    double _crsigma;
    size_t _lp;
    struct Strategy
    {
        Strategy(IMutator* m, ICrossover* c) : mutator(m), crossover(c) {}
        IMutator*   mutator;
        ICrossover* crossover;
        ~Strategy(){};  // Do not delete strategy pointers when destructing
        Strategy(const Strategy&)            = default;
        Strategy& operator=(const Strategy&) = default;
    };
    std::vector<Strategy>           _strategy_pool;
    std::vector<double>             _strategy_prob;
    std::deque<std::vector<size_t>> _mem_success;
    std::deque<std::vector<size_t>> _mem_failure;
    std::vector<Strategy> init_strategy() const noexcept;
    std::vector<double>   init_strategy_prob() const noexcept;
    Strategy select_strategy(const std::vector<double>& probs, std::vector<Strategy>& strategies) const noexcept;

public:
    SaDE(const SaDE&) = delete;
    SaDE(SaDE&&) = delete;
    SaDE& operator=(const SaDE&) = delete;
    SaDE(Objective, 
         const Ranges&,
         size_t np,
         size_t max_iter,
         size_t lp,
         SelectionStrategy, 
         std::unordered_map<std::string, double> extra);
    ~SaDE();
    double f()  const noexcept;
    double cr() const noexcept;
    Solution solver();
};
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
      _strategy_pool(init_strategy()), 
      _strategy_prob(init_strategy_prob()), 
      _mem_success(vector<vector<size_t>>(_lp, vector<size_t>(_strategy_pool.size(), 0))), 
      _mem_failure(vector<vector<size_t>>(_lp, vector<size_t>(_strategy_pool.size(), 0)))
{
    _selector = set_selector(ss, extra);
    init_strategy();
}
SaDE::~SaDE()
{
    DE::~DE();
    for(auto& s : _strategy_pool)
    {
        if(s.mutator   != nullptr) delete s.mutator;
        if(s.crossover != nullptr) delete s.crossover;
    }
}
double SaDE::f() const noexcept
{
    return normal_distribution<double>(_fmu, _fsigma)(engine);
}
double SaDE::cr() const noexcept
{
    return normal_distribution<double>(_crmu, _crsigma)(engine);
}
vector<SaDE::Strategy> SaDE::init_strategy() const noexcept
{
    return vector<Strategy> {
        Strategy(new Mutator_Rand_1, new Crossover_Bin), 
        Strategy(new Mutator_RandToBest_1, new Crossover_Bin), 
        Strategy(new Mutator_Rand_2, new Crossover_Bin), 
        Strategy(new Mutator_CurrentToRand_1, new Crossover_Exp)
    };
}
vector<double> SaDE::init_strategy_prob() const noexcept
{
    const size_t num_strategy = _strategy_pool.size();
    return vector<double>(num_strategy, 1.0 / (double)(num_strategy));
}
Solution SaDE::solver()
{
    init();
    for(_curr_gen = 1; _curr_gen < _max_iter; ++_curr_gen)
    {
        Strategy strategy = select_strategy(_strategy_prob, _strategy_pool);
        // auto doners = strategy.mutator->mutation(*this);
        // auto trials
    }
}

// Solution DE::solver()
// {
//     init();
//     for (_curr_gen = 1; _curr_gen < _max_iter; ++_curr_gen)
//     {
//         auto doners = _mutator->mutation(*this);
//         auto trials = _crossover->crossover(*this, _population, doners);
//         vector<Evaluated> trial_results(_np);
// #pragma omp parallel for
// 		// OpenMP 2.0 doesn't allow unsigned for loop index!
//         for (int p_idx = 0; p_idx < (int)_population.size(); ++p_idx)
//         {
//             trial_results[p_idx] = _func(p_idx, trials[p_idx]);
//         }
//         auto new_result = _selector->select(*this, _population, trials,
//                                             _results, trial_results);
//         copy(new_result.first.begin(), new_result.first.end(),
//              _results.begin());
//         copy(new_result.second.begin(), new_result.second.end(),
//              _population.begin());
//         report_best();
//     }
//     size_t best_idx = find_best();
//     return _population[best_idx];
// }
