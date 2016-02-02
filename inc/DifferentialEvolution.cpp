#include "DifferentialEvolution.h"
#include "util.h"
#include <random>
#include <omp.h>
#include <cassert>
#include <numeric>
using namespace std;
mt19937_64 engine(random_device{}());
Solution operator-(const Solution s1, const Solution s2)
{
    assert(s1.size() == s2.size());
    vector<double> result = s1;
    for (size_t i = 0; i < s1.size(); ++i)
    {
        result[i] -= s2[i];
    }
    return result;
}
Solution operator+(const Solution s1, const Solution s2)
{
    assert(s1.size() == s2.size());
    vector<double> result = s1;
    for (size_t i = 0; i < s1.size(); ++i)
    {
        result[i] += s2[i];
    }
    return result;
}
Solution operator*(double f, const Solution s)
{
    vector<double> result = s;
    for (size_t i = 0; i < s.size(); ++i)
    {
        result[i] *= f;
    }
    return result;
}
vector<Solution> Mutator_Rand_1::mutation(const DE& de, const vector<Solution>& population)
{
    assert(population.size() == de.np());
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    vector<Solution> mutated = population;
    for (Solution& m : mutated)
    {
        size_t r1 = random_exclusive<size_t>(i_distr);
        size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t> {r1});
        size_t r3 = random_exclusive<size_t>(i_distr, vector<size_t> {r1, r2});
        m = population[r1] + de.f() * (population[r2] - population[r3]);
    }
    return mutated;
}
vector<Solution> Mutator_Best_1::mutation(const DE& de, const vector<Solution>& population)
{
    assert(population.size() == de.np());
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    vector<Solution> mutated = population;
    size_t best_idx = de.find_best(population);
    for (Solution& m : mutated)
    {
        size_t r1 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx});
        size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx, r1});
        m = population[best_idx] + de.f() * (population[r1] - population[r2]);
    }
    return mutated;
}
vector<Solution> Mutator_Best_2::mutation(const DE& de, const vector<Solution>& population)
{
    assert(population.size() == de.np());
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    vector<Solution> mutated = population;
    size_t best_idx = de.find_best(population);
    for (Solution& m : mutated)
    {
        size_t r1 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx});
        size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx, r1});
        size_t r3 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx, r1, r2});
        size_t r4 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx, r1, r2, r3});
        m = population[best_idx] + de.f() * (population[r1] - population[r2]) + de.f() * (population[r3] - population[r4]);
    }
    return mutated;
}
vector<Solution> Mutator_RandToBest_1::mutation(const DE& de, const vector<Solution>& population)
{
    assert(population.size() == de.np());
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    vector<Solution> mutated = population;
    size_t best_idx = de.find_best(population);
    for (Solution& m : mutated)
    {
        size_t r1 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx});
        size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t> {best_idx, r1});
        m = m + de.f() * (population[best_idx] - m) + de.f() * (population[r1] - population[r2]);
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
    uniform_int_distribution<size_t>  int_distr(0, de.dimension() - 1);
    uniform_real_distribution<double> real_distr(0, 1);
    const double cr         = de.cr();
    const size_t dim        = de.dimension();
    vector<Solution> trials = targets;
    for (size_t i = 0; i < de.np(); ++i)
    {
        assert(targets[i].size() == dim && dim == doners[i].size());
        size_t l = 1;
        for (; real_distr(engine) <= cr && l < dim; ++l) {}
        const size_t start_idx = int_distr(engine);
        for (size_t j = start_idx; j < start_idx + l; ++j)
        {
            trials[i][j] = doners[i][j];
        }
    }
    return trials;
}
pair<vector<Evaluated>, vector<Solution>> ISelector::select(const DE& de
        , const vector<Solution>& targets
        , const vector<Solution>& trials
        , const vector<Evaluated>& target_results
        , const vector<Evaluated>& trial_results)
{
    assert(targets.size() == de.np() && de.np() == trials.size());
    assert(target_results.size() == de.np() && de.np() == trial_results.size());
    vector<Solution> offspring(targets);
    vector<Evaluated> child_results(target_results);
    for (size_t i = 0; i < de.np(); ++i)
    {
        if (better(trial_results[i], target_results[i]))
        {
            copy(trials[i].begin(), trials[i].end(), offspring[i].begin());
            child_results[i] = trial_results[i];
        }
    }
    return make_pair(child_results, offspring);
}
pair<vector<Evaluated>, vector<Solution>> Selector_Epsilon::select(const DE& de
        , const vector<Solution>& targets
        , const vector<Solution>& trials
        , const vector<Evaluated>& target_results
        , const vector<Evaluated>& trial_results)
{
    auto ret        = ISelector::select(de, targets, trials, target_results, trial_results);
    size_t curr_gen = de.curr_gen();
    epsilon_level   = curr_gen > tc ? 0 : epsilon_0 * pow(1 - curr_gen / tc, cp);
    return ret;
}
bool Selector_StaticPenalty::better(const Evaluated& r1, const Evaluated& r2)
{
    const double fom1 = r1.first + accumulate(r1.second.begin(), r1.second.end(), 0);
    const double fom2 = r2.first + accumulate(r2.second.begin(), r2.second.end(), 0);
    return fom1 <= fom2;
}
bool Selector_FeasibilityRule::better(const Evaluated& r1, const Evaluated& r2)
{
    const double fom1       = r1.first;
    const double fom2       = r2.first;
    const double violation1 = accumulate(r1.second.begin(), r1.second.end(), 0);
    const double violation2 = accumulate(r2.second.begin(), r2.second.end(), 0);
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
    const double violation1 = accumulate(r1.second.begin(), r1.second.end(), 0);
    const double violation2 = accumulate(r2.second.begin(), r2.second.end(), 0);
    if (violation1 <= epsilon_level && violation2 <= epsilon_level)
        return fom1 <= fom2;
    else if (violation1 > epsilon_level && violation2 > epsilon_level)
        return violation1 == violation2 ? fom1 <= fom2 : violation1 <= violation2;
    else
        return violation1 <= epsilon_level;
}
