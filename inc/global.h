#pragma once
#include <random>
// Random number engine
#ifdef NDEBUG // release
std::mt19937_64 engine(std::random_device{}());
#else
std::mt19937_64 engine(0);
#endif
