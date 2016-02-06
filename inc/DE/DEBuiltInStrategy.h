#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include "DEInterface.h"
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
