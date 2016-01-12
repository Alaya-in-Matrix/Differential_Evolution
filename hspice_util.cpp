#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include <unordered_map>
#include "hspice_util.h"
using namespace std;
void gen_param(const vector<string>& names, const vector <double>& values, const string path)
{
    assert(names.size() == values.size());
    ofstream ofile;
    ofile.open(path);
    // assert(ofile.is_open());
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

unordered_map<string, double> parse_hspice_measure_file(string path)
{
    unordered_map<string, double> result;
    ifstream ma0_file(path);
    string ignore_lines;
    while (getline(ma0_file, ignore_lines))
    {
        // to upper ignore lines
        const string title_line = ".TITLE";
        for (size_t i = 0; i < title_line.size(); ++i)
            ignore_lines[i] = toupper(ignore_lines[i]);

        // check whether is this line is a title line
        if (ignore_lines.size() >= title_line.size() && ignore_lines.substr(0, title_line.size()) == title_line)
            break;
    }
    vector<string> names;
    vector<double> values;
    string token;
    bool failed = false;
    while (ma0_file >> token)
    {
        names.push_back(token);
        if (token == "alter#") break;
    }
    while (ma0_file >> token)
    {
        if (token == "failed")
        {
            failed = true;
            break;
        }
        values.push_back(atof(token.c_str()));
    }
    if (failed)
        result["failed"] = 1;
    else
    {
        result["failed"] = 0;
        for (size_t i = 0; i < names.size(); ++i)
            result.insert(make_pair(names[i], values[i]));
    }
    return result;
}
