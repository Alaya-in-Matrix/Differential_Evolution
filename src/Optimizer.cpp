#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>
#include <cassert>
#include <omp.h>
#include <limits>
#include <exception>
#include <system_error>
#include <functional>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include "hspice_util.h"
#include "Evolution.h"
#include "Config.h"
#include "Optimizer.h"
using namespace std;
Optimizer::Optimizer(const Config& opt_info)
    : _opt_info(opt_info), _de_solver(nullptr)
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
            cmd = "mkdir -p " + path + " && cp " + circuit_dir + "/* " + path;
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
vector<double> Optimizer::run()
{
    assert(_de_solver == nullptr);
    const auto opt_func         = gen_opt_func();
    const auto ranges           = _opt_info.get_para_ranges();
    const unsigned int iter_num = _opt_info.iter_num();
    const unsigned int para_num = _opt_info.para_num();
    const unsigned int init_num = _opt_info.population();
    _de_solver = new DESolver(opt_func, ranges, iter_num, para_num, init_num);
    vector<double> solution = _de_solver->solver();
    return solution;
}
function<double(unsigned int, const vector<double>&)> Optimizer::gen_opt_func() const
{
    return [&](unsigned int idx,  const vector<double>& input) -> double
    {

        double fom = numeric_limits<double>::infinity();
        const auto measured = simulation(idx, input);
        const auto meas_failed    = measured.find("failed");

        static int iter_counter = 0;
        #pragma omp atomic
        iter_counter = iter_counter + 1;

        if (meas_failed == measured.end() || meas_failed->second == 0)
        {
            printf("Iter: %d, Population Idx: %d, ", iter_counter, idx);
            for (auto p : measured)
            {
                if (p.first != "failed")
                {
                    printf("%s = %.2f, ", p.first.c_str(), p.second);
                }
            }
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
            double penalty = 0;
            for (auto c_pair : constraints)
            {
                const string c_name       = c_pair.first;
                double c_value      = c_pair.second;
                const auto weight_p = constr_w.find(c_name);
                assert(weight_p != constr_w.end());
                const double c_weight     = weight_p->second;
                const auto m_pair = measured.find(c_name);
                if (m_pair == measured.end())
                {
                    cerr << "Constraint variable " << c_name << " is not measured" << endl;
                    exit(EXIT_FAILURE);
                }
                double m_value = m_pair->second;
                m_value *= c_weight;
                c_value *= c_weight;
                const double tmp_penalty = m_value <= c_value ? 0 : m_value - c_value;
                penalty += tmp_penalty;
            }
            penalty = std::isnan(penalty) ? numeric_limits<double>::infinity() : penalty;
            fom     = std::isnan(fom) ? numeric_limits<double>::infinity() : fom; 
            penalty *= penalty_weight;
            fom += penalty;
            assert(penalty >= 0);
            printf("penalty = %g, fom: %g\n", penalty, fom);
            if (penalty == 0)
            {
                const string para_path = _opt_info.workspace() + "/" + to_string(idx) + "/" + _opt_info.para_file();
                const string out_path  = _opt_info.out_dir() + "/" + "good_" + to_string(iter_counter) + "_" + to_string(idx) + "_" + to_string(fom);
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
            printf("Iter: %d, Population Idx: %d, failed\n", iter_counter, idx);
        }
        fflush(stdout);
        return fom;
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
