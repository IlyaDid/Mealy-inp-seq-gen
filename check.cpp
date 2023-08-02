#include "MealyFSM.hpp"
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
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
        if(buf.size() != 0 && buf[0] != ' '){
            state = machine.initial_state;
            for(const auto& ch : buf){
                if(ch != ' ') symb += ch;
                else{
                    found = false;
                    for(const auto& transition : machine.transitions[state].second){
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
    }
    if(s.empty()) std::cout<<"OK"<<std::endl;
    else{
        std::cout<<"The following states weren`t covered: ";
        for(const auto& st : s)
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
        for(size_t j = 0; j < machine.transitions[i].second.size(); j++)
            t.insert(std::make_pair(i,j));
    }
    while(!in.eof()){
        getline(in, buf);
        state = machine.initial_state;
        if(buf.size() != 0 && buf[0] != ' '){
            for(const auto& ch : buf){
                if(ch != ' ') symb += ch;
                else{
                    found = false;
                    for(size_t i = 0; i < machine.transitions[state].second.size(); i++){
                        if(machine.transitions[state].second[i].input == symb){
                            t.erase(std::make_pair(state, i));
                            symb.clear();
                            state = machine.transitions[state].second[i].to;
                            found = true;
                            break;
                        }
                    }
                    if(!found || !machine.transitions[state].second.size()){std::cerr<<"Invalid input sequence"<<std::endl;return;}
                }
            }
        }
    }
    if(t.empty()) std::cout<<"OK"<<std::endl;
    else{
        std::cout<<"The following transitions weren`t covered:"<<std::endl;
        for(const auto& transition : t)
            std::cout<<"From: "<<machine.states[transition.first]<<" Input: "<<machine.transitions[transition.first].second[transition.second].input<<" To: "<<machine.states[machine.transitions[transition.first].second[transition.second].to]<<std::endl;
    }
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
        ("help,h", "Show help")
        ("mode,m", po::value<std::string>(&mode)->required(), "Select the mode : states, transitions")
        ("file,f", po::value<std::string>(&file)->required(), "JSON file with description of Mealy finite state machine")
        ("input,i", po::value<std::string>(&input)->required(), "Input sequences file")
    ;
    pos_desc.add("file", 1);
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run();
    po::store(parsed, vm);
    if(vm.count("help")){std::cerr<<desc<<std::endl;return 0;}
    po::notify(vm);
    try{
        MealyFSM machine(file);
        if(mode == "states")
            StatesCheck(machine,input);
        else if(mode == "transitions")
            TransitionsCheck(machine,input);
        else
            std::cerr << "Invalid mode" << std::endl;
    }
    catch(std::runtime_error& e){
        std::cerr<<e.what()<<std::endl;
        return 1;
    }
    return 0;
}