#pragma once
#include <vector>
#include <functional>
#include <utility>
#include <random>
class DESolver
{
    // maybe I shouldn't use 2d vector, I should use 2D array?
protected:
    typedef std::vector<std::vector<double> > Vec2D;
    typedef std::vector<std::pair<double, double> > RangeVec;
    const std::function<std::pair<double, double>(unsigned int idx, const std::vector<double>&)> _func;
    const RangeVec _ranges;
    const unsigned int _iter_num;
    const unsigned int _para_num;
    const unsigned int _init_num;

    // three parameters to be tuned
    // Not const, as they might be self-adaptive
    const double _cr;
    const double _fmu;
    const double _fsigma;


    Vec2D _candidates;
    // fom and constraint violation
    std::vector<std::pair<double, double>> _results;
    bool _better(const std::pair<double, double>& p1, const std::pair<double, double>& p2) const noexcept;
    size_t _find_best(const Vec2D&) const noexcept;
    Vec2D _mutation(const Vec2D&) const noexcept;
    Vec2D _crossover(const Vec2D&, const Vec2D&) const noexcept;
    void _selection(const Vec2D&, const Vec2D&) noexcept;

public:
    DESolver( std::function <std::pair<double, double>(unsigned int idx, const std::vector<double>&)> f
              , RangeVec rg
              , unsigned int iter_num
              , unsigned int para_num
              , unsigned int init_num
              , double cr     = 0.8
              , double fmu    = 0.75
              , double fsigma = 0.25
            );
    std::vector<double> solver();
};
