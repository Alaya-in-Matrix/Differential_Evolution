#include "Config.h"
#include "hspice_util.h"
#include "DifferentialEvolution.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <utility>
#include <cassert>
#include <unordered_map>
#include <string>
#include <cmath>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace std;
using namespace boost::property_tree;
void Config::set_para()   // I really miss Maybe monad in haskell!
{
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
                throw invalid_argument(name + ": lb < ub");
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
void Config::set_opt_settings() noexcept
{
    _iter_num   = _info_tree.get("iter_num", 600);
    _para_num   = _para_names.size();
    _population = _info_tree.get("init_num", 10 * _para_num);
    _thread_num = _info_tree.get("thread_num", 1);
    _prj_dir    = _info_tree.get("prj_dir", ".") + "/";
    _out_dir    = _prj_dir + _info_tree.get("out_dir", "out");
    _workspace  = _prj_dir + _info_tree.get("workspace", "workspace");
}
void Config::set_algo_para()
{
    try
    {
        _f  = 0.8;
        _cr = 0.9;
        _ms = MutationStrategy::Best1;
        _cs = CrossoverStrategy::Bin;
        _ss = SelectionStrategy::StaticPenalty;
        _de_type    = "DE";
        _extra_conf = unordered_map<string, double>{};
        const auto& algo_setting_maybe = _info_tree.get_child_optional("algorithm");
        if(algo_setting_maybe == boost::none) 
            return;

        const auto& algo_setting = algo_setting_maybe.value();
        string ms_str = algo_setting.get("mutation", "best1");
        string cs_str = algo_setting.get("crossover", "bin");
        string ss_str = algo_setting.get("selection", "static-penalty");
        auto ms_iter  = ms_lut.find(ms_str);
        auto cs_iter  = cs_lut.find(cs_str);
        auto ss_iter  = ss_lut.find(ss_str);
        if(ms_iter == ms_lut.end())
            throw invalid_argument("Unrecognized mutation strategy: " + ms_str);
        if(cs_iter == cs_lut.end())
            throw invalid_argument("Unrecognized crossover strategy: " + cs_str);
        if(ss_iter == ss_lut.end())
            throw invalid_argument("Unrecognized selection strategy: " + ss_str);
        _de_type     = algo_setting.get("type", "DE");
        _f           = algo_setting.get("f", 0.8);
        _cr          = algo_setting.get("cr", 0.9);
        _ms = ms_iter->second;
        _cs = cs_iter->second;
        _ss = ss_iter->second;
        auto extra_setting = algo_setting.get_child_optional("extra");
        if(extra_setting != boost::none)
        {
            for(auto pr : extra_setting.value())
            {
                string name  = pr.first;
                double value = pr.second.get_value<double>();
                _extra_conf.insert(make_pair(name, value));
            }
        }
    }
    catch (const ptree_error& e)
    {
        cerr << "Exception: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    catch (const exception& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
void Config::set_sim_info()
{
    try
    {
        _para_file   = _info_tree.get<string>("simulation.para_file");
        _circuit_dir = _prj_dir + _info_tree.get<string>("simulation.circuit_dir");
        _testbench   = _info_tree.get<string>("simulation.testbench");
        _sim_tool    = _info_tree.get<string>("simulation.sim_tool");
        if (_supported_sim_tool.end() == find(_supported_sim_tool.begin(), _supported_sim_tool.end(), _sim_tool))
        {
            string err_msg = "Unsupported simulation tool: " + _sim_tool + ". ";
            err_msg += boost::algorithm::join(_supported_sim_tool, ", ");
            err_msg += ". ";
            throw invalid_argument(err_msg);
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
void Config::set_measured_vars()
{
    try
    {
        ptree meas_tree = _info_tree.get_child("measured");
        _measured_vars.clear();
        _measured_func.clear();
        _measured_vars_on_fail.clear();
        for (const auto& node : meas_tree)
        {
            const auto& node_tree = node.second;
            const string name      = node_tree.get<string>("name");
            const string meas_file = node_tree.get<string>("file");
            const string func_str  = node_tree.get<string>("func");
            const double on_fail   = node_tree.get<double>("onfail", numeric_limits<double>::quiet_NaN());
            if (_measured_vars.find(meas_file) == _measured_vars.end())
                _measured_vars[meas_file] = vector<string> {name};
            else
                _measured_vars[meas_file].push_back(name);

            if (_measured_func.find(name) != _measured_func.end())
                throw invalid_argument("variable " + name + " is measured twice!");
            _measured_func[name]         = func_str;
            _measured_vars_on_fail[name] = on_fail;
        }
    }
    catch (ptree_error& e)
    {
        cerr << "Fail to get measure info: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    catch (exception& e)
    {
        cerr << "Exception: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void Config::set_spec()
{
    auto get_opt_direction = [](string type) -> int
    {
        if (type == "minimize")
            return 1;
        else if (type == "maximize")
            return -1;
        else
        {
            throw invalid_argument("Invalid optimization direciton: " + type + ", supported: mimimize, maximize");
        }
    };
    try
    {
        const ptree spec_tree   = _info_tree.get_child("spec");
        const ptree fom_tree    = spec_tree.get_child("fom");
        const ptree constr_tree = spec_tree.get_child("constraints");
        _penalty_weight          = spec_tree.get<double>("penalty_weight");
        _fom_name = fom_tree.get<string>("name");
        string fom_type = fom_tree.get<string>("type");
        _fom_direction = get_opt_direction(fom_type);
        for (const auto& node : constr_tree)
        {
            const auto& node_tree = node.second;
            string name          = node_tree.get<string>("name");
            _constraints[name]   = node_tree.get<double>("value");
            _constr_weight[name] = node_tree.get<double>("weight") * get_opt_direction(node_tree.get<string>("type"));
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
Config::Config(string config, SpecFormat format)
{
    try
    {
        switch (format)
        {
        case Json:
            read_json(config, _info_tree);
            break;
        default:
            throw invalid_argument("Only support json format config file now");
        }
        set_para();
        set_opt_settings();
        set_sim_info();
        set_measured_vars();
        set_spec();
        set_algo_para();
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
Config::Config(const ptree& pt)
    : _info_tree(pt)
{
    try
    {
        set_para();
        set_opt_settings();
        set_sim_info();
        set_measured_vars();
        set_spec();
        set_algo_para();
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
double Config::process_measured(const string var_name, const vector<double>& data) const noexcept
{
    assert(! data.empty());
    const auto iter = _measured_func.find(var_name);
    if (iter == _measured_func.end())
    {
        cerr << "function can not find for variable " << var_name << endl;
        exit(EXIT_FAILURE);
    }
    const string func_str = iter->second;
    bool no_nan = data.end() == find_if(data.begin(), data.end(), [](double x) -> bool
    {
        return x != x; // check whether a floating number is NaN
    });
    if (func_str == "min")
    {
        return no_nan ? *(min_element(data.begin(), data.end())) : numeric_limits<double>::quiet_NaN();
    }
    else if (func_str == "max")
    {
        return no_nan ? *(max_element(data.begin(), data.end())) : numeric_limits<double>::quiet_NaN();
    }
    else if (func_str == "head")
    {
        return data[0];
    }
    else
    {
        cerr << "Unsupported function: " << func_str << endl;
        exit(EXIT_FAILURE);
    }
}
double Config::lookup_onfail(const string var_name) const noexcept
{
    auto iter = _measured_vars_on_fail.find(var_name);
    if (iter == _measured_vars_on_fail.end())
    {
        return numeric_limits<double>::quiet_NaN();
    }
    else
    {
        return iter->second;
    }
}
