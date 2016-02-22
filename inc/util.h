#pragma once
#include "global.h"
#include <iostream>
#include <vector>
#include <random>
#include <string>
template <typename Integral>
Integral random_exclusive(std::uniform_int_distribution<Integral>& distr,
                          const std::vector<Integral>& exclude = std::vector<Integral>{})
{
    using namespace std;
    if (exclude.size() >= (distr.b() - distr.a() + 1))
    {
        cerr << "random_exclusive, exclusive list greater than random range!" << endl;
        exit(EXIT_FAILURE);
    }
    Integral ret;
    bool replicate = false;
    do
    {
        ret = distr(engine);
        replicate = false;
        for (const auto v : exclude)
        {
            if (ret == v)
            {
                replicate = true;
                break;
            }
        }
    } while (replicate);
    return ret;
}
