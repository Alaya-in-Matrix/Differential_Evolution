#pragma once
#include <cstdio>
#include <iostream>
#include <fstream>
#include <utility>
#include <cassert>
#include <unordered_map>
#include <string>
#include "DE/DEStrategy.h"
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
    std::string  _prj_dir;
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
    std::unordered_map<std::string, double> _measured_vars_on_fail;

    // spec
    // usage: auto w = OptDirectionWeight[Minimize]...
    int    _penalty_weight;
    std::string _fom_name;
    double _fom_direction;
    std::unordered_map<std::string, double> _constraints;
    std::unordered_map<std::string, double> _constr_weight;

    // algorithm
    std::string       _de_type;
    double            _f;
    double            _cr;
    MutationStrategy  _ms;
    CrossoverStrategy _cs;
    SelectionStrategy _ss;
    std::unordered_map<std::string, double> _extra_conf;

    void set_para();
    void set_opt_settings() noexcept; //these settings have default value, so no exception would be thrown
    void set_sim_info();
    void set_measured_vars();
    void set_spec();
    void set_algo_para();
public:
    // I perhaps should use TOML format config file
    // as it is more human-readable
    enum SpecFormat { Json = 0, TOML };
    Config(std::string, SpecFormat = Json);
    Config(const ptree& );
    decltype(_para_names) get_para_names()  const noexcept { return _para_names;     }
    decltype(_ranges)     get_para_ranges() const noexcept { return _ranges;         }
    unsigned int iter_num()                 const noexcept { return _iter_num;       }
    unsigned int para_num()                 const noexcept { return _para_num;       }
    unsigned int population()               const noexcept { return _population;     }
    unsigned int thread_num()               const noexcept { return _thread_num;     }
    std::string  prj_dir()                  const noexcept { return _prj_dir;        }
    std::string  out_dir()                  const noexcept { return _out_dir;        }
    std::string  workspace()                const noexcept { return _workspace;      }
    std::string  para_file()                const noexcept { return _para_file;      }
    std::string  circuit_dir()              const noexcept { return _circuit_dir;    }
    std::string  testbench()                const noexcept { return _testbench;      }
    std::string  sim_tool()                 const noexcept { return _sim_tool;       }
    std::string  fom_name()                 const noexcept { return _fom_name;       }
    double penalty_weight()                 const noexcept { return _penalty_weight; }
    int    fom_direction_weight()           const noexcept { return _fom_direction;  }
    std::unordered_map<std::string, double> constraints()        const noexcept { return _constraints; }
    std::unordered_map<std::string, double> constraints_weight() const noexcept { return _constr_weight; }
    std::unordered_map<std::string, std::vector<std::string>> measured_vars() const noexcept { return _measured_vars; }
    std::string  de_type()                  const noexcept { return _de_type;        }
    double       de_f()                     const noexcept { return _f;              }
    double       de_cr()                    const noexcept { return _cr;             }
    MutationStrategy  mutation_strategy()   const noexcept { return _ms;             }
    CrossoverStrategy crossover_strategy()  const noexcept { return _cs;             }
    SelectionStrategy selection_strategy()  const noexcept { return _ss;             }
    std::unordered_map<std::string, double> extra_conf() const noexcept { return _extra_conf; }
    double process_measured(const std::string, const std::vector<double>&) const noexcept;
    double lookup_onfail(const std::string) const noexcept;
};
