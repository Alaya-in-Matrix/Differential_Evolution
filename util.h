#ifndef __UTIL_H__
#define __UTIL_H__
#include <vector>
#include <random>
template<typename Integral>
Integral random_exclusive(std::uniform_int_distribution<Integral>& distr, const std::vector<Integral> exclude)
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

template<typename Integral>
Integral random_exclusive(std::uniform_int_distribution<Integral>&& distr, const std::vector<Integral> exclude)
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
#endif
