#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <utility>
#include <functional>
#include <unordered_map>
#include <boost/optional.hpp>
class DE;
typedef std::vector<double> Solution;
typedef std::vector<std::pair<double, double>> Ranges;
typedef std::vector<double> ConstraintViolation;
typedef std::pair<double, ConstraintViolation> Evaluated;
typedef std::function<Evaluated(const Solution&)> Objective;
enum MutationStrategy {Rand1, Best1, Best2, RandToBest1};
enum CrossoverStrategy {Bin = 0, Exp};
enum SelectionStrategy {StaticPenalty = 0, FeasibilityRule, Epsilon};
class IMutator
{
public:
    virtual std::vector<Solution> mutation(const DE&, const std::vector<Solution>&) = 0;
};
class ICrossover
{
public:
    virtual std::vector<Solution> crossover(const DE&, const std::vector<Solution>&, const std::vector<Solution>&) = 0;
};
class ISelector
{
public:
    virtual bool better(const Evaluated&, const Evaluated&) = 0;
    virtual std::pair<std::vector<Evaluated>, std::vector<Solution>> select(const DE&      // There'll be a default implementation
                                                                            , const std::vector<Solution>&
                                                                            , const std::vector<Solution>&
                                                                            , const std::vector<Evaluated>&
                                                                            , const std::vector<Evaluated>&);
};
class Mutator_Rand_1 : public IMutator
{
public:
    std::vector<Solution> mutation(const DE&, const std::vector<Solution>&);
};
class Mutator_Best_1 : public IMutator
{
public:
    std::vector<Solution> mutation(const DE&, const std::vector<Solution>&);
};
class Mutator_Best_2 : public IMutator
{
public:
    std::vector<Solution> mutation(const DE&, const std::vector<Solution>&);
};
class Mutator_RandToBest_1 : public IMutator
{
public:
    std::vector<Solution> mutation(const DE&, const std::vector<Solution>&);
};
class Crossover_Bin : public ICrossover
{
public:
    std::vector<Solution> crossover(const DE&, const std::vector<Solution>&, const std::vector<Solution>&);
};
class Crossover_Exp : public ICrossover
{
public:
    std::vector<Solution> crossover(const DE&, const std::vector<Solution>&, const std::vector<Solution>&);
};
class Selector_StaticPenalty : ISelector
{
public:
    bool better(const Evaluated&, const Evaluated&);
};
class Selector_FeasibilityRule : ISelector
{
public: // perhaps it would be better if this class inherits Selector_Epsilon and set epsilon_0 = 0
    bool better(const Evaluated&, const Evaluated&);
};
class Selector_Epsilon : ISelector
{
    double epsilon_0;
    double theta;
    size_t tc;
    size_t cp;
    double epsilon_level;
public:
    bool better(const Evaluated&, const Evaluated&);
    std::pair<std::vector<Evaluated>, std::vector<Solution>> select(const DE&      // There'll be a default implementation
                                                                    , const std::vector<Solution>&
                                                                    , const std::vector<Solution>&
                                                                    , const std::vector<Evaluated>&
                                                                    , const std::vector<Evaluated>&);
    Selector_Epsilon(double ep0, double theta, size_t tc, size_t cp)
        : epsilon_0(ep0), theta(theta), tc(tc), cp(cp), epsilon_level(ep0)
    {}
};

class DE
{
protected:
    Objective _func;
    Ranges _ranges;
    double _f;
    double _cr;
    size_t _curr_gen;
    const size_t _np;
    const size_t _dim;
    const size_t _max_iter;
    std::vector<Solution>  _population; // use vector to represent a 2D matrix might not be efficient
    std::vector<Evaluated> _results;

    IMutator*   _mutator;
    ICrossover* _crossover;
    ISelector*  _selector;
    void init();
    void set_mutator();
    void set_corssover();
    void set_selector();
public:
    DE(Objective,
       const Ranges&,
       double f          = 0.8,
       double cr         = 0.8,
       size_t np         = 100,
       size_t max_iter   = 200,
       MutationStrategy  = Best1,
       CrossoverStrategy = Bin,
       SelectionStrategy = StaticPenalty,
       std::unordered_map<std::string, double> extra_para = std::unordered_map<std::string, double> {});
    ~DE();
    Solution solver();
    double f()         const noexcept { return _f; }
    double cr()        const noexcept { return _cr; }
    size_t np()        const noexcept { return _np; }
    size_t curr_gen()  const noexcept { return _curr_gen; }
    size_t dimension() const noexcept { return _dim; }
    size_t find_best(const std::vector<Solution>&) const noexcept;
};
