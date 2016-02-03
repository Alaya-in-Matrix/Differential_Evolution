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
    virtual std::vector<Solution> mutation(const DE&) = 0;
    virtual double boundary_constraint(std::pair<double, double>, double) const noexcept;
    virtual ~IMutator(){}
};
class ICrossover
{
public:
    virtual std::vector<Solution> crossover(const DE&, const std::vector<Solution>&, const std::vector<Solution>&) = 0;
    virtual ~ICrossover(){}
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
    virtual ~ISelector(){}
};
class Mutator_Rand_1 : public IMutator
{
public:
    std::vector<Solution> mutation(const DE&);
};
class Mutator_Best_1 : public IMutator
{
public:
    std::vector<Solution> mutation(const DE&);
};
class Mutator_Best_2 : public IMutator
{
public:
    std::vector<Solution> mutation(const DE&);
};
class Mutator_RandToBest_1 : public IMutator
{
public:
    std::vector<Solution> mutation(const DE&);
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
class Selector_StaticPenalty : public ISelector
{
public:
    bool better(const Evaluated&, const Evaluated&);
};
class Selector_FeasibilityRule : public ISelector
{
public: // perhaps it would be better if this class inherits Selector_Epsilon and set epsilon_0 = 0
    bool better(const Evaluated&, const Evaluated&);
};
class Selector_Epsilon : public ISelector
{
    const double theta;
    const size_t tc;
    const size_t cp;
    double epsilon_0;
    double epsilon_level;
public:
    bool better(const Evaluated&, const Evaluated&);
    std::pair<std::vector<Evaluated>, std::vector<Solution>> select(const DE&      // There'll be a default implementation
            , const std::vector<Solution>&
            , const std::vector<Solution>&
            , const std::vector<Evaluated>&
            , const std::vector<Evaluated>&);
    Selector_Epsilon(double theta, size_t tc, size_t cp)
        : theta(theta), tc(tc), cp(cp)
    {
        if(theta < 0 || theta > 1)
        {
            std::cerr << "theta should be in [0, 1]" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
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
    const std::unordered_map<std::string, double> _extra_conf;
    std::vector<Solution>  _population; // use vector to represent a 2D matrix might not be efficient
    std::vector<Evaluated> _results;

    IMutator*   _mutator;
    ICrossover* _crossover;
    ISelector*  _selector;
    bool _use_built_in_strategy;
    void set_mutator(MutationStrategy,    const std::unordered_map<std::string, double>&);
    void set_crossover(CrossoverStrategy, const std::unordered_map<std::string, double>&);
    void set_selector(SelectionStrategy,  const std::unordered_map<std::string, double>&);
    void init();
public:
    DE(Objective, // User-defined strategy, and strategy pointers would be destructed by user
       const Ranges&,
       IMutator* m,
       ICrossover* c,
       ISelector* s,
       std::unordered_map<std::string, double> extra_para = std::unordered_map<std::string, double> {},
       double f          = 0.8,
       double cr         = 0.8,
       size_t np         = 100,
       size_t max_iter   = 200);
    DE(Objective,
       const Ranges&,
       MutationStrategy  = Best1,
       CrossoverStrategy = Bin,
       SelectionStrategy = StaticPenalty,
       std::unordered_map<std::string, double> extra_para = std::unordered_map<std::string, double> {},
       double f          = 0.8,
       double cr         = 0.8,
       size_t np         = 100,
       size_t max_iter   = 200);
    ~DE();
    Solution solver();

    double f()         const noexcept { return _f; }
    double cr()        const noexcept { return _cr; }
    size_t np()        const noexcept { return _np; }
    size_t curr_gen()  const noexcept { return _curr_gen; }
    size_t dimension() const noexcept { return _dim; }
    std::pair<double, double> range(size_t i) const { return _ranges.at(i);}
    const std::vector<Solution>& population() const noexcept { return _population; }
    const std::vector<Evaluated>& evaluated() const noexcept { return _results; }
    size_t find_best() const noexcept;
    void report_best() const noexcept;
};
