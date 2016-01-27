#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <functional>
#include "Evolution.h"
#include "Config.h"
class Optimizer
{
    const Config _opt_info;
    EpsilonDE_Best_1* _de_solver;
    std::function<std::pair<double, double>(unsigned int, const std::vector<double>&)> gen_opt_func() const;
    std::unordered_map<std::string, double> simulation(unsigned int, const std::vector<double>&) const;
public:
    Optimizer(const Config&);
    void init();
    std::vector<double> run();
    ~Optimizer();
};
