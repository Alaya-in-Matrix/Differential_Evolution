#include "hspice_util.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <unordered_map>
#include <limits>
using namespace std;
void gen_param(const vector<string>& names, const vector <double>& values, const string path)
{
    assert(names.size() == values.size());
    ofstream ofile;
    ofile.open(path);
    if(! ofile.is_open())
    {
        cerr << "File " << path << " can not be created" << endl;
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < names.size(); ++i)
    {
        ofile << ".param " << names[i] << " = " << values[i] << endl;
    }
    ofile.close();
}

unordered_map<string, vector<double>> parse_hspice_measure_file(string path, int lines_to_ignore)
{
    unordered_map<string, vector<double>> result;
    ifstream ma0_file(path);
    if(! ma0_file.is_open())
    {
        cerr << "File " << path << " is not open" << endl;
        exit(EXIT_FAILURE);
    }
    for(int ignored = 0; ignored < lines_to_ignore; ++ignored) 
        ma0_file.ignore(numeric_limits<streamsize>::max(), '\n');
    vector<string> names;
    vector<double> values;
    string token;
    bool parse_name = true;
    while (ma0_file >> token)
    {
        if(parse_name)
        {
            names.push_back(token);
            if (token == "alter#") parse_name = false;
        }
        else
        {
            stringstream ss(token);
            double num;
            if(! (ss >> num)) num = numeric_limits<double>::quiet_NaN();
            values.push_back(num);
        }
    }
    size_t names_size  = names.size();
    size_t values_size = values.size();
    assert(values_size % names_size == 0);
    for(size_t i = 0; i < names_size; ++i)
    {
        result.insert(make_pair(names[i], vector<double>()));
        result[names[i]].reserve(values_size / names_size);
    }
    for(size_t i = 0; i < values_size; ++i)
        result[names[i % names_size]].push_back(values[i]);
    for(auto p : result)
        assert(p.second.size() == values_size / names_size);
    return result;
}
