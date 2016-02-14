#pragma once
#include "DEOrigin.h"
#include <vector>
#include <unordered_map>
#include <string>
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
        IMutator* mutator;
        ICrossover* crossover;
        ~Strategy();
        Strategy(const Strategy&) =default;
        Strategy& operator=(const Strategy&) = default;
    };
    std::vector<Strategy>            _strategy_pool;
    std::vector<double>              _strategy_prob;
    std::vector<std::vector<size_t>> _mem_success;
    std::vector<std::vector<size_t>> _mem_failure;
    std::vector<Strategy> init_strategy() const noexcept;
    std::vector<double> init_strategy_prob() const noexcept;

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
#include <string>
#include <random>
using namespace std;
static mt19937_64 engine(random_device{}());
SaDE::Strategy::~Strategy()
{
    if(mutator != nullptr)   delete mutator;
    if(crossover != nullptr) delete crossover;
}
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
double SaDE::f() const noexcept
{
    return normal_distribution<double>(_fmu, _fsigma)(engine);
}
double SaDE::cr() const noexcept
{
    return normal_distribution<double>(_crmu, _crsigma)(engine);
}
Solution SaDE::solver()
{
    return _population.at(0);
}
