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
        ~Strategy()
        {
            if(mutator   != nullptr) delete mutator;
            if(crossover != nullptr) delete crossover;
        }
        Strategy(const Strategy&)            = default;
        Strategy& operator=(const Strategy&) = default;
    };
    const std::vector<Strategy>     _strategy_pool;
    std::vector<double>             _strategy_prob;
    std::deque<std::vector<size_t>> _mem_success;
    std::deque<std::vector<size_t>> _mem_failure;
    std::vector<Strategy>           _init_strategy() const noexcept;
    std::vector<double>             _init_strategy_prob() const noexcept;
    Strategy _select_strategy(const std::vector<double>& probs,
                             const std::vector<Strategy>& strategies) const noexcept;
    void _update_memory_prob(const std::vector<Evaluated>& old_result,
                             const std::vector<Evaluated>& new_result) noexcept;

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
    ~SaDE() = default;
    double f()  const noexcept;
    double cr() const noexcept;
    Solution solver();
};
