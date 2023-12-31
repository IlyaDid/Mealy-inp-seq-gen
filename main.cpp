#include "MealyFSM.hpp"
#include <iostream>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
int main(int argc, char *argv[]){
    po::options_description desc("Options");
    po::positional_options_description pos_desc;
    std::string mode;
    std::string file;
    po::variables_map vm;
    desc.add_options()
        ("help,h", "Show help")
        ("mode,m", po::value<std::string>(&mode)->required(), "Select the mode : states, transitions, paths")
        ("file,f", po::value<std::string>(&file)->required(), "JSON file with description of Mealy finite state machine")
    ;
    pos_desc.add("file", 1);
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run();
    po::store(parsed, vm);
    if(vm.count("help")){std::cerr<<desc<<std::endl;return 0;}
    po::notify(vm);
    try{
        MealyFSM machine(file);
        if(mode == "states"){
            if(!machine.StatesInpSeqGen()) return 2;
        }
        else if(mode == "transitions"){
            if(!machine.TransitionsInpSeqGen()) return 3;
        }
        else if(mode == "paths"){
            if(!machine.PathsInpSeqGen()) return 4;
        }
        else
            std::cerr << "Invalid mode" << std::endl;
    }
    catch(std::runtime_error& e){
        std::cerr<<e.what()<<std::endl;
        return 1;
    }
    return 0;
}