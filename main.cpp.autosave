#include <iostream>
#include "irm.h"
#include "boost/program_options.hpp"
#include "date/include/date/date.h"

using namespace date;
using namespace std;
namespace po = boost::program_options;

std::string input;
std::string inputGrid;
std::string inputGridDef;
std::string pathOut;

int main(int argc, char *argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("in", po::value< std::string>()->required(), "file in")
            ("inputGrid", po::value< std::string>()->required(), "directory of netcdf template for IRM containing the grid with associated pixel_id")
            ("inputGridDef", po::value< std::string>()->required(), "txt with definition of grid")
            ("out", po::value< std::string>()->required(), "file out")
            ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    if (vm.count("in")) {input=vm["in"].as<std::string>();
        std::cout << " je vais utiliser le fichier de donnée IRM " << input << std::endl;
    }
    if (vm.count("inputGrid")) {inputGrid=vm["inputGrid"].as<std::string>();
        std::cout << " je vais utiliser le nc template" << inputGrid << std::endl;}
    if (vm.count("inputGridDef")) {inputGridDef=vm["inputGridDef"].as<std::string>();}
    if (vm.count("out")) {pathOut=vm["out"].as<std::string>();
        std::cout << " j'écrirai les résutats dans le fichier " << pathOut << std::endl;}
    std::cout << " create netcdf for IRM gridded observation" << std::endl;
    if (boost::filesystem::exists(input)){
        irmData d(input);
        d.saveNetCDF(pathOut);
    } else {
        std::cout << " je ne peux rien faire car " << input << " n'existe pas..." <<std::endl;
    }
    std::cout << "done" << std::endl;
    return 0;
}

