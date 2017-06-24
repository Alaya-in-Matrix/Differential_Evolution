# README

* Author: lvwenlong_lambda@qq.com
* Last Modified: 2017/06/24-11:13:23

## Build and Install

Dependency:
* CMake, with a minimum version of 3.2.1
* Boost, and if you installed boost in a custom directory, you have to specify it by `-DBOOST_ROOT=yore/boost/root`

Below is an examle to install:

```bash
cmake -DBOOST_ROOT=~/.softwares/boost -DCMAKE_INSTALL_PREFIX=~/mysoft/de
make install
```

## Features

Basic DE mutation strategies:

```cpp
enum MutationStrategy
{
    Rand1 = 0,
    Rand2, 
    Best1,
    Best2,
    CurrentToRand1, 
    RandToBest1, 
    RandToBest2
};
```
Crossover strategies:

```cpp
enum CrossoverStrategy
{
    Bin = 0,
    Exp
};
```

Selection strategies, to handle constraints:

```cpp
enum SelectionStrategy
{
    StaticPenalty = 0,
    FeasibilityRule,
    Epsilon
};
```

- Epsilon: [Takahama, Tetsuyuki, and Setsuko Sakai. "Constrained optimization by the ε constrained differential evolution with an archive and gradient-based mutation." Evolutionary Computation (CEC), 2010 IEEE Congress on. IEEE, 2010.](http://ieeexplore.ieee.org/abstract/document/5586484/)
- FeasibilityRule: [Mezura-Montes, Efrén, Carlos A. Coello Coello, and Edy I. Tun-Morales. "Simple feasibility rules and differential evolution for constrained optimization." Mexican International Conference on Artificial Intelligence. Springer Berlin Heidelberg, 2004.](https://pdfs.semanticscholar.org/e90d/c00b726b01d3da39d39bd5182278c15f13af.pdf)
- User-defined mutation/crossover/selection strategy is also supported.

Two DE variants are implemented:

- DERandomF: Original DE, but the parameter F in each iteration is a random variable following gaussian distribution
- SaDE: [Qin, A. Kai, Vicky Ling Huang, and Ponnuthurai N. Suganthan. "Differential evolution algorithm with strategy adaptation for global numerical optimization." IEEE transactions on Evolutionary Computation 13.2 (2009): 398-417.](http://ieeexplore.ieee.org/abstract/document/4632146/)

My recommendation:

- DERandomF
- Best1/Bin/FeasibilityRule
- F ~ N(0.75, 0.25)
- CR = 0.8

## Example

```cpp
#include "DifferentialEvolution.h"
#include <iostream>
#include <vector>
#include <utility>
#include <map>
using namespace std;
int main()
{
    Objective objf = [](const size_t, const vector<double>& xs)->Evaluated{
        double fom = 0.0; 
        for(auto v : xs)
            fom += v * v;
        vector<double> constr_vios = {}; // no constraints
        return {fom, constr_vios};
    };

    const vector<pair<double, double>> ranges{{-3, 3}, {-5, 5}};
    const MutationStrategy ms  = MutationStrategy::Best1;
    const CrossoverStrategy cs = CrossoverStrategy::Bin;
    const SelectionStrategy ss = SelectionStrategy::FeasibilityRule; // How to handle constraints
    const double F             = 0.8;
    const double CR            = 0.8;
    const size_t population    = 10;
    const size_t max_iter      = 100; // total evaluation: max_iter * population
    const map<string, double> extra_conf = {};

    DE de1(objf, ranges); // use default setting
    DE de2(objf, ranges, ms, cs, ss, F, CR, population, max_iter, extra_conf);
    Solution sol1 = de1.solver();
    Solution sol2 = de2.solver();

    return EXIT_SUCCESS;
}
```
