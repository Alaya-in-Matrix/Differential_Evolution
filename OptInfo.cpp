#include <cstdio>
#include <iostream>
#include <fstream>
#include <utility>
#include <cassert>
#include <unordered_map>
#include <string>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "OptInfo.h"
#include "hspice_util.h"
using namespace std;
using namespace boost::property_tree;
void OptInfo::set_para()
{
    // I really miss Maybe monad in haskell!
    ptree para_tree;
    _para_names.clear();
    _ranges.clear();
    try
    {
        para_tree = _info_tree.get_child("parameter");
        _para_names.reserve(para_tree.size());
        _ranges.reserve(para_tree.size());
        for (auto para = para_tree.begin(); para !=  para_tree.end(); ++para)
        {
            string name = para->second.get<string>("name");
            double lb   = para->second.get<double>("lb");
            double ub   = para->second.get<double>("ub");
            _para_names.push_back(name);
            _ranges.push_back(make_pair(lb, ub));
            if (lb > ub)
                throw new invalid_argument(name + ": lb < ub");
        }
    }
    catch (ptree_error& e)
    {
        cerr << "Fail to get parameters: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    catch (invalid_argument& e)
    {
        cerr << "Logic error in parameters: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    catch (exception& e)
    {
        cerr << "Unknown exception: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
void OptInfo::set_opt_settings() noexcept
{
    _iter_num   = _info_tree.get("iter_num", 600);
    _para_num   = _para_names.size();
    _population = _info_tree.get("init_num", 10 * _para_num);
    _out_dir    = _info_tree.get("out_dir", "out");
    _workspace  = _info_tree.get("workspace", "workspace");
    _thread_num = _info_tree.get("thread_num", 1);
}
void OptInfo::set_sim_info()
{
    try
    {
        _para_file   = _info_tree.get<string>("simulation.para_file");
        _circuit_dir = _info_tree.get<string>("simulation.circuit_dir");
        _testbench   = _info_tree.get<string>("simulation.testbench");
        _sim_tool    = _info_tree.get<string>("simulation.sim_tool");
        if (_supported_sim_tool.end() == find(_supported_sim_tool.begin(), _supported_sim_tool.end(), _sim_tool))
        {
            string err_msg = "Unsupported simulation tool: " + _sim_tool + ". ";
            err_msg += boost::algorithm::join(_supported_sim_tool, ", ");
            err_msg += ". ";
            throw new invalid_argument(err_msg);
        }
    }
    catch (ptree_error& e)
    {
        cerr << "Fail to get sim info: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    catch (invalid_argument& e)
    {
        cerr << "Invalid argument: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
void OptInfo::set_measured_vars()
{
    try
    {
        ptree meas_tree = _info_tree.get_child("measured");
        _measured_vars.clear();
        _measured_vars.reserve(meas_tree.size());
        for (auto node = meas_tree.begin(); node != meas_tree.end(); ++node)
        {
            const string meas_file = node->second.get<string>("file");
            const ptree var_list   = node->second.get_child("meas");
            vector<string> meas_var_names;
            for(auto var : var_list)
            {
                meas_var_names.push_back(var.second.get_value<string>());
            }
            _measured_vars[meas_file] = meas_var_names;
        }
    }
    catch (ptree_error& e)
    {
        cerr << "Fail to get measure info: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void OptInfo::set_spec()
{
    auto get_opt_direction = [](string type) -> int
    {
        if (type == "minimize")
            return 1;
        else if (type == "maximize")
            return -1;
        else
        {
            throw new invalid_argument("Invalid optimization direciton: " + type + ", supported: mimimize, maximize");
        }
    };
    try
    {
        const ptree spec_tree   = _info_tree.get_child("spec");
        const ptree fom_tree    = spec_tree.get_child("fom");
        const ptree constr_tree = spec_tree.get_child("constraints");
        _normalizer              = spec_tree.get<double>("normalizer");
        _penalty_weight          = spec_tree.get<double>("penalty_weight");
        _fom_name = fom_tree.get<string>("name");
        string fom_type = fom_tree.get<string>("type");
        _fom_direction = get_opt_direction(fom_type);
        for (const auto& node : constr_tree)
        {
            const auto& node_tree = node.second;
            string name              = node_tree.get<string>("name");
            _constraints[name]       = node_tree.get<double>("value");
            _constr_directions[name] = get_opt_direction(node_tree.get<string>("type"));
        }
    }
    catch (ptree_error& e)
    {
        cerr << "Fail to get spec: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    catch (exception& e)
    {
        cerr << "Exception: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
OptInfo::OptInfo(string config, SpecFormat format)
{
    try
    {
        switch (format)
        {
        case Json:
            read_json(config, _info_tree);
            break;
        default:
            throw new invalid_argument("Only support json format config file now");
        }
        set_para();
        set_opt_settings();
        set_sim_info();
        set_measured_vars();
        set_spec();
    }
    catch (ptree_error& e)
    {
        cerr << "Fail to parse json: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    catch (exception& e)
    {
        cerr << "Exception: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
OptInfo::OptInfo(const ptree& pt)
    : _info_tree(pt)
{
    try
    {
        set_para();
        set_opt_settings();
        set_sim_info();
        set_measured_vars();
        set_spec();
    }
    catch (ptree_error& e)
    {
        cerr << "Fail to parse json: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    catch (exception& e)
    {
        cerr << "Exception: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
vector<string> OptInfo::get_para_names() const noexcept
{
    return _para_names;
}
vector<pair<double, double>> OptInfo::get_para_ranges() const noexcept
{
    return _ranges;
}
unsigned int OptInfo::iter_num() const noexcept
{
    return _iter_num;
}
unsigned int OptInfo::para_num() const noexcept
{
    return _para_num;
}
unsigned int OptInfo::population() const noexcept
{
    return _population;
}
unsigned int OptInfo::thread_num() const noexcept
{
    return _thread_num;
}
string OptInfo::out_dir() const noexcept
{
    return _out_dir;
}
string OptInfo::workspace() const noexcept
{
    return _workspace;
}
string OptInfo::sim_tool() const noexcept
{
    return _sim_tool;
}
string OptInfo::para_file() const noexcept
{
    return _para_file;
}
string OptInfo::circuit_dir() const noexcept
{
    return _circuit_dir;
}
string OptInfo::testbench() const noexcept
{
    return _testbench;
}
unordered_map<string, vector<string>> OptInfo::measured_vars() const noexcept
{
    return _measured_vars;
}
double OptInfo::penalty_weight() const noexcept
{
    return _penalty_weight;
}
double OptInfo::constraint_normalizer() const noexcept
{
    return _normalizer;
}
string OptInfo::fom_name() const noexcept
{
    return _fom_name;
}
int OptInfo::fom_direction_weight() const noexcept
{
    return _fom_direction;
}
std::unordered_map<std::string, double> OptInfo::constraints() const noexcept
{
    return _constraints;
}
std::unordered_map<std::string, int> OptInfo::constraint_direction_weight() const noexcept
{
    return _constr_directions;
}
void OptInfo::print() const noexcept
{
    printf("iter num: %d\n", _iter_num);
    printf("dimension: %d\n", _para_num);
    printf("population: %d\n", _population);
    printf("thread_num: %d\n", _thread_num);
    printf("output directory: %s\n", _out_dir.c_str());
    printf("workspace: %s\n", _workspace.c_str());
    printf("circuit: %s/%s\n", _circuit_dir.c_str(), _testbench.c_str());
    printf("sim tool: %s\n", _sim_tool.c_str());
    puts("==================================================================================");
    assert(_para_names.size() == _ranges.size());
    assert(_para_names.size() == _para_num);
    for(size_t i = 0; i < _para_num; ++i)
    {
        printf("param %s = (%.2f ~ %.2f)\n", _para_names[i].c_str(), _ranges[i].first, _ranges[i].second);
    }
    puts("==================================================================================");
    printf("measured variables: \n");
    for(auto meas_p : _measured_vars)
    {
        string file = meas_p.first;
        printf("\t%s:\n", file.c_str());
        for(auto var : meas_p.second)
        {
            printf("\t\t%s\n", var.c_str());
        }
    }
    puts("==================================================================================");
    assert(_constraints.size() == _constr_directions.size());
    for(auto constr_pair : _constraints)
    {
        string name      = constr_pair.first;
        double value     = constr_pair.second;
        auto c_direction = _constr_directions.find(name);
        assert(c_direction != _constr_directions.end());
        string cmp_str   = c_direction->second == 1 ? "<" : ">";
        printf("constraint %s %s %g\n", name.c_str(), cmp_str.c_str(), value);
    }
    printf("FOM: %s %s\n", _fom_direction == 1 ? "minimize" : "maximize", _fom_name.c_str());
}
