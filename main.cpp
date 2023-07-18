#include "MealyFSM.hpp"
#include <boost/program_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
namespace po = boost::program_options;
int main(int argc, char *argv[]){
    po::options_description desc("Options");
    std::string mode;
    std::string file;
    po::variables_map vm;
    desc.add_options()
        ("help,h", "Show help")
        ("mode,m", po::value<std::string>(&mode)->required()->default_value(""), "Select the mode : states, transitions, paths")
        ("file,f", po::value<std::string>(&file)->required()->default_value(""), "Provide program with a .json file to work")
    ;
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).run();
    po::store(parsed, vm);
    po::notify(vm);
    if(vm.count("help")){std::cerr<<desc<<std::endl;return 0;}
    MealyFSM machine;
    if(!machine.ReadFromJson(file)) return 1;
    if(mode == "states")
        machine.StatesInpSeqGen();
    else if(mode == "transitions")
        machine.TransitionsInpSeqGen();
    else if(mode == "paths")
        machine.PathsInpSeqGen();
    return 0;
}