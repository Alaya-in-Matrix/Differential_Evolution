#include "global.h"
#include <random>
using namespace std;
#ifndef NDEBUG // debug mode
mt19937_64 engine(0);
#else // release mode
mt19937_64 engine(random_device{}());
#endif
