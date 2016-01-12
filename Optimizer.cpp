#include <cstdio>
#include <iostream>
#include <omp.h>
#include <string>
#include <cassert>
#include <limits>
#include <exception>
#include <system_error>
#include <functional>
#include <unordered_map>
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
        if (ret != 0) throw new runtime_error("Fail to execute: " + cmd);

        cmd = "mkdir -p " + workspace;
        ret = system(cmd.c_str());
        if (ret != 0) throw new runtime_error("Fail to execute: " + cmd);

        cmd = "mkdir -p " + workspace;
        ret = system(cmd.c_str());
        if (ret != 0) throw new runtime_error("Fail to execute: " + cmd);

        for (unsigned int i = 0; i < population; ++i)
        {
            string path = workspace + "/" + to_string(i);
            cmd = "mkdir -p " + path + " && cp " + circuit_dir + "/* " + path;
            ret = system(cmd.c_str());
            if (ret != 0) throw new runtime_error("Fail to execute: " + cmd);
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
        auto meas_failed = measured.find("failed");
        if(meas_failed == measured.end() || meas_failed->second == 0)
        {
            const string fom_name   = _opt_info.fom_name();
            const double fom_weight = _opt_info.fom_direction_weight();
            if(measured.find(fom_name) == measured.end())
            {
                cerr << "fom " + fom_name + " is not measured" << endl;
                exit(EXIT_FAILURE);
            }
            fom = measured.find(fom_name)->second * fom_weight;
            const auto constraints      = _opt_info.constraints();
            const auto constr_w         = _opt_info.constraint_direction_weight();
            const double penalty_weight = _opt_info.penalty_weight();
            const double normalizer     = _opt_info.constraint_normalizer();
            assert(constraints.size()  == constr_w.size());
            for(auto c_pair : constraints)
            {
                string c_name        = c_pair.first;
                double c_value       = c_pair.second;
                const auto weight_p  = constr_w.find(c_name);
                assert(weight_p != constr_w.end());
                int c_weight         = weight_p->second;
                const auto m_pair = measured.find(c_name);
                if(m_pair == measured.end())
                {
                    cerr << "Constraint variable " << c_name << " is not measured" << endl;
                    exit(EXIT_FAILURE);
                }
                double m_value = m_pair->second;
                m_value *= c_weight;
                c_value *= c_weight;
                double penalty = m_value <= c_value ? 0 : m_value - c_value;
                penalty *= (normalizer / c_value);
                penalty *= penalty_weight;
                assert(penalty > 0);
                fom += penalty;
            }
        }
        return fom;
    };
}
unordered_map<string, double> Optimizer::simulation(unsigned int pop_idx, const vector<double>& params) const
{
    const string workspace    = _opt_info.workspace() + "/" + to_string(pop_idx);
    const string para_path    = workspace + "/" + _opt_info.para_file();
    const string netlist_path = workspace + "/" + _opt_info.testbench();
    const string output_path  = workspace + "/" + "output.info";
    const string sim_cmd      = _opt_info.sim_tool() + " " + netlist_path + " -o " + workspace + " >  " + output_path + " 2>&1";
    unordered_map<string, vector<string>> measured_vars = _opt_info.measured_vars();
    unordered_map<string, double> measured;
    gen_param(_opt_info.get_para_names(), params, para_path);
    int ret = system(sim_cmd.c_str());
    if (ret == 0)
    {
        for(auto meas_p : measured_vars)
        {
            const string meas_file              = meas_p.first;
            const vector<string> meas_var_names = meas_p.second;
            const auto  tmp_measured            = parse_hspice_measure_file(meas_file);
            if(tmp_measured.find("failed") != tmp_measured.end())
            {
                measured.clear();
                measured["failed"] = 1;
                break;
            }
            else
            {
                for(const string var_name : meas_var_names)
                {
                    if(measured.find(var_name) != measured.end())
                    {
                        cerr << var_name << " has been measured in more than one file" << endl;
                        exit(EXIT_FAILURE);
                    }
                    if(tmp_measured.find(var_name) == tmp_measured.end())
                    {
                        cerr << var_name << " is not measured in your testbench" << endl;
                        exit(EXIT_FAILURE);
                    }
                    measured[var_name] = tmp_measured.find(var_name)->second;
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
