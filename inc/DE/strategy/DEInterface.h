#pragma once
#include <vector>
#include <unordered_map>
#include <functional>
#include <string>
enum MutationStrategy {
    Rand1 = 0,
    Best1,
    Best2,
    RandToBest1
};
enum CrossoverStrategy {
    Bin = 0,
    Exp
};
enum SelectionStrategy {
    StaticPenalty = 0,
    FeasibilityRule,
    Epsilon
};
const std::unordered_map<std::string, MutationStrategy> ms_lut{
    { "rand1", Rand1 }, { "best1", Best1 }, { "best2", Best2 }, { "rand-to-best1", RandToBest1 }
};
const std::unordered_map<std::string, CrossoverStrategy> cs_lut{
    { "bin", Bin }, { "exp", Exp }
};
const std::unordered_map<std::string, SelectionStrategy> ss_lut{
    { "static-penalty", StaticPenalty }, { "feasibility-rule", FeasibilityRule }, { "epsilon", Epsilon }
};
typedef std::vector<double>                    Solution;
typedef std::vector<std::pair<double, double>> Ranges;
typedef std::vector<double>                    ConstraintViolation;
typedef std::pair<double, ConstraintViolation> Evaluated;
typedef std::function<Evaluated(const size_t, const Solution&)> Objective;
class DE;
class IMutator {
public:
    virtual std::vector<Solution> mutation(const DE&) = 0;
    virtual double boundary_constraint(std::pair<double, double>, double) const noexcept;
    virtual ~IMutator() {}
};
class ICrossover {
public:
    virtual std::vector<Solution> crossover(const DE&, const std::vector<Solution>&, const std::vector<Solution>&) = 0;
    virtual ~ICrossover() {}
};
class ISelector {
public:
    virtual bool better(const Evaluated&, const Evaluated&) = 0;
    virtual std::pair<std::vector<Evaluated>
        , std::vector<Solution> > select(const DE& // There'll be a default implementation
        , const std::vector<Solution>&
        , const std::vector<Solution>&
        , const std::vector<Evaluated>&
        , const std::vector<Evaluated>&);
    virtual ~ISelector() {}
};
