#pragma once
#include <cstdio>
#include <iostream>
#include <fstream>
#include <utility>
#include <cassert>
#include <unordered_map>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
class Config
{
    using ptree = boost::property_tree::ptree;
    // boost property tree;
    ptree _info_tree;
    // parameters
    std::vector<std::string> _para_names;
    std::vector<std::pair<double, double>> _ranges;

    // DE settings and working directories
    unsigned int _iter_num; 
    unsigned int _para_num;
    unsigned int _population;
    unsigned int _thread_num;
    std::string  _out_dir;
    std::string  _workspace;

    // sim info
    std::string _para_file; // You should ".inc" this para_file in your testbench file
    std::string _circuit_dir;
    std::string _testbench; 
    std::string _sim_tool;
    const std::vector<std::string> _supported_sim_tool{"hspice", "hspice64", "hspicerf64"};

    // measured variables
    std::unordered_map<std::string, std::vector<std::string>> _measured_vars;
    std::unordered_map<std::string, std::string> _measured_func;

    // spec
    // usage: auto w = OptDirectionWeight[Minimize]...
    int    _penalty_weight;
    std::string _fom_name;
    double _fom_direction;
    std::unordered_map<std::string, double> _constraints;
    std::unordered_map<std::string, double> _constr_weight; 

    void set_para();
    void set_opt_settings() noexcept; //these settings have default value, so no exception would be thrown
    void set_sim_info();
    void set_measured_vars();
    void set_spec();
public:
    // I perhaps should use TOML format config file
    // as it is more human-readable
    enum SpecFormat { Json = 0, TOML }; 
    Config(std::string, SpecFormat = Json);
    Config(const ptree& );
    void print() const noexcept;
    decltype(_para_names) get_para_names() const noexcept;
    decltype(_ranges)     get_para_ranges() const noexcept;;
    unsigned int iter_num() const noexcept;
    unsigned int para_num() const noexcept;
    unsigned int population() const noexcept;
    unsigned int thread_num() const noexcept;
    std::string out_dir() const noexcept;
    std::string workspace() const noexcept;
    std::string para_file() const noexcept;
    std::string circuit_dir() const noexcept;
    std::string testbench() const noexcept;
    std::string sim_tool() const noexcept;
    std::unordered_map<std::string, std::vector<std::string>> measured_vars() const noexcept;
    double penalty_weight() const noexcept;
    std::string fom_name() const noexcept;
    int fom_direction_weight() const noexcept;
    std::unordered_map<std::string, double> constraints() const noexcept;
    std::unordered_map<std::string, double> constraints_weight() const noexcept;
    double process_measured(const std::string, const std::vector<double>&) const noexcept;
};
