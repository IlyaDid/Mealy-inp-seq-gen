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
        for(const auto& state : machine.states)
            std::cout << state << " ";
        std::cout << std::endl;
        std::cout << "Initial state: " << machine.initial_state << std::endl;
        for(size_t i = 0; i < machine.transitions.size(); i++){
            for(size_t j = 0; j < machine.transitions[i].size(); j++){
                std::cout << "From: " << i;
                std::cout << " To: " << machine.transitions[i][j].to;
                std::cout << " Input: " << machine.transitions[i][j].input;
                std::cout << std::endl;
            }
        }
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