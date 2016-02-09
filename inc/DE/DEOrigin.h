#pragma once
#include "strategy/DEInterface.h"
#include "strategy/DEBuiltInStrategy.h"
class DE {
protected:
    Objective _func;
    const Ranges _ranges;
    const double _f;
    const double _cr;
    const size_t _np;
    const size_t _dim;
    const size_t _max_iter;
    const std::unordered_map<std::string, double> _extra_conf;
    size_t _curr_gen;
    std::vector<Solution> _population; // use vector to represent a 2D matrix might not be efficient
    std::vector<Evaluated> _results;
    IMutator*   _mutator;
    ICrossover* _crossover;
    ISelector*  _selector;
    bool _use_built_in_strategy;

    virtual IMutator*   set_mutator(MutationStrategy, const std::unordered_map<std::string, double>&)    const noexcept;
    virtual ICrossover* set_crossover(CrossoverStrategy, const std::unordered_map<std::string, double>&) const noexcept;
    virtual ISelector*  set_selector(SelectionStrategy, const std::unordered_map<std::string, double>&)  const noexcept;
    virtual void init();

public:
    DE(Objective, // User-defined strategy, and strategy pointers would be destructed by user
        const Ranges&,
        IMutator*   m,
        ICrossover* c,
        ISelector*  s,
        double      f        = 0.8,
        double      cr       = 0.8,
        size_t      np       = 100,
        size_t      max_iter = 200,
        std::unordered_map<std::string, double> extra_para = std::unordered_map<std::string, double>{});
    DE(Objective,
        const Ranges&,
        MutationStrategy  = Best1,
        CrossoverStrategy = Bin,
        SelectionStrategy = StaticPenalty,
        double f          = 0.8,
        double cr         = 0.8,
        size_t np         = 100,
        size_t max_iter   = 200,
        std::unordered_map<std::string, double> extra_para = std::unordered_map<std::string, double>{});
    virtual ~DE();
    virtual Solution solver();

    virtual double f() const noexcept { return _f; }
    virtual double cr() const noexcept { return _cr; }
    virtual size_t np() const noexcept { return _np; }
    virtual size_t curr_gen() const noexcept { return _curr_gen; }
    virtual size_t dimension() const noexcept { return _dim; }
    virtual size_t find_best() const noexcept;
    virtual void report_best() const noexcept;
    virtual std::pair<double, double> range(size_t i) const { return _ranges.at(i); }
    virtual const std::vector<Solution>& population() const noexcept { return _population; }
    virtual const std::vector<Evaluated>& evaluated() const noexcept { return _results; }
};
