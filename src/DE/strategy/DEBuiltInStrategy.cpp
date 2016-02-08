#include "DifferentialEvolution.h"
#include "util.h"
#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;
static mt19937_64 engine(random_device{}());
vector<Solution> Mutator_Rand_1::mutation(const DE& de)
{
    const vector<Solution>& population = de.population();
    assert(population.size() == de.np());
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    vector<Solution> mutated = population;
    for (Solution& m : mutated)
    {
        size_t r1 = random_exclusive<size_t>(i_distr);
        size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t> {r1});
        size_t r3 = random_exclusive<size_t>(i_distr, vector<size_t> {r1, r2});
        // de.f() might give a random number
        double f  = de.f();
        for (size_t i = 0; i < de.dimension(); ++i)
        {
            m[i] = population[r1][i] + f * (population[r2][i] - population[r3][i]);
            m[i] = boundary_constraint(de.range(i), m[i]);
        }
    }
    return mutated;
}
vector<Solution> Mutator_Best_1::mutation(const DE& de)
{
    const vector<Solution>& population = de.population();
    assert(population.size() == de.np());
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    vector<Solution> mutated = population;
    size_t best_idx = de.find_best();
    for (Solution& m : mutated)
    {
        size_t r1 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx});
        size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx, r1});
        // de.f() might give a random number
        double f  = de.f(); 
        for (size_t i = 0; i < de.dimension(); ++i)
        {
            m[i] = population[best_idx][i] + f * (population[r1][i] - population[r2][i]);
            m[i] = boundary_constraint(de.range(i), m[i]);
        }
    }
    return mutated;
}
vector<Solution> Mutator_Best_2::mutation(const DE& de)
{
    const vector<Solution>& population = de.population();
    assert(population.size() == de.np());
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    vector<Solution> mutated = population;
    size_t best_idx = de.find_best();
    for (Solution& m : mutated)
    {
        size_t r1 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx});
        size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx, r1});
        size_t r3 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx, r1, r2});
        size_t r4 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx, r1, r2, r3});
        double f1 = de.f();
        double f2 = de.f();
        for (size_t i = 0; i < de.dimension(); ++i)
        {
            m[i] = population[best_idx][i] + f1 * (population[r1][i] - population[r2][i]) + f2 * (population[r3][i] - population[r4][i]);
            m[i] = boundary_constraint(de.range(i), m[i]);
        }
    }
    return mutated;
}
vector<Solution> Mutator_RandToBest_1::mutation(const DE& de)
{
    const vector<Solution>& population = de.population();
    assert(population.size() == de.np());
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    vector<Solution> mutated = population;
    size_t best_idx = de.find_best();
    for (Solution& m : mutated)
    {
        size_t r1 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx});
        size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx, r1});
        double f1 = de.f();
        double f2 = de.f();
        for (size_t i = 0; i < de.dimension(); ++i)
        {
            m[i] = m[i] + f1 * (population[best_idx][i] - m[i]) + f2 * (population[r1][i] - population[r2][i]);
            m[i] = boundary_constraint(de.range(i), m[i]);
        }
    }
    return mutated;
}
vector<Solution> Crossover_Bin::crossover(const DE& de, const vector<Solution>& targets, const vector<Solution>& doners)
{
    assert(targets.size() == de.np() && de.np() == doners.size());
    uniform_int_distribution<size_t> distr_j(0, de.dimension() - 1);
    uniform_real_distribution<double> distr_ij(0, 1);
    const double cr         = de.cr();
    const size_t dimension  = de.dimension();
    vector<Solution> trials = targets;
    for (size_t i = 0; i < de.np(); ++i)
    {
        assert(targets[i].size() == dimension && dimension == doners[i].size());
        const size_t jrand = distr_j(engine);
        for (size_t j = 0; j < dimension; ++j)
        {
            trials[i][j] = distr_ij(engine) <= cr || jrand == j ? doners[i][j] : targets[i][j];
        }
    };
    return trials;
}
vector<Solution> Crossover_Exp::crossover(const DE& de, const vector<Solution>& targets, const vector<Solution>& doners)
{
    assert(targets.size() == de.np() && de.np() == doners.size());
    const double cr         = de.cr();
    const size_t dim        = de.dimension();
    uniform_int_distribution<size_t>  int_distr(0, dim - 1);
    uniform_real_distribution<double> real_distr(0, 1);
    vector<Solution> trials = targets;
    for (size_t i = 0; i < de.np(); ++i)
    {
        assert(targets[i].size() == dim && dim == doners[i].size());
        size_t l = 1;
        for (; real_distr(engine) <= cr && l < dim; ++l) {}
        const size_t start_idx = int_distr(engine);
        for (size_t j = start_idx; j < start_idx + l; ++j)
        {
            trials[i][j % dim] = doners[i][j % dim];
        }
    }
    return trials;
}
pair<vector<Evaluated>, vector<Solution>> Selector_Epsilon::select(const DE& de
                                       , const vector<Solution>& targets
                                       , const vector<Solution>& trials
                                       , const vector<Evaluated>& target_results
                                       , const vector<Evaluated>& trial_results)
{
    if (de.curr_gen() == 1)
    {
        vector<double> violations(de.np(), numeric_limits<double>::infinity());
        for (size_t i = 0; i < target_results.size(); ++i)
            violations[i] = accumulate(target_results[i].second.begin(), target_results[i].second.end(), 0.0);
        size_t cutoff = (size_t)(de.np() * theta);
        partial_sort(violations.begin(), violations.begin() + cutoff, violations.end());
        epsilon_0     = violations[cutoff - 1];
        epsilon_level = epsilon_0;
    }
    auto ret      = ISelector::select(de, targets, trials, target_results, trial_results);
    size_t gen    = de.curr_gen();
    cout << "Epsilon level: " << epsilon_level << endl;
    epsilon_level = gen > tc ? 0 : epsilon_0 * pow(1.0 - (double)gen / (double)tc, (double)cp);
    return ret;
}
bool Selector_StaticPenalty::better(const Evaluated& r1, const Evaluated& r2)
{
    const double fom1 = r1.first + accumulate(r1.second.begin(), r1.second.end(), 0.0);
    const double fom2 = r2.first + accumulate(r2.second.begin(), r2.second.end(), 0.0);
    return fom1 <= fom2;
}
bool Selector_FeasibilityRule::better(const Evaluated& r1, const Evaluated& r2)
{
    const double fom1       = r1.first;
    const double fom2       = r2.first;
    const double violation1 = accumulate(r1.second.begin(), r1.second.end(), 0.0);
    const double violation2 = accumulate(r2.second.begin(), r2.second.end(), 0.0);
    if (violation1 == 0 && violation2 == 0)
        return fom1 <= fom2;
    else if (violation1 != 0 && violation2 != 0)
        return violation1 == violation2 ? fom1 <= fom2 : violation1 <= violation2;
    else
        return violation1 == 0;
}
bool Selector_Epsilon::better(const Evaluated& r1, const Evaluated& r2)
{
    const double fom1       = r1.first;
    const double fom2       = r2.first;
    const double violation1 = accumulate(r1.second.begin(), r1.second.end(), 0.0);
    const double violation2 = accumulate(r2.second.begin(), r2.second.end(), 0.0);
    if (violation1 <= epsilon_level && violation2 <= epsilon_level)
        return fom1 <= fom2;
    else if (violation1 > epsilon_level && violation2 > epsilon_level)
        return violation1 == violation2 ? fom1 <= fom2 : violation1 <= violation2;
    else
        return violation1 <= epsilon_level;
}
