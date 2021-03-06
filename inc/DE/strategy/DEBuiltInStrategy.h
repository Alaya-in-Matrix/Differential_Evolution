#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include "DEInterface.h"
class Mutator_Rand_1 : public IMutator
{
public:
    Solution mutation_solution(const DE&, size_t);
};
class Mutator_Rand_2 : public IMutator
{
public:
    Solution mutation_solution(const DE&, size_t);
};
class Mutator_Best_1 : public IMutator
{
public:
    Solution mutation_solution(const DE&, size_t);
};
class Mutator_Best_2 : public IMutator
{
public:
    Solution mutation_solution(const DE&, size_t);
};
class Mutator_RandToBest_1 : public IMutator
{
public:
    Solution mutation_solution(const DE&, size_t);
};
class Mutator_RandToBest_2 : public IMutator
{
public:
    Solution mutation_solution(const DE&, size_t);
};
class Mutator_CurrentToRand_1 : public IMutator
{
public:
    Solution mutation_solution(const DE&, size_t);
};
class Crossover_Bin : public ICrossover
{
public:
    Solution crossover_solution(const DE&, const Solution&, const Solution&);
};
class Crossover_Exp : public ICrossover
{
public:
    Solution crossover_solution(const DE&, const Solution&, const Solution&);
};
class Selector_StaticPenalty : public ISelector
{
public:
    bool better(const Evaluated&, const Evaluated&);
};
class Selector_FeasibilityRule : public ISelector
{
public:  // perhaps it would be better if this class inherits Selector_Epsilon and set epsilon_0 = 0
    bool better(const Evaluated&, const Evaluated&);
};
class Selector_Epsilon : public ISelector
{
    const double theta;
    const double cp;
    const size_t tc;
    double epsilon_0;
    double epsilon_level;

public:
    bool better(const Evaluated&, const Evaluated&);
    std::pair<std::vector<Evaluated>, std::vector<Solution> > select(const DE&,
                                                                     const std::vector<Solution>&,
                                                                     const std::vector<Solution>&,
                                                                     const std::vector<Evaluated>&,
                                                                     const std::vector<Evaluated>&);
    Selector_Epsilon(double theta, double cp, size_t tc) : theta(theta), cp(cp), tc(tc)
    {
        if (theta < 0 || theta > 1)
        {
            std::cerr << "theta should be in [0, 1]" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
};
