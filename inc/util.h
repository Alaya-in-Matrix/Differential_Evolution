#pragma once
#include <vector>
#include <random>
template<typename Integral>
Integral random_exclusive(std::uniform_int_distribution<Integral>& distr, const std::vector<Integral>& exclude = std::vector<Integral>{})
{
    using namespace std;
    static mt19937_64 engine(std::random_device{}());
    Integral ret;
    bool replicate = false;
    do
    {
        ret       = distr(engine);
        replicate = false;
        for (const auto v : exclude)
        {
            if (ret == v)
            {
                replicate = true;
                break;
            }
        }
    }
    while (replicate);
    return ret;
}
