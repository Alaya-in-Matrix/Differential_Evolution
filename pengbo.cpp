#include <cstdlib>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <cstdio>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <algorithm>
#include <limits>
#include <unordered_map>
#include "Evolution.h"
#include "hspice_util.h"

using namespace std;
vector<string> names
{
    "ib"
    , "wa2"
    , "wb1"
    , "wb2"
    , "w1"
    , "w3"
    , "w5"
    , "w7"
    , "w9"
    , "w11"
    , "w12"
    , "w16"
    , "w13"
    , "w14"
    , "w15"
    , "cmm"
    , "r1"
    , "cz"
    , "rz"
    , "l5"
    , "l7"
    , "l9"
    , "ln"
    , "lp"
};
unordered_map<string, double> run_spice(string folder, const vector<double>& params)
{
    vector<string> netlist_vec{"Single_ended", "Single_ended_FF", "Single_ended_SS"};
    string para_path   = folder + "/param.sp";
    string output_path = folder + "/output.info";
    unordered_map<string, double> measured;
    measured["fail"] = false;
    measured["gain"] = numeric_limits<double>::infinity();
    measured["pm"]   = numeric_limits<double>::infinity();
    measured["ugf"]  = numeric_limits<double>::infinity();

    for (string corner : netlist_vec)
    {
        string netlist_path = corner + ".sp";
        string ma0_path     = folder + "/" + corner + ".ma0";
        string hspice_cmd = "cd " + folder + " && hspicerf64 " + netlist_path + " > output.info 2>&1";
        unordered_map<string, double> tmp_measured;
        gen_param(names, params, para_path);
        int ret = system(hspice_cmd.c_str());
        if (ret == 0)
        {
            tmp_measured       = parse_hspice_measure_file(ma0_path);
            tmp_measured["pm"] = tmp_measured["pm"] > 0 ? tmp_measured["pm"] - 180 : tmp_measured["pm"] + 180;
        }
        else
        {
            cerr << "Fail to run hspice" << endl;
            for (auto p : params)
                cerr << p << endl;
            tmp_measured["failed"] = true;
            tmp_measured["gain"] = -1 * numeric_limits<double>::infinity();
            tmp_measured["pm"]   = -1 * numeric_limits<double>::infinity();
            tmp_measured["ugf"]  = -1 * numeric_limits<double>::infinity();
        }
        measured["failed"] = tmp_measured["failed"];
        if (measured["failed"]) break;
        if (tmp_measured["gain"] < measured["gain"]) measured["gain"] = tmp_measured["gain"];
        if (tmp_measured["pm"]   < measured["pm"])   measured["pm"]   = tmp_measured["pm"];
        if (tmp_measured["ugf"]  < measured["ugf"])  measured["ugf"]  = tmp_measured["ugf"];
    }
    return measured;
}
double read_iq(string netlist_path)
{
    vector<string> netlist_vec{"Single_ended.printsw0", "Single_ended_FF.printsw0", "Single_ended_SS.printsw0"};
    double iq = 0;
    for (auto filename : netlist_vec)
    {
        double tmp_iq;
        string path = netlist_path + "/" + filename;
        ifstream ifile(path);
        string line;
        getline(ifile, line); // ignore first line
        getline(ifile, line); // ignore second line
        ifile >> tmp_iq >> tmp_iq;
        if (fabs(tmp_iq) > iq)
            iq = fabs(tmp_iq);
    }
    return iq;
}
double opt_func(unsigned int idx, const vector<double>& params) // params without vin_cm
{
    // Iq < 68.5uA
    // Gain > 104.7 dB
    // UGF  > 1.17MHz
    // PM   > 62.5 degree
    string netlist_path;
    netlist_path = "workspace/" + to_string(idx);
    unordered_map<string, double> measured = run_spice(netlist_path, params);

    bool failed    = measured["failed"];
    double gain    = measured["gain"];
    double pm      = measured["pm"];
    double ugf     = measured["ugf"] / 1e6;
    double iq      = 1e6 * read_iq(netlist_path);
    double penalty = numeric_limits<double>::infinity();
    double fom     = numeric_limits<double>::infinity();
    if (! failed)
    {
        const double pm_constr   = 55.5;
        const double ugf_constr  = 1.17;
        const double iq_constr   = 60.7;
        const double penalty_pm  = pm  > pm_constr  ? 0 : pm_constr - pm;
        const double penalty_ugf = ugf > ugf_constr ? 0 : 50 * (ugf_constr  - ugf);
        const double penalty_iq  = iq  < iq_constr  ? 0 : iq - iq_constr;

        penalty = 30 * (penalty_pm + penalty_ugf + penalty_iq);
        char buf[100];
        fflush(stdout);
        if (gain > 100 && penalty < 0.1)
        {
            sprintf(buf, "out/good_%d_%g", idx, gain);
            string stat_name(buf);
            gen_param(names, params, stat_name);
        }
        fom = -1 * (gain - penalty);
    }
    #pragma omp critical
    {
        printf("idx: %d, gain = %g dB, pm = %g degree, ugf = %g MHz, iq = %g uA, penalty = %g, fom = %g\n", idx, gain, pm, ugf, iq, penalty, fom);
    }
    return fom;
}
int main(int arg_num, char** args)
{
    vector<pair<double, double>> ranges
    {
        make_pair(1.70e+0, 2.20e+0)
        , make_pair(5.00e+0, 8.00e+0)
        , make_pair(5.00e+0, 8.00e+0)
        , make_pair(9.00e+0, 1.50e+1)
        , make_pair(1.10e+2, 1.60e+2)
        , make_pair(9.00e+0, 1.30e+1)
        , make_pair(9.00e+0, 1.30e+1)
        , make_pair(1.00e+1, 1.50e+1)
        , make_pair(9.00e+0, 1.40e+1)
        , make_pair(2.00e+1, 3.50e+1)
        , make_pair(1.70e+1, 2.70e+1)
        , make_pair(1.10e+2, 1.60e+2)
        , make_pair(1.40e+1, 2.20e+1)
        , make_pair(1.20e+1, 1.60e+1)
        , make_pair(2.70e+1, 4.20e+1)
        , make_pair(1.00e+0, 2.00e+0)
        , make_pair(1.40e+2, 2.20e+2)
        , make_pair(5.00e-1, 1.00e+0)
        , make_pair(2.60e+2, 3.20e+2)
        , make_pair(3.70e-1, 4.70e-1)
        , make_pair(3.70e-1, 4.70e-1)
        , make_pair(1.00e+0, 1.20e+0)
        , make_pair(6.00e-1, 8.00e-1)
        , make_pair(1.00e+0, 1.30e+0)
    };
    int thread_num = 2;
    if (arg_num > 1)
    {
        thread_num = atoi(args[1]);
    }
    omp_set_num_threads(thread_num);

    const unsigned int iter_num = 600;
    const unsigned int para_num = ranges.size();
    const unsigned int init_num = 11 * para_num - 1;
    int mkdir_ret = system ("mkdir -p out/ && mkdir -p workspace");
    #pragma omp parallel for schedule(static)
    for (unsigned int i = 0; i < init_num; ++i)
    {
        string path = "workspace/" + to_string(i);
        string cmd = "mkdir -p " + path + " && cp circuit/pengbo/* " + path;
        system(cmd.c_str());
    }
    if (mkdir_ret != 0)
    {
        cerr << "Can't mkdir" << endl;
        exit(0);
    }
    // vector<vector<double>> vec = vector<vector<double>>(3, vector<double>(4, 0));
    DESolver desolver(opt_func, ranges, iter_num, para_num, init_num);
    vector<double> solution = desolver.solver();
    // printf("Result is %g dB\n", -1 * opt_func(solution));
    return 0;
}
