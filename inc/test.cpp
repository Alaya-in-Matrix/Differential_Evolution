#include "DifferentialEvolution.h"
#include <iostream>
#include <vector>
#include <functional>
#include <utility>
#include <testbench.h>
using namespace std;

int main()
{
    Objective f = Testbench::alpine;
    vector<double> best_guess{0, 0, 0, 0, 0};
    vector<pair<double, double>> ranges{ make_pair(-10, 10)
                                         , make_pair(-10, 10)
                                         , make_pair(-10, 10)
                                         , make_pair(-10, 10)
                                         , make_pair(-10, 10)
                                       };
    assert(best_guess.size() == ranges.size());
    DE de_solver(f
                 , ranges
                 , MutationStrategy::Rand1
                 , CrossoverStrategy::Bin
                 , SelectionStrategy::StaticPenalty
                 , 0.8
                 , 1
                 , ranges.size() * 10
                 , 100
                );
    Solution best = de_solver.solver();
    cout << "Best guess: " << f(best_guess).first << endl;
    cout << "DE Optimized: " << f(best).first << endl;
    for (auto para : best)
    {
        cout << para << endl;
    }
    return 0;
}
