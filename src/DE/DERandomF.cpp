#include "DifferentialEvolution.h"
#include <random>
using namespace std;
static mt19937_64 engine(random_device{}());
double DERandomF::f() const noexcept
{
    auto fsigma_iter = _extra_conf.find("fsigma");
    double fsigma = 0;
    if(fsigma_iter != _extra_conf.end())
    {
        fsigma = fsigma_iter->second;
    }
    return normal_distribution<double>(_f, fsigma)(engine);
}
