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
    measured["cmrr"] = numeric_limits<double>::infinity() * -1;
    measured["psr"]  = numeric_limits<double>::infinity() * -1;
    measured["srr"]  = numeric_limits<double>::infinity();
    measured["srf"]  = numeric_limits<double>::infinity();
    measured["iq"]   = numeric_limits<double>::infinity() * -1;

    for (string corner : netlist_vec)
    {
        string netlist_path = corner + ".sp";
        string ma0_path     = folder + "/" + corner + ".ma0";
        string ms0_path     = folder + "/" + corner + ".ms0";
        string mt0_path     = folder + "/" + corner + ".mt0";
        string hspice_cmd = "cd " + folder + " && hspicerf64 " + netlist_path + " > output.info 2>&1";
        unordered_map<string, double> ma0_measured; // ac
        unordered_map<string, double> mt0_measured; // tran
        unordered_map<string, double> ms0_measured; // dc
        gen_param(names, params, para_path);
        int ret = system(hspice_cmd.c_str());
        if (ret == 0)
        {
            ma0_measured       = parse_hspice_measure_file(ma0_path);
            mt0_measured       = parse_hspice_measure_file(mt0_path);
            ms0_measured       = parse_hspice_measure_file(ms0_path);

            ma0_measured["pm"]   = ma0_measured["pm"] > 0 ? ma0_measured["pm"] - 180 : ma0_measured["pm"] + 180;
            ma0_measured["ugf"] *= 1e-6;
            mt0_measured["srr"] = 1e-6 * fabs(mt0_measured["srr"]);
            mt0_measured["srf"] = 1e-6 * fabs(mt0_measured["srf"]);
            ms0_measured["iq"]  *= 1e6;

            measured["failed"] = ma0_measured["failed"] || mt0_measured["failed"] || ms0_measured["failed"];
        }
        else
        {
            measured["failed"] = 1;
            cerr << "Fail to run hspice" << endl;
            for (auto p : params)
                cerr << p << endl;
        }
        if (measured["failed"]) 
        {
            measured["gain"]   = numeric_limits<double>::infinity() * -1;
            measured["pm"]     = numeric_limits<double>::infinity() * -1;
            measured["ugf"]    = numeric_limits<double>::infinity() * -1;
            measured["srr"]    = numeric_limits<double>::infinity() * -1;
            measured["srf"]    = numeric_limits<double>::infinity() * -1;
            measured["cmrr"]   = numeric_limits<double>::infinity();
            measured["psr"]    = numeric_limits<double>::infinity();
            measured["iq"]     = numeric_limits<double>::infinity();
            break;
        }
        if (ma0_measured["gain"]  < measured["gain"])  measured["gain"] = ma0_measured["gain"];
        if (ma0_measured["pm"]    < measured["pm"])    measured["pm"]   = ma0_measured["pm"];
        if (ma0_measured["ugf"]   < measured["ugf"])   measured["ugf"]  = ma0_measured["ugf"];
        if (ma0_measured["cmrr"]  > measured["cmrr"])  measured["cmrr"] = ma0_measured["cmrr"];
        if (ma0_measured["psr"]   > measured["psr"])   measured["psr"]  = ma0_measured["psr"];
        if (mt0_measured["srr"]   < measured["srr"])   measured["srr"]  = mt0_measured["srr"];
        if (mt0_measured["srf"]   < measured["srf"])   measured["srf"]  = mt0_measured["srf"];
        if (ms0_measured["iq"]    > measured["iq"])    measured["iq"]   = ms0_measured["iq"];
    }
    return measured;
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

    bool failed = measured["failed"];
    double gain = measured["gain"];
    double pm   = measured["pm"];
    double ugf  = measured["ugf"];
    double iq   = measured["iq"];
    double cmrr = measured["cmrr"];
    double psr  = measured["psr"];
    double srr  = measured["srr"];
    double srf  = measured["srf"];

    double penalty = numeric_limits<double>::infinity();
    double fom     = numeric_limits<double>::infinity();
    if (! failed)
    {
        const double pm_constr   = 55.5;
        const double iq_constr   = 56.0;
        const double gain_constr = 100;
        const double srr_constr  = 0.26;
        const double srf_constr  = 0.26;
        const double cmrr_constr = numeric_limits<double>::infinity(); // 这里不是共模抑制比，是共模增益, 越小越好
        const double psr_constr  = numeric_limits<double>::infinity();
        
        penalty = 0;
        penalty += iq   < iq_constr   ? 0 : iq - iq_constr;
        penalty += pm   > pm_constr   ? 0 : pm_constr - pm;
        penalty += gain > gain_constr ? 0 : gain_constr - gain;
        penalty += srr  > srr_constr  ? 0 : 50 * (srr_constr - srr);
        penalty += srf  > srf_constr  ? 0 : 50 * (srf_constr - srf);
        penalty += cmrr < cmrr_constr ? 0 : cmrr - cmrr_constr;
        penalty += psr  < psr_constr  ? 0 : psr - psr_constr;
        penalty *= 30;

        char buf[100];
        fflush(stdout);
        fom = -1 * (ugf - penalty);
        if (ugf > 1.17 && penalty < 0.1)
        {
            sprintf(buf, "out/good_%d_%g", idx, fom);
            string stat_name(buf);
            gen_param(names, params, stat_name);
        }
    }
    #pragma omp critical
    {
        printf("idx: %d, gain = %g dB, pm = %g degree, ugf = %g MHz, iq = %g uA, srr = %g V/us, srf = %g V/us, penalty = %g, fom = %g\n", idx, gain, pm, ugf, iq, srr, srf, penalty, fom);
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
