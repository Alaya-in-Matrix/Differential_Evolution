#ifndef __DE_H__
#define __DE_H__
#include <vector>
#include <functional>
#include <utility>
#include <random>
class DESolver
{
    // maybe I shouldn't use 2d vector, I should use 2D array?
    typedef std::vector<std::vector<double> > Vec2D;
    typedef std::vector<std::pair<double, double> > RangeVec;
    const std::function<double(const std::vector<double>&)> _func;
    const RangeVec _ranges;
    const unsigned int _iter_num;
    const unsigned int _para_num;
    const unsigned int _init_num;

    // three parameters to be tuned
    const double _cr;
    const double _fmu;
    const double _fsigma;


    Vec2D _candidates;
    std::pair<int, double> _find_best(const Vec2D&) const noexcept;
    Vec2D _mutation(const Vec2D&) const noexcept;
    Vec2D _crossover(const Vec2D&, const Vec2D&) const noexcept;
    Vec2D _selection(const Vec2D&, const Vec2D&) const noexcept;

public:
    DESolver( std::function<double(const std::vector<double>&)>
              , RangeVec
              , unsigned int iter_num
              , unsigned int para_num
            );
    std::vector<double> solver();
};

#endif
