#include <cstdio>
#include <iostream>
#include <fstream>
#include <utility>
#include <cassert>
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>
#include "Config.h"
#include "Optimizer.h"
using namespace std;
int main(int arg_num, char** args)
{
    if(arg_num < 2)
    {
        cerr << "Usage: " << args[0] << " config-json-file-name" << endl;
        return EXIT_FAILURE;
    }
    Config config(args[1]);
    config.print();
    
    Optimizer opt(config);
    opt.init();
    opt.run();
    return EXIT_SUCCESS;
}
