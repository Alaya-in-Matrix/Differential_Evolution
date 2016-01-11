#include <cstdio>
#include <iostream>
#include <fstream>
#include <utility>
#include <cassert>
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
using namespace std;
using namespace boost::property_tree;

void get_para(const ptree& pt, vector<string>& names, vector<pair<double, double>>& ranges)
{
    // I really miss Maybe monad in haskell!
    ptree para_tree;
    names.clear();
    ranges.clear();
    try
    {
        para_tree = pt.get_child("parameter");
        names.reserve(para_tree.size());
        ranges.reserve(para_tree.size());
        for(auto para = para_tree.begin(); para !=  para_tree.end(); ++para)
        {
            string name = para->second.get<string>("name");
            double lb   = para->second.get<double>("lb");
            double ub   = para->second.get<double>("ub");
            names.push_back(name);
            ranges.push_back(make_pair(lb, ub));
            if(lb > ub) 
                throw new logic_error(name + ": lb < ub");
        }
    }
    catch(ptree_error& e)
    {
        cerr << "Fail to get parameters: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    catch(logic_error& e)
    {
        cerr << "Logic error in parameters: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    catch(exception& e)
    {
        cerr << "Unknown exception: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

unordered_map<string, string> get_sim_info(const ptree& pt)
{
    unordered_map<string, string> sim_info;
    try 
    {
        sim_info["para_file"]   = pt.get<string>("simulation.para_file");
        sim_info["circuit_dir"] = pt.get<string>("simulation.circuit_dir");
        sim_info["testbench"]   = pt.get<string>("simulation.testbench");
        sim_info["sim_tool"]    = pt.get<string>("simulation.sim_tool");
    }
    catch(ptree_error& e)
    {
        cerr << "Fail to get sim info: " << e.what() << endl;
    }
    return sim_info;
}

unordered_map<string, string> get_measure_file(const ptree& pt)
{
    unordered_map<string, string> measure_from;
    try
    {
        ptree meas_tree = pt.get_child("measured");
        measure_from.reserve(meas_tree.size());
        for(auto node = meas_tree.begin(); node != meas_tree.end(); ++node)
        {
            const string name = node->second.get<string>("name");
            const string from = node->second.get<string>("from");
            measure_from[name] = from;
        }
    }
    catch(ptree_error& e)
    {
        cerr << "fail to get measure info: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    return measure_from;
}
int main()
{
    ptree pt;
    ifstream config("./template.json");
    try {
        read_json(config, pt);
    }
    catch(ptree_error& e)
    {
        cerr << "Fail to parse json: " << e.what() << endl;
        return EXIT_FAILURE;
    }
    vector<string> para_names;
    vector<pair<double, double>> ranges;
    get_para(pt, para_names, ranges);
    assert(para_names.size() == ranges.size());

    const unsigned int iter_num = pt.get("iter_num", 600);
    const unsigned int para_num = para_names.size();
    const unsigned int init_num = pt.get("init_num", 10 * para_num);
    const string out_dir        = pt.get("out_dir", "out");
    const string workspace      = pt.get("workspace", "workspace");
    
    unordered_map<string, string> sim_info = get_sim_info(pt);
    for(auto sim_pair : sim_info)
    {
        printf("%s is %s\n", sim_pair.first.c_str(), sim_pair.second.c_str());
    }
    unordered_map<string, string> measure_file = get_measure_file(pt);

    for(auto meas_pair : measure_file)
    {
        printf("%s is %s\n", meas_pair.first.c_str(), meas_pair.second.c_str());
    }

    return EXIT_SUCCESS;
}
