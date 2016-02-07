#pragma once
#include "DE.h"
class DERandomF : public DE
{
    double _fsigma; // F ~ N(_f, _fsigma)
public:
    using DE::DE;
    double f()  const noexcept;
};
