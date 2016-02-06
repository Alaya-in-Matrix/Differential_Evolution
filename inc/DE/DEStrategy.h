#pragma once
#include <vector>
#include <functional>
typedef std::vector<double> Solution;
typedef std::vector<std::pair<double, double>> Ranges;
typedef std::vector<double> ConstraintViolation;
typedef std::pair<double, ConstraintViolation> Evaluated;
typedef std::function<Evaluated(const Solution&)> Objective;
enum MutationStrategy
{
    Rand1 = 0
    , Best1
    , Best2
    , RandToBest1
};
enum CrossoverStrategy
{
    Bin = 0
    , Exp
};
enum SelectionStrategy
{
    StaticPenalty = 0
    , FeasibilityRule
    , Epsilon
};
