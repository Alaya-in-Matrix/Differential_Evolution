#include "Optimizer.h"
#include "hspice_util.h"
#include "DifferentialEvolution.h"
#include "Config.h"
#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <cassert>
#include <limits>
#include <exception>
#include <system_error>
#include <functional>
#include <memory>
#include <boost/algorithm/string.hpp>
using namespace std;
Optimizer::Optimizer(const Config& opt_info)
    : _opt_info(opt_info), _opt_func(gen_opt_func()), _de_solver(de_factory())
{}
Optimizer::~Optimizer()
{
    if (_de_solver != nullptr)
        delete _de_solver;
}
void Optimizer::init()
{
    const string out_dir          = _opt_info.out_dir();
    const string workspace        = _opt_info.workspace();
    const string circuit_dir      = _opt_info.circuit_dir();
    const unsigned int population = _opt_info.population();
    try
    {
        int ret;
        string cmd;
        cmd = "mkdir -p " + out_dir;
        ret = system(cmd.c_str());
        if (ret != 0) throw runtime_error("Fail to execute: " + cmd);

        cmd = "mkdir -p " + workspace;
        ret = system(cmd.c_str());
        if (ret != 0) throw runtime_error("Fail to execute: " + cmd);

        cmd = "mkdir -p " + workspace;
        ret = system(cmd.c_str());
        if (ret != 0) throw runtime_error("Fail to execute: " + cmd);

        for (unsigned int i = 0; i < population; ++i)
        {
            string path = workspace + "/" + to_string(i);
            cmd = "mkdir -p " + path + " && cp -r " + circuit_dir + "/* " + path;
            ret = system(cmd.c_str());
            if (ret != 0) throw runtime_error("Fail to execute: " + cmd);
        }
        cout << "Init done" << endl;
    }
    catch (exception& e)
    {
        cerr << "Exception: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
DE* Optimizer::de_factory() const noexcept
{
    DE* de_solver;
    if (_opt_info.de_type() == "DE")
    {
        de_solver = new DE(_opt_func, _opt_info.get_para_ranges(),
                           _opt_info.mutation_strategy(),
                           _opt_info.crossover_strategy(),
                           _opt_info.selection_strategy(), _opt_info.de_f(),
                           _opt_info.de_cr(), _opt_info.population(),
                           _opt_info.iter_num(), _opt_info.extra_conf());
    }
    else if (_opt_info.de_type() == "DERandomF")
    {
        de_solver = new DERandomF(
            _opt_func, _opt_info.get_para_ranges(),
            _opt_info.mutation_strategy(), _opt_info.crossover_strategy(),
            _opt_info.selection_strategy(), _opt_info.de_f(), _opt_info.de_cr(),
            _opt_info.population(), _opt_info.iter_num(),
            _opt_info.extra_conf());
    }
    else if (_opt_info.de_type() == "SaDE")
    {
        de_solver = new SaDE(_opt_func, _opt_info.get_para_ranges(), _opt_info.population(),
                             _opt_info.iter_num(), _opt_info.selection_strategy(),
                             _opt_info.extra_conf());
    }
    else
    {
        cerr << "Unsupported DE variant: " << _opt_info.de_type() << endl;
        exit(EXIT_FAILURE);
    }
    return de_solver;
}
vector<double> Optimizer::run()
{
    vector<double> solution = _de_solver->solver();
    return solution;
}
Objective Optimizer::gen_opt_func() const
{
    shared_ptr<int> iter_counter = make_shared<int>(0);
	return [&, iter_counter](const size_t idx, const vector<double>& input) -> pair<double, vector<double>>
	{

		vector<double> c_violation;
		double fom = numeric_limits<double>::infinity();
		const auto measured = simulation(idx, input);
		const auto meas_failed = measured.find("failed");

#pragma omp atomic
		*iter_counter += 1;

        if (meas_failed == measured.end() || meas_failed->second == 0)
        {
            const string fom_name   = _opt_info.fom_name();
            const double fom_weight = _opt_info.fom_direction_weight();
            if (measured.find(fom_name) == measured.end())
            {
                cerr << "fom " + fom_name + " is not measured" << endl;
                exit(EXIT_FAILURE);
            }
            fom = measured.find(fom_name)->second * fom_weight;
            const auto constraints      = _opt_info.constraints();
            const auto constr_w         = _opt_info.constraints_weight();
            const double penalty_weight = _opt_info.penalty_weight();
            assert(constraints.size()  == constr_w.size());
            c_violation.reserve(constraints.size());
            double total_violation = 0;
            for (auto c_pair : constraints)
            {
                // All constraints are converted to "less-than" constraints
                const string c_name   = c_pair.first;
                const auto weight_p   = constr_w.find(c_name);
                const double c_weight = weight_p == constr_w.end() ? 1 : weight_p->second;
                const auto m_pair     = measured.find(c_name);
                if (m_pair == measured.end())
                {
                    cerr << "Constraint variable " << c_name << " is not measured" << endl;
                    exit(EXIT_FAILURE);
                }
                const double m_value = c_weight * m_pair->second;
                const double c_value = c_weight * c_pair.second;
                double violation;
                if(std::isnan(m_value))
                    violation = numeric_limits<double>::infinity();
                else
                    violation = m_value <= c_value ? 0 : penalty_weight * (m_value - c_value);
                assert(violation >= 0);
                c_violation.push_back(violation);
                total_violation += violation;
            }
            fom = std::isnan(fom) ? numeric_limits<double>::infinity() : fom;
            // assert(c_violation >= 0);
            #pragma omp critical
            {
                printf("Iter: %d, Population Idx: %ld, ", *iter_counter, idx);
                for (auto p : measured)
                {
                    if (p.first != "failed")
                    {
                        printf("%s = %.2f, ", p.first.c_str(), p.second);
                    }
                }
                printf("constraint violation = %g, fom: %g\n", total_violation, fom);
            }
            if (total_violation == 0)
            {
                const string para_path = _opt_info.workspace() + "/" + to_string(idx) + "/" + _opt_info.para_file();
                const string out_path  = _opt_info.out_dir() + "/" + "good_" + to_string(*iter_counter) + "_" + to_string(idx) + "_" + to_string(fom);
                const string cmd       = "cp " + para_path + " " + out_path;
                const int ret = system(cmd.c_str());
                if (ret != 0)
                {
                    cerr << "fail to execute: " << cmd << endl;
                }
            }
        }
        else
        {
            #pragma omp critical
            {
                printf("Iter: %d, Population Idx: %ld, failed\n", *iter_counter, idx);
            }
        }
        fflush(stdout);
        return make_pair(fom, c_violation);
    };
}
unordered_map<string, double> Optimizer::simulation(unsigned int pop_idx, const vector<double>& params) const
{
    const string workspace = _opt_info.workspace() + "/" + to_string(pop_idx);
    const string testbench = _opt_info.testbench();
    const string sim_tool  = _opt_info.sim_tool();
    const string para_path = workspace + "/" + _opt_info.para_file();
    const string sim_cmd   = "cd " + workspace
                             + " && "
                             + sim_tool + " " + testbench + " > output.info 2>&1";

    unordered_map<string, vector<string>> measured_vars = _opt_info.measured_vars();
    unordered_map<string, double> measured;
    gen_param(_opt_info.get_para_names(), params, para_path);
    int ret = system(sim_cmd.c_str());
    if (ret == 0)
    {
        for (auto meas_p : measured_vars)
        {
            const string meas_file              = meas_p.first;
            const vector<string> meas_var_names = meas_p.second;
            const auto  tmp_measured            = parse_hspice_measure_file(workspace + "/" + meas_file);
            if (tmp_measured.find("failed") != tmp_measured.end() && tmp_measured.find("failed")->second.at(0) == 1)
            {
                measured.clear();
                measured["failed"] = 1;
                break;
            }
            else
            {
                measured["failed"] = 0;
                for (const string var_name : meas_var_names)
                {
                    if (measured.find(var_name) != measured.end())
                    {
                        cerr << var_name << " has been measured in more than one file" << endl;
                        exit(EXIT_FAILURE);
                    }
                    if (tmp_measured.find(var_name) == tmp_measured.end())
                    {
                        cerr << var_name << " is not measured in your testbench" << endl;
                        exit(EXIT_FAILURE);
                    }
                    vector<double> meas_vec = tmp_measured.find(var_name)->second;
                    for (auto& v : meas_vec)
                    {
                        if (std::isnan(v))
                        {
                            v = _opt_info.lookup_onfail(var_name);
                        }
                    }
                    measured[var_name] = _opt_info.process_measured(var_name, meas_vec);
                }
            }
        }
    }
    else
    {
        assert(measured.empty());
        measured["failed"] = 1;
    }
    return measured;
}
