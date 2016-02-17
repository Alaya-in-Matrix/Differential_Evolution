#include "DifferentialEvolution.h"
#include "util.h"
#include <random>
#include <cassert>
using namespace std;
static mt19937_64 engine(random_device{}());
double IMutator::boundary_constraint(pair<double, double> rg, double val) const noexcept
{
    assert(rg.first <= rg.second);
    uniform_real_distribution<double> distr(rg.first, rg.second);
    return rg.first <= val && val <= rg.second ? val : distr(engine);
}
std::vector<Solution> IMutator::mutation(const DE& de) 
{
    vector<Solution> mutated;
    mutated.reserve(de.population().size());
    for(size_t i = 0; i < de.population().size(); ++i)
    {
        mutated.push_back(mutation_solution(de, i));
    }
    return mutated;
}
vector<Solution> ICrossover::crossover(const DE& de, const vector<Solution>& targets, const vector<Solution>& doners)
{
    vector<Solution> trials(de.np(), Solution(de.dimension()));
    for(size_t i = 0; i < de.np(); ++i)
    {
        trials[i] = crossover_solution(de, targets[i], doners[i]);
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
