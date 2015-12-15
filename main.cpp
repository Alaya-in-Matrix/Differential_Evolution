#include <iostream>
#include <cstdio>
#include <vector>
#include <functional>
#include <cassert>
#include <utility>
#include <cmath>

#include "Evolution.h"
using namespace std;

double testFunc(const vector<double>& vec);
double testFunc2(const vector<double>& vec);

int main()
{
    const unsigned int iter_num = 100;
    const unsigned int para_num = 2;

    vector<pair<double, double>> ranges{make_pair(0.01, 20), make_pair(0.01, 20)};
    DESolver desolver(testFunc, ranges, iter_num, para_num);
    vector<double> solution = desolver.solver();
    printf("(x, y) = (%g, %g)\nResult = %g\n", solution[0], solution[1], testFunc(solution));

    vector<pair<double,double>> ranges2{make_pair(-1, 1), make_pair(-1, 1)};
    DESolver desolver2(testFunc2,ranges2,iter_num,para_num);
    vector<double> solution2 = desolver2.solver();
    printf("(x, y) = (%g, %g)\nResult = %g\n", solution2[0], solution2[1], testFunc2(solution2));

    return 0;
}

double testFunc(const vector<double>& vec)
{
    assert(vec.size() == 2);
    const double pi = 3.14159265358979323846;
    const double x  = vec[0];
    const double y  = vec[1];
    return sin(y / x * pi) * cos(x / y * pi) * exp(-1*x/y);
}

double testFunc2(const vector<double>& vec)
{
    assert(vec.size() == 2);
    const double pi = 3.14159265358979323846;
    const double x  = vec[0];
    const double y  = vec[1];
    return exp(sin(50*x)) + sin(60*exp(y)) + sin(70 * sin(x)) + sin(sin(80*y)) - sin(10 * (x+y)) + 0.25 * (x*x + y*y);
}
