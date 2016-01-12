#ifndef __OPTIMIZER_H__
#define __OPTIMIZER_H__
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "Evolution.h"
#include "Config.h"
class Optimizer
{
    const Config _opt_info;
    DESolver* _de_solver;
    std::function<double(unsigned int, const std::vector<double>&)> gen_opt_func() const;
    std::unordered_map<std::string, double> simulation(unsigned int, const std::vector<double>&) const;
public:
    Optimizer(const Config&);
    void init();
    std::vector<double> run();
    ~Optimizer();
};
#endif
