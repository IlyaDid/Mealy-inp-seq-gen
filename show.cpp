#include "MealyFSM.hpp"
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
void show(MealyFSM machine){
    std::ofstream out("machine.dot", std::ios_base::out);
    out << "digraph {\n";
    for(size_t i = 0; i < machine.transitions.size(); i++){
        for(size_t j = 0; j < machine.transitions[i].size(); j++)
            out << machine.states[i] << "->" << machine.states[machine.transitions[i][j].to] << "[label=\"" << machine.transitions[i][j].input << "/" << machine.transitions[i][j].output << "\"]\n";
    }
    out << "}";
    out.close();
    system("dot machine.dot -Tpng -o machine.png");
    system("fim machine.png");
}
namespace po = boost::program_options;
int main(int argc, char *argv[]){
    po::options_description desc("Options");
    po::positional_options_description pos_desc;
    std::string mode;
    std::string file;
    std::string input;
    po::variables_map vm;
    desc.add_options()
        ("help,h", "Show help. You need fim to automatically open an image. To install fim run \"sudo apt install fim\" in terminal.")
        ("file,f", po::value<std::string>(&file)->required(), "JSON file with description of Mealy finite state machine.")
    ;
    pos_desc.add("file", 1);
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run();
    po::store(parsed, vm);
    if(vm.count("help")){std::cerr<<desc<<std::endl;return 0;}
    po::notify(vm);
    try{
        MealyFSM machine(file);
        show(machine);
    }
    catch(std::runtime_error& e){
        std::cerr<<e.what()<<std::endl;
        return 1;
    }
    return 0;
}