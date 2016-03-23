#pragma once
#include "global.h"
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <cassert>
template <typename Integral>
Integral random_exclusive(std::uniform_int_distribution<Integral>& distr,
                          const std::vector<Integral>& exclude = std::vector<Integral>{})
{
    // assert(exclude.size() < (distr.b() - distr.a() + 1));
    Integral lb = distr.a();
    Integral ub = distr.b();
    bool all_excluded = true;
    for (size_t i = lb; i <= ub; ++i)
    {
        if (std::find(exclude.begin(), exclude.end(), i) == exclude.end())
        {
            all_excluded = false;
            break;
        }
    }
    assert(all_excluded == false);

    Integral rand_val;
    bool replicate = false;
    do
    {
        rand_val  = distr(engine);
        replicate = std::find(exclude.begin(), exclude.end(), rand_val) != exclude.end();
    } while (replicate);
    return rand_val;
}
