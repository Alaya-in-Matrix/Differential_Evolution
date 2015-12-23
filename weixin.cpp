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
    "cm"
    , "ival"
    , "l_fixed"
    , "w10"
    , "w11"
    , "w12"
    , "w13"
    , "w14"
    , "w15"
    , "w16_18"
    , "w17_19"
    , "w1_2"
    , "w20"
    , "w21"
    , "w22_23"
    , "w24"
    , "w25"
    , "w26"
    , "w27"
    , "w28"
    , "w29"
    , "w30"
    , "w31"
    , "w32"
    , "w33"
    , "w34"
    , "w35"
    , "w3_4"
    , "w5"
    , "w6"
    , "w7"
    , "w8"
    , "w9"
    , "vin_cm"
    , "vin_cm_rr"
};
unordered_map<string, double> run_spice(string folder, vector<double>& params)
{
    string para_path    = folder + "/param.sp";
    string netlist_path = folder + "/Single_ended.sp";
    string ma0_path     = folder + "/Single_ended.ma0";
    string output_path  = folder + "/output.info";
    string hspice_cmd   = "hspice64 " + netlist_path + " -o " + folder + "> " + output_path + " 2>&1";
    gen_param(names, params, para_path);
    int ret = system(hspice_cmd.c_str());
    unordered_map<string, double> measured;
    if (ret == 0)
    {
        measured = parse_hspice_measure_file(ma0_path);
    }
    else
    {
        cerr << "Fail to run hspice" << endl;
        for (auto p : params)
            cerr << p << endl;
        measured["gain"] = -1 * numeric_limits<double>::infinity();
        measured["pm"]   = -180;
        measured["ugf"]  = 0;
    }
    return measured;
}
double opt_func(unsigned int idx, const vector<double>& params) // params without vin_cm
{
    vector<double> sweep_vin_cm{0, 1.1, 3.3};
    vector<double> gain_vec;
    vector<double> penalty_vec;
    string netlist_path;
    #pragma omp critical

    netlist_path = "workspace/" + to_string(idx);
    for (auto vin_cm : sweep_vin_cm)
    {
        vector<double> new_params = params;
        new_params.push_back(vin_cm);
        auto measured = run_spice(netlist_path, new_params);
        if (measured["failed"] == 0)
        {
            double this_gain    = measured["gain"];
            double this_gain_rr = measured["gain_rr"];
            double this_ugf     = measured["ugf"] / 1e6; // MHz
            double this_pm      = measured["pm"] > 0 ? measured["pm"] - 180 : measured["pm"] + 180;
            if (this_gain_rr < this_gain)
            {
                this_gain = this_gain_rr;
            }

            double this_penalty_ugf = this_ugf > 200 ? 0 : 200 - this_ugf;
            double this_penalty_pm  = this_pm  > 60  ? 0 : 60  - this_pm;
            double this_penalty     = this_penalty_ugf + this_penalty_pm;

            gain_vec.push_back(this_gain);
            penalty_vec.push_back(this_penalty);
        }
        else
        {
            gain_vec.push_back(-1 * numeric_limits<double>::infinity());
            penalty_vec.push_back(numeric_limits<double>::infinity());
            break;
        }
    }
    double gain    = *(min_element(gain_vec.begin(), gain_vec.end()));
    double penalty = *(min_element(penalty_vec.begin(), penalty_vec.end()));
    double fom     = gain - 3 * penalty;
    char buf[100];
    printf("idx: %d, gain = %g dB, penalty = %g, fom = %g\n", idx, gain, penalty, fom);
    fflush(stdout);
    if (fom > 70 && penalty < 1)
    {
        sprintf(buf, "out/good_%d_%g", idx, gain);
        string stat_name(buf);
        vector<double> new_params = params;
        new_params.push_back(numeric_limits<double>::infinity());
        gen_param(names, new_params, stat_name);
    }
    return -1 * fom;
}
int main(int arg_num, char** args)
{
    vector<pair<double, double>> ranges
    {
        make_pair(0.5, 10)
        , make_pair(0  , 3)
        , make_pair(0.35, 10)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0.4, 20)
        , make_pair(0, 3.3)
    }; //without vin_cm_rtr
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
        string cmd = "mkdir -p " + path + " && cp circuit/weixin/* " + path;
        system(cmd.c_str());
    }
    if (mkdir_ret != 0)
    {
        cerr << "Can't mkdir" << endl;
        exit(0);
    }
    DESolver desolver(opt_func, ranges, iter_num, para_num, init_num);
    vector<double> solution = desolver.solver();
    // printf("Result is %g dB\n", -1 * opt_func(solution));
    return 0;
}
