#include<cmath>
#include<vector>
#include<algorithm>
#include<functional>
#include<iterator>
#include"DifferentialEvolution.h"
namespace Testbench
{
using namespace std;
const double pi = 3.141592653589793238L;
Evaluated fuck(double r)
{
    return make_pair(r, vector<double> {});
}
Evaluated ackley(const vector<double>& input)
{
    // range: [-35, 35]
    // global optimum: x = (0, ....., 0)
    double dim = (double)(input.size());
    double square_sum = 0;
    for (auto x : input)
        square_sum += x * x;
    double part1 = -0.02 * sqrt(square_sum / dim);
    double cos_sum = 0;
    for (auto x : input)
        cos_sum += cos(2 * pi * x);
    double part2 = cos_sum / dim;
    double result = -20 * exp(part1) - exp(part2) + 20 + exp(1);
    return fuck(result);
}
Evaluated adjiman(const vector<double>& input)
{
    assert(input.size() == 2);
    // range: x1: [-1, 2], x2: [-1, 1]
    // global optimum: x = (2, 0.10578)
    double x1 = input[0];
    double x2 = input[1];
    return fuck(cos(x1) * sin(x2) - x1 / (1 + pow(x2, 2)));
}
Evaluated alpine(const vector<double>& input)
{
    // range: [-10, 10]
    // global optimum: (0, ..., 0) -> 0
    vector<double> mapped;
    transform(input.begin(), input.end(), std::back_inserter(mapped), [](double x) -> double
    {
        return abs(x * sin(x) + 0.1 * x);
    });
    return fuck(accumulate(mapped.begin(), mapped.end(), 0));
}
};
