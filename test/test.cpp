#include "testbench.h"
#include "DifferentialEvolution.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <string>
#include <functional>
using namespace std;
void test_all_mutation_strategy();

int main()
{
    test_all_mutation_strategy();
    return 0;
}
void test_all_mutation_strategy()
{
    // use 3D Ackley function to test all mutation strategies
    // use bin crossover
    vector<string> m_names{"Rand1",          "Rand2",       "Best1",      "Best2",
                           "CurrentToRand1", "RandToBest1", "RandToBest2"};
    vector<MutationStrategy> pool{Rand1,          Rand2,       Best1,      Best2,
                                  CurrentToRand1, RandToBest1, RandToBest2};
    CrossoverStrategy c = Exp;
    SelectionStrategy s = StaticPenalty;
    Objective f = Testbench::ackley;
    vector<double> best_guess{0, 0};
    vector<pair<double, double>> ranges{make_pair(-10, 10), make_pair(-10, 10)};
    const double global_best = f(0, best_guess).first;
    for (size_t i = 0; i < pool.size(); ++i)
    {
        DERandomF test_solver(f, ranges, pool[i], c, s, 0.75, 0.8, 100, 200,
                              unordered_map<string, double>{{"fsigma", 0.25}});
        const double calc_best = f(0, test_solver.solver()).first;
        cout << m_names[i]
             << ", delta between global and best-so-far: " 
             << fabs(global_best - calc_best) << endl;
    }
}
