#include "MealyFSM.hpp"
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
bool StatesCheck(const MealyFSM& machine, const std::string& input){
    bool found;
    size_t count = 0;
    size_t s_count = 0;
    size_t state;
    std::set<size_t> s;
    std::string buf;
    std::ifstream in(input, std::ios_base::in);
    std::vector<std::string> line;
    for(size_t i = 0; i < machine.states.size(); i++)
        if(i != machine.initial_state) s.insert(i);
    while(!in.eof()){
        getline(in, buf);
        count++;
        s_count = 0;
        boost::split(line, buf, boost::is_any_of(" "));
        state = machine.initial_state;
        for(const auto& symb : line){
            if(symb == "") break;
            s_count++;
            found = false;
            for(const auto& transition : machine.transitions[state]){
                if(transition.input == symb){
                    state = transition.to;
                    s.erase(state);
                    found = true;
                    break;
                }
            }
            if(!found){std::cerr<<"Invalid input sequence on line "<< count << ", symbol " << s_count <<std::endl;return false;}
        }
    }
    if(s.empty()) std::cout<<"OK"<<std::endl;
    else{
        std::cout<<"The following states weren`t covered: ";
        for(const auto& st : s)
            std::cout<<machine.states[st]<<" ";
        std::cout<<std::endl; 
        return false;
    }
    return true;
}
bool TransitionsCheck(const MealyFSM& machine, const std::string& input){
    bool found;
    size_t state;
    size_t count = 0;
    size_t s_count = 0;
    std::set<std::pair<size_t, size_t>> t;
    std::string buf;
    std::string symb;
    std::ifstream in(input, std::ios_base::in);
    std::vector<std::string> line;
    for(size_t i = 0; i < machine.transitions.size(); i++){
        for(size_t j = 0; j < machine.transitions[i].size(); j++)
            t.insert(std::make_pair(i,j));
    }
    while(!in.eof()){
        getline(in, buf);
        count++;
        s_count = 0;
        boost::split(line, buf, boost::is_any_of(" "));
        state = machine.initial_state;
        for(const auto& symb : line){
            if(symb == "") break;
            s_count++;
            found = false;
            for(size_t i = 0; i < machine.transitions[state].size(); i++){
                if(machine.transitions[state][i].input == symb){
                    t.erase(std::make_pair(state, i));
                    state = machine.transitions[state][i].to;
                    found = true;
                    break;
                }
            }
            if(!found){std::cerr<<"Invalid input sequence on line "<< count << ", symbol " << s_count <<std::endl;return false;}
        }
    }
    if(t.empty()) std::cout<<"OK"<<std::endl;
    else{
        std::cout<<"The following transitions weren`t covered:"<<std::endl;
        for(const auto& transition : t)
            std::cout<<"From: "<<machine.states[transition.first]<<" Input: "<<machine.transitions[transition.first][transition.second].input<<" To: "<<machine.states[machine.transitions[transition.first][transition.second].to]<<std::endl;
        return false;
    }
    return true;
}
bool PathsCheck(const MealyFSM& machine, std::string input){
    std::vector<std::vector<std::string>> lines;
    std::ifstream in(input, std::ios_base::in);
    std::string buf;
    std::set<std::string> not_visited_s;
    std::set<std::pair<size_t,size_t>> not_visited;
    std::pair<size_t, size_t> tr;
    size_t state;
    size_t count = 0;
    size_t s_count = 0;
    bool found;
    while(!in.eof()){
        getline(in, buf);
        lines.push_back(std::vector<std::string>());
        boost::split(lines.back(), buf, boost::is_any_of(" "));
    }
    for(const auto& line : lines){
        count++;
        s_count = 0;
        state = machine.initial_state;
        for(size_t i = 0; i < machine.transitions.size(); i++){
            for(size_t j = 0; j < machine.transitions[i].size(); j++)
                not_visited.insert(std::make_pair(i,j));
        }
        for(size_t i = 0; i < line.size(); i++){
            s_count++;
            found = false;
            for(size_t j = 0; j < machine.transitions[state].size(); j++){
                if(machine.transitions[state][j].input == line[i]){
                    if(not_visited.find(std::make_pair(state,j)) == not_visited.end()){
                        std::cerr<<"Repeated same transition "<< machine.states[state] << "->" <<machine.transitions[state][j].input << "->" <<machine.states[machine.transitions[state][j].to] <<" in path "<< count <<std::endl;return false;}
                    found = true;
                    tr = std::make_pair(state, j);
                    not_visited.erase(std::make_pair(state,j));
                }
                else if(not_visited.find(std::make_pair(state,j)) != not_visited.end())
                    not_visited_s.insert(machine.transitions[state][j].input);
            }
            if(!found && line[i] != ""){std::cerr<<"Invalid input sequence on line "<< count << ", symbol " << s_count <<std::endl;return false;}
            for(size_t j = 0; j < lines.size(); j++){
                if(lines[j].size() > i){
                    found = true;
                    for(size_t k = 0; k < i; k++){
                        if(lines[j][k] != line[k]){
                            found = false;
                            break;
                        }
                    }
                    if(found){
                        for(size_t k = 0; k < machine.transitions[state].size(); k++){
                            if(machine.transitions[state][k].input == lines[j][i] && not_visited.find(std::make_pair(state,k)) != not_visited.end())
                                not_visited_s.erase(machine.transitions[state][k].input);
                        }
                    }
                }
            }
            if(!not_visited_s.empty()){
                std::cerr<<"Not all paths were covered. Example: "<<std::endl;
                for(const auto& symb : not_visited_s){
                    for(const auto& s : lines[i])
                        if(s!="") std::cerr << s << " ";
                    std::cerr << symb << std::endl;
                }
                return false;
            }
            not_visited_s.clear();
            if(machine.transitions[state].size() > 0) state = machine.transitions[tr.first][tr.second].to;
        }
        not_visited.clear();
    }
    std::cout << "OK" << std::endl;
    return true;
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
        ("mode,m", po::value<std::string>(&mode)->required(), "Select the mode : states, transitions, paths")
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
        if(mode == "states"){
            if(!StatesCheck(machine,input)) return 2;
        }
        else if(mode == "transitions"){
            if(!TransitionsCheck(machine,input)) return 3;
        }
        else if(mode == "paths"){
            if(!PathsCheck(machine,input)) return 4;
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