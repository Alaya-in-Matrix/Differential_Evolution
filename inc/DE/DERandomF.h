#pragma once
#include "DEOrigin.h"
#include <unordered_map>
#include <string>
#include <vector>
class DERandomF : public DE {
public:
    using DE::DE;
    double f() const noexcept;
};
