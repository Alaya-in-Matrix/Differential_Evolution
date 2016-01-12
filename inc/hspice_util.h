#ifndef __HSPICE_UTIL_H_
#define __HSPICE_UTIL_H_
#include <string>
#include <vector>
#include <unordered_map>

void gen_param(const std::vector<std::string>& names, const std::vector <double>& values, const std::string path);
std::unordered_map<std::string, std::vector<double>> parse_hspice_measure_file(std::string path);
#endif
