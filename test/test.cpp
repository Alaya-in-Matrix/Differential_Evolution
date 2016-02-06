#include "testbench.h"
#include "DifferentialEvolution.h"
#include <iostream>
#include <vector>
#include <functional>
#include <utility>
using namespace std;

int main()
{
    Objective f = Testbench::ackley;
    vector<double> best_guess{0, 0};
    vector<pair<double, double>> ranges{ make_pair(-10, 10)
                                         , make_pair(-10, 10)
                                       };
    assert(best_guess.size() == ranges.size());
    DE de_solver(f
                 , ranges
                 , MutationStrategy::Best1
                 , CrossoverStrategy::Bin
                 , SelectionStrategy::StaticPenalty
                 , 0.8
                 , 0.8
                 , 100
                 , 500
                );
    Solution best = de_solver.solver();
    cout << "Best guess: " << f(0, best_guess).first << endl;
    cout << "DE Optimized: " << f(0, best).first << endl;
    for (auto para : best)
    {
        cout << para << endl;
    }
    return 0;
}
