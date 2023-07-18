#include "MealyFSM.hpp"
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
void StatesCheck(const MealyFSM& machine, const std::string& input){
    bool found;
    size_t state;
    std::set<size_t> s;
    std::string buf;
    std::string symb;
    state = machine.initial_state;
    std::ifstream in(input, std::ios_base::in);
    for(size_t i = 0; i < machine.states.size(); i++)
        if(i != machine.initial_state) s.insert(i);
    while(!in.eof()){
        getline(in, buf);
        for(auto ch : buf){
            if(ch != ' ') symb += ch;
            else{
                found = false;
                for(auto transition : machine.transitions[state].second){
                    if(transition.input == symb){
                        state = transition.to;
                        s.erase(state);
                        symb.clear();
                        found = true;
                        break;
                    }
                }
                if(!found){std::cerr<<"Invalid input sequence"<<std::endl;return;}
            }
        }
    }
    if(s.empty()) std::cout<<"OK"<<std::endl;
    else{
        std::cout<<"The following states weren`t covered: ";
        for(auto st : s)
            std::cout<<machine.states[st]<<" ";
        std::cout<<std::endl; 
    }
}
void TransitionsCheck(const MealyFSM& machine, const std::string& input){
    bool found;
    size_t state;
    std::set<std::pair<size_t, size_t>> t;
    std::string buf;
    std::string symb;
    state = machine.initial_state;
    std::ifstream in(input, std::ios_base::in);
    for(size_t i = 0; i < machine.transitions.size(); i++){
        for(auto transition : machine.transitions[i].second)
            t.insert(std::make_pair(i,transition.to));
    }
    while(!in.eof()){
        getline(in, buf);
        for(auto ch : buf){
            if(ch != ' ') symb += ch;
            else{
                found = false;
                for(auto transition : machine.transitions[state].second){
                    if(transition.input == symb){
                        t.erase(std::make_pair(state, transition.to));
                        symb.clear();
                        state = transition.to;
                        found = true;
                        break;
                    }
                }
                if(!found){std::cerr<<"Invalid input sequence"<<std::endl;return;}
            }
        }
    }
    if(t.empty()) std::cout<<"OK"<<std::endl;
    else{
        std::cout<<"The following transitions weren`t covered:"<<std::endl;
        for(auto transition : t)
            std::cout<<"From: "<<machine.states[transition.first]<<" Input: "<<machine.transitions[transition.first].second[transition.second].input<<" To: "<<machine.states[machine.transitions[transition.first].second[transition.second].to]<<std::endl;
    }
}
namespace po = boost::program_options;
int main(int argc, char *argv[]){
    po::options_description desc("Options");
    std::string mode;
    std::string file;
    std::string input;
    po::variables_map vm;
    desc.add_options()
        ("help,h", "Show help")
        ("mode,m", po::value<std::string>(&mode)->required()->default_value(""), "Select the mode : states, transitions")
        ("file,f", po::value<std::string>(&file)->required()->default_value(""), "Provide program with a .json file to work")
        ("input,i", po::value<std::string>(&input)->required()->default_value(""), "Provide program with input sequences file")
    ;
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).run();
    po::store(parsed, vm);
    po::notify(vm);
    if(vm.count("help")){std::cerr<<desc<<std::endl;return 0;}
    MealyFSM machine;
    if(!machine.ReadFromJson(file)) return 1;
    if(mode == "states")
        StatesCheck(machine,input);
    else if(mode == "transitions")
        TransitionsCheck(machine,input);
    return 0;
}