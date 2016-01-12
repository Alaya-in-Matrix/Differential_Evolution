#include <cstdio>
#include <iostream>
#include <fstream>
#include <utility>
#include <cassert>
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include "OptInfo.h"
int main()
{
    OptInfo opt_info("template.json");
    opt_info.print();
    return 0;
}
