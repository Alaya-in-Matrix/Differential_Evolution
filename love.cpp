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
#include "Optimizer.h"
int main()
{
    OptInfo config("template.json");
    config.print();
    
    Optimizer opt(config);
    opt.init();
    opt.run();
    return 0;
}
