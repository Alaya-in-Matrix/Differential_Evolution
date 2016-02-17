#include "DifferentialEvolution.h"
#include "util.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cassert>
using namespace std;
static mt19937_64 engine(random_device{}());
Solution Mutator_Rand_1::mutation_solution(const DE& de, size_t curr_idx)
{
    const vector<Solution>& population = de.population();
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    Solution mutated = population[curr_idx];
    size_t r1 = random_exclusive<size_t>(i_distr);
    size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t>{r1});
    size_t r3 = random_exclusive<size_t>(i_distr, vector<size_t>{r1, r2});
    double f  = de.f();
    for (size_t i = 0; i < de.dimension(); ++i)
    {
        mutated[i] = boundary_constraint(
                de.range(i),
                population[r1][i] + f * (population[r2][i] - population[r3][i]));
    }
    return mutated;
}
Solution Mutator_Rand_2::mutation_solution(const DE& de, size_t curr_idx)
{
    const vector<Solution>& population = de.population();
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    Solution mutated = population[curr_idx];
    size_t r1 = random_exclusive<size_t>(i_distr);
    size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t> {r1});
    size_t r3 = random_exclusive<size_t>(i_distr, vector<size_t> {r1, r2});
    size_t r4 = random_exclusive<size_t>(i_distr, vector<size_t> {r1, r2, r3});
    size_t r5 = random_exclusive<size_t>(i_distr, vector<size_t> {r1, r2, r3, r4});
    double f1 = de.f();
    double f2 = de.f();
    for (size_t i = 0; i < de.dimension(); ++i)
    {
        mutated[i] = boundary_constraint(
            de.range(i), population[r1][i] + f1 * (population[r2][i] - population[r3][i]) 
                                           + f2 * (population[r4][i] - population[r5][i]));
    }
    return mutated;
}
Solution Mutator_Best_1::mutation_solution(const DE& de, size_t)
{
    const vector<Solution>& population = de.population();
    const size_t best_idx       = de.find_best();
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    const size_t r1 = random_exclusive<size_t>(i_distr, vector<size_t>{best_idx});
    const size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t>{best_idx, r1});
    const double f  = de.f();
    Solution mutated(de.dimension());
    for (size_t i = 0; i < de.dimension(); ++i)
    {
        mutated[i] = boundary_constraint(
                de.range(i), population[best_idx][i] + f * (population[r1][i] - population[r2][i]));
    }
    return mutated;
}
Solution Mutator_Best_2::mutation_solution(const DE& de, size_t)
{
    const vector<Solution>& population = de.population();
    const size_t best_idx = de.find_best();
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    const size_t r1 = random_exclusive<size_t>(i_distr, vector<size_t>{best_idx});
    const size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t>{best_idx, r1});
    const size_t r3 = random_exclusive<size_t>(i_distr, vector<size_t>{best_idx, r1, r2});
    const size_t r4 = random_exclusive<size_t>(i_distr, vector<size_t>{best_idx, r1, r2, r3});
    const double f1 = de.f();
    const double f2 = de.f();
    Solution mutated(de.dimension());
    for (size_t i = 0; i < de.dimension(); ++i)
    {
        mutated[i] = boundary_constraint(
                de.range(i), population[best_idx][i] + f1 * (population[r1][i] - population[r2][i]) 
                + f2 * (population[r3][i] - population[r4][i]));
    }
    return mutated;
}
Solution Mutator_CurrentToRand_1::mutation_solution(const DE& de, size_t curr_idx)
{
    assert(curr_idx < de.population().size());
    const vector<Solution>& population = de.population();
    uniform_int_distribution<size_t>  i_distr(0, population.size() - 1);
    uniform_real_distribution<double> k_distr(0, 1);
    Solution mutated(population[curr_idx]);
    const size_t r1 = random_exclusive<size_t>(i_distr);
    const size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t>{r1});
    const size_t r3 = random_exclusive<size_t>(i_distr, vector<size_t>{r1, r2});
    const double f  = de.f();
    const double k  = k_distr(engine);
    for (size_t i = 0; i < de.dimension(); ++i)
    {
        mutated[i] = boundary_constraint(
                de.range(i), mutated[i] + k * (population[r1][i] - mutated[i]) +
                f * (population[r2][i] - population[r3][i]));
    }
    return mutated;
}
Solution Mutator_RandToBest_1::mutation_solution(const DE& de, size_t curr_idx)
{
    const vector<Solution>& population = de.population();
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    Solution mutated(population[curr_idx]);
    const size_t best_idx = de.find_best();
    const size_t r1 = random_exclusive<size_t>(i_distr, vector<size_t>{best_idx});
    const size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t>{best_idx, r1});
    const double f1 = de.f();
    const double f2 = de.f();
    for(size_t i = 0; i < de.dimension(); ++i)
    {
        mutated[i] = boundary_constraint(de.range(i),
                                         mutated[i] + f1 * (population[best_idx][i] - mutated[i]) 
                                                    + f2 * (population[r1][i] - population[r2][i]));
    }
    return mutated;
}
Solution Mutator_RandToBest_2::mutation_solution(const DE& de, size_t curr_idx)
{
    const vector<Solution>& population = de.population();
    uniform_int_distribution<size_t> i_distr(0, population.size() - 1);
    Solution mutated(population[curr_idx]);
    const size_t best_idx = de.find_best();
    const size_t r1 = random_exclusive<size_t>(i_distr, vector<size_t>{best_idx});
    const size_t r2 = random_exclusive<size_t>(i_distr, vector<size_t>{best_idx, r1});
    const size_t r3 = random_exclusive<size_t>(i_distr, vector<size_t>{best_idx, r1, r2});
    const size_t r4 = random_exclusive<size_t>(i_distr, vector<size_t>{best_idx, r1, r2, r3});
    const double f1 = de.f();
    const double f2 = de.f();
    const double f3 = de.f();
    for(size_t i = 0; i < de.dimension(); ++i)
    {
        mutated[i] = boundary_constraint(de.range(i),
                                         mutated[i] + f1 * (population[best_idx][i] - mutated[i]) 
                                                    + f2 * (population[r1][i] - population[r2][i])
                                                    + f3 * (population[r3][i] - population[r4][i]));
    }
    return mutated;
}
Solution Crossover_Bin::crossover_solution(const DE& de, const Solution& target, const Solution& doner)
{
    const double cr  = de.cr();
    const size_t dim = de.dimension();
    uniform_int_distribution<size_t> distr_idx(0, dim - 1);
    uniform_real_distribution<double> distr_prob(0, 1);
    Solution trial(dim);
    const size_t rand_idx = distr_idx(engine);
    for (size_t i = 0; i < dim; ++i)
    {
        trial[i] = distr_prob(engine) <= cr || i == rand_idx ? doner[i] : target[i];
    }
    return trial;
}
Solution Crossover_Exp::crossover_solution(const DE& de, const Solution& target, const Solution& doner)
{
    const double cr  = de.cr();
    const size_t dim = de.dimension();
    assert(target.size() == dim && dim == doner.size());
    uniform_int_distribution<size_t>  distr_idx(0, dim - 1);
    uniform_real_distribution<double> distr_prob(0, 1);
    Solution trial(target);
    size_t l = 1;
    for(; distr_prob(engine) < cr && l < dim; ++l);
    const size_t start_idx = distr_idx(engine);
    for(size_t i = start_idx; i < start_idx + l; ++i)
    {
        trial[i % dim] = doner[i % dim];
    }
    return trial;
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
