#include "testbench.h"
#include "DifferentialEvolution.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <string>
#include <functional>
using namespace std;
void test_all_mutation_strategy(size_t);
void test_SaDE(size_t);
int main()
{
    test_all_mutation_strategy(5);
    test_SaDE(5);
    return 0;
}
void test_all_mutation_strategy(size_t dim)
{
    // use 3D Ackley function to test all mutation strategies
    // use bin crossover
    vector<string> m_names{"Rand1",          "Rand2",       "Best1",      "Best2",
                           "CurrentToRand1", "RandToBest1", "RandToBest2"};
    vector<MutationStrategy> pool{Rand1,          Rand2,       Best1,      Best2,
                                  CurrentToRand1, RandToBest1, RandToBest2};
    CrossoverStrategy c = Bin;
    SelectionStrategy s = StaticPenalty;
    Objective f = Testbench::ackley;
    const vector<double> best_guess(dim, 0);
    const vector<pair<double, double>> ranges(dim, {-10, 10});
    const double global_best = f(0, best_guess).first;
    for (size_t i = 0; i < pool.size(); ++i)
    {
        DERandomF test_solver(f, ranges, pool[i], c, s, 0.75, 0.8, 10 * dim, 600,
                              {{"fsigma", 0.25}});
        const double calc_best = f(0, test_solver.solver()).first;
        cout << m_names[i]
             << ", delta between global and solved: " 
             << fabs(global_best - calc_best) << endl;
    }
}
void test_SaDE(size_t dim)
{
    // use dim-D Ackley function to test SaDE
    // use bin crossover
    Objective f = Testbench::ackley;
    const vector<double> best_guess(dim, 0);
    const vector<pair<double, double>> ranges(dim, {-10, 10});
    const double global_best = f(0, best_guess).first;
    const unordered_map<string, double> extra{
        {"lp", 50}, {"fmu", 0.75}, {"fsigma", 0.25}, {"crmu", 0.8}, {"crsigma", 0.1},
    };
    SaDE sade(f, ranges, dim * 10, 600, StaticPenalty, extra);
    const Solution solved = sade.solver();
    const double solved_fom = f(0, solved).first;
    cout << "SaDE, delta between global and solved: " << fabs(global_best - solved_fom) << endl;
}
