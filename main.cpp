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
        ("mode,m", po::value<std::string>(&mode)->required(), "Select the mode : states, transitions, paths")
        ("file,f", po::value<std::string>(&file)->required(), "Provide program with a .json file to work")
    ;
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).run();
    po::store(parsed, vm);
    po::notify(vm);
    if(vm.count("help")){std::cerr<<desc<<std::endl;return 0;}
    try{
        MealyFSM machine(file);
        if(mode == "states")
            machine.StatesInpSeqGen();
        else if(mode == "transitions")
            machine.TransitionsInpSeqGen();
        else if(mode == "paths")
            machine.PathsInpSeqGen();
        else
            std::cerr << "Invalid mode" << std::endl;
    }
    catch(std::runtime_error& e){
        std::cerr<<e.what()<<std::endl;
        return 1;
    }
    return 0;
}