#include "DifferentialEvolution.h"
#include <iostream>
#include <vector>
#include <functional>
#include <utility>
using namespace std;

Evaluated quad(const vector<double>& input)
{
    double result = 0;
    for (auto x : input)
        result += x * x;
    return make_pair(result, vector<double> {});
}

int main()
{
    vector<pair<double, double>> ranges{ make_pair(-10, 10)
                                         , make_pair(-10, 10)
                                         , make_pair(-10, 10)};
    DE de_solver(quad
                 , ranges
                 , MutationStrategy::Best1
                 , CrossoverStrategy::Exp
                 , SelectionStrategy::StaticPenalty
                 , 0.8
                 , 0.8
                 , 10
                 , 100
                );
    Solution best = de_solver.solver();

    for (auto para : best)
    {
        cout << para << endl;
    }
    return 0;
}
