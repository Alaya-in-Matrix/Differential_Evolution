#pragma once
#include <unordered_map>
#include <string>
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
const std::unordered_map<std::string, MutationStrategy> ms_lut
{
    {"rand1", Rand1}
    , {"best1", Best1}
    , {"best2", Best2}
    , {"rand-to-best1", RandToBest1}
};
const std::unordered_map<std::string, CrossoverStrategy> cs_lut
{
    {"bin", Bin}
    , {"exp", Exp}
};
const std::unordered_map<std::string, SelectionStrategy> ss_lut
{
    {"static-penalty", StaticPenalty}
    , {"feasibility-rule", FeasibilityRule}
    , {"epsilon", Epsilon}
};
