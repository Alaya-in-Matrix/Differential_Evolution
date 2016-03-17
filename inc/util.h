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
    assert(exclude.size() < (distr.b() - distr.a() + 1));
    Integral ret;
    bool replicate = false;
    do
    {
        ret       = distr(engine);
        replicate = std::find(exclude.begin(), exclude.end(), ret) != exclude.end();
    } while (replicate);
    return ret;
}
