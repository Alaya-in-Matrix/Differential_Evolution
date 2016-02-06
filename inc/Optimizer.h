#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <functional>
#include "Config.h"
#include "DifferentialEvolution.h"
class Optimizer
{
    const Config _opt_info;
    DE* _de_solver;
    Objective gen_opt_func() const;
    std::unordered_map<std::string, double> simulation(unsigned int, const std::vector<double>&) const;
public:
    Optimizer(const Config&);
    void init();
    std::vector<double> run();
    ~Optimizer();
};
