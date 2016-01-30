#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <utility>
#include <random>
#include <string>
enum DE_Stragety {
    BEST_1_BIN = 0 
    , RAND_1_BIN 
    , TARGET_TO_RAND_1_BIN 
    , TARGET_TO_BEST_1_BIN 
};

class DESolver
{
    // Default DE solver
    // DE/best/1
    // F is a random number following normal distribution
    // Static Function inequality constraint handling
    // Random Reintialize boundary constraint handling
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

    const DE_Stragety _strategy;

    Vec2D _candidates;
    // fom and constraint violation
    std::vector<std::pair<double, double>> _results;
    virtual Vec2D  _mutation(const Vec2D&) const noexcept;
    virtual Vec2D  _crossover(const Vec2D&, const Vec2D&) const noexcept;
    virtual void   _selection(const Vec2D&, const Vec2D&) noexcept;

    virtual bool   _better(const std::pair<double, double>& p1, const std::pair<double, double>& p2) const noexcept;
    virtual size_t _find_best(const Vec2D&) const noexcept;
    virtual std::pair<size_t, std::vector<double>> _mutation_base(const Vec2D&) const noexcept;
    virtual std::string _show_strategy() const noexcept;
    virtual void _report_best() const noexcept;

public:
    virtual ~DESolver() {}
    DESolver( std::function <std::pair<double, double>(unsigned int idx, const std::vector<double>&)> f
              , RangeVec rg
              , unsigned int iter_num
              , unsigned int para_num
              , unsigned int init_num
              , DE_Stragety stragety = BEST_1_BIN
              , double cr            = 0.8
              , double fmu           = 0.75
              , double fsigma        = 0.25
            );
    virtual std::vector<double> solver();
};
class FeasibilityRuleDE : public DESolver
{
protected:
    bool _better(const std::pair<double, double>& p1, const std::pair<double, double>& p2) const noexcept;
public:
    ~FeasibilityRuleDE(){}
    using DESolver::DESolver;
};
class EpsilonDE : public DESolver
{
protected:
    double epsilon_0;
    double epsilon_level;
    const double theta, cp;
    const unsigned int tc;
    unsigned int curr_gen;
    void init_epsilon();
    void update_epsilon();
    bool _better(const std::pair<double, double>& p1, const std::pair<double, double>& p2) const noexcept;
    void _report_best() const noexcept;
public:
    ~EpsilonDE() {}
    EpsilonDE( std::function <std::pair<double, double>(unsigned int idx, const std::vector<double>&)> f
                      , RangeVec rg
                      , unsigned int iter_num
                      , unsigned int para_num
                      , unsigned int init_num
                      , DE_Stragety stragety = BEST_1_BIN
                      , double cr            = 0.8
                      , double fmu           = 0.75
                      , double fsigma        = 0.25
                      , double theta         = 0.2
                      , double tc_rate       = 0.1
                      , double cp            = 2
                    )
        : DESolver(f, rg, iter_num, para_num, init_num, stragety, cr, fmu, fsigma)
        , theta(theta)
        , cp(cp)
        , tc((unsigned int)floor(tc_rate * iter_num))
        , curr_gen(0)
    {}
    std::vector<double> solver();
};
