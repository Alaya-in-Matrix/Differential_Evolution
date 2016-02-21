#pragma once
#include "DEOrigin.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <deque>
#include <utility>
#include <memory>
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
        Strategy(std::shared_ptr<IMutator> m, std::shared_ptr<ICrossover> c) : mutator(m), crossover(c) {}
        std::shared_ptr<IMutator> mutator;
        std::shared_ptr<ICrossover> crossover;
    };
    const std::vector<Strategy>     _strategy_pool;
    std::vector<double>             _strategy_prob;
    std::deque<std::vector<size_t>> _mem_success;
    std::deque<std::vector<size_t>> _mem_failure;
    std::vector<std::deque<std::vector<double>>> _crmemory;
    double _curr_cr;
    std::vector<Strategy>           _init_strategy() const noexcept;
    std::vector<double>             _init_strategy_prob() const noexcept;
    size_t _select_strategy(const std::vector<double>& probs) const noexcept;
    std::vector<double> gen_cr_vec(const std::vector<size_t>&) noexcept;
    void _update_memory_prob(const std::vector<size_t>& strategy_vec,
                             const std::vector<Evaluated>& old_result,
                             const std::vector<Evaluated>& new_result);
    void _update_cr_memory(const std::vector<size_t>&, const std::vector<double>&,
                           const std::vector<Evaluated>&, const std::vector<Evaluated>&) noexcept;

public:
    SaDE(const SaDE&) = delete;
    SaDE(SaDE&&)      = delete;
    SaDE& operator=(const SaDE&) = delete;
    SaDE(Objective, 
         const Ranges&,
         size_t np,
         size_t max_iter,
         SelectionStrategy, 
         std::unordered_map<std::string, double> extra);
    ~SaDE() = default;
    double f()  const noexcept;
    double cr() const noexcept;
    Solution solver();
};
