#include "MealyFSM.hpp"
#include <iostream>
#include <stack>
#include <queue>
#include <set>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
namespace pt = boost::property_tree;
void MealyFSM::traverse(Node *n) const{
    bool found;
    Node *ptr;
    for(const auto& transition : transitions[n->state]){
        ptr = n;
        found = false;
        //Checking branch possible transitions
        while(ptr->par != NULL){
            if(ptr->par->state == n->state && ptr->state == transition.to && ptr->input == transition.input){
                found = true;
                break;
            }
            ptr = ptr->par;
        }
        //If found adding new transition to the end of the branch
        if(found == false){
            Node child;
            child.state = transition.to;
            child.input = transition.input;
            child.par = n;
            n->children.push_back(child);
        }
    }
    //Initializing traverse for each child branch
    for(auto& child : n->children)
        traverse(&child);
}
void MealyFSM::print(Node *n) const{
    Node *ptr = n;
    std::stack<std::string> s;
    if(!n->children.size()){
        //Adding all transitions in branch to stack
        while(ptr->par != NULL){
            s.push(ptr->input);
            ptr = ptr->par;
        }
        //Printing input symbols
        while(!s.empty()){
            std::cout << s.top() << " ";
            s.pop();
        }
        std::cout << std::endl;
    }
    //Initializing print for each child branch
    for(size_t i = 0; i < n->children.size(); i++)
        print(&n->children[i]);
}
std::vector<std::pair<size_t, std::string>> MealyFSM::BFSStates(const size_t& u) const{
    size_t v;
    std::queue<size_t> q;
    std::vector<bool> color;
    //Vector of preceding states and input symbols to reach any state
    std::vector<std::pair<size_t, std::string>> pred;
    for(size_t i = 0; i < states.size(); i++){
        color.push_back(0);
        pred.push_back(std::make_pair(0, std::string()));
    }
    color[u] = 1;
    pred[u] = std::make_pair(0, std::string());
    q.push(u);
    while(!q.empty()){
        v = q.front();
        q.pop();
        for(const auto& transition : transitions[v]){
            if(!color[transition.to]){
                pred[transition.to] = std::make_pair(v, transition.input);
                color[transition.to] = 1;
                q.push(transition.to);
            }
        }
    }
    return pred;
}
std::vector<std::vector<std::pair<size_t, size_t>>> MealyFSM::BFSTransitions(const size_t& u, std::vector<std::vector<std::string>>& input) const{
    size_t to;
    std::pair<size_t, size_t> v;
    std::queue<std::pair<size_t, size_t>> q;
    std::vector<std::vector<bool>> color(states.size());
    //Vector of vectors where indexes mean first - index of state, second - index of transition from this state
    //Indexes in pair indicate preceding transition in this vector of vectors
    std::vector<std::vector<std::pair<size_t, size_t>>> pred(states.size());
    for(size_t i = 0; i < transitions.size(); i++){
        for(size_t j = 0; j < transitions[i].size(); j++){
            color[i].push_back(0);
            pred[i].push_back(std::make_pair(0,0));
            input[i].push_back("");
        }
    }
    for(size_t i = 0; i < transitions[u].size(); i++){
        color[u][i] = 1;
        pred[u][i] = std::make_pair(u, i);
        input[u][i] = transitions[u][i].input;
        q.push(std::make_pair(u, i));
    }
    while(!q.empty()){
        v = q.front();
        q.pop();
        to = transitions[v.first][v.second].to;
        if(to < transitions.size()){
            for(size_t i = 0; i < transitions[to].size(); i++){
                if(!color[to][i]){
                    input[to][i] = transitions[to][i].input;
                    color[to][i] = 1;
                    pred[to][i] = std::make_pair(v.first, v.second);
                    q.push(std::make_pair(to, i));
                }
            }
        }
    }
    return pred;
}
std::vector<size_t> MealyFSM::greedy_set_cover(const std::vector<std::vector<bool>>& matrix) const{
    std::set<size_t> u;
    std::vector<size_t> res;
    std::vector<size_t> max(3);
    size_t cur;
    size_t size;
    for(size_t i = 0; i < matrix.size(); i++)
        u.insert(i);
    while(!u.empty()){
        max[0] = 0;
        for(size_t i = 0; i < matrix.size(); i++){
            cur = 0;
            size = 0;
            for(size_t j = 0; j < matrix[i].size(); j++){
                if(matrix[i][j]){
                    size++;
                    if(u.find(j) != u.end())
                        cur++;
                }
            }
            if(cur > max[0]){
                max[0] = cur;
                max[1] = i;
                max[2] = size;
            }else if(cur == max[0]){
                if(size < max[2]){
                    max[1] = i;
                    max[2] = size;
                }
            }
        }
        for(size_t i = 0; i < matrix.size(); i++){
            if(matrix[max[1]][i])
                u.erase(i);
        }
        res.push_back(max[1]);
    }
    return res;
}
MealyFSM::MealyFSM(const std::string& filename){
    pt::ptree root;
    size_t ind;
    //Reading Json file into ptree
    try{
        pt::read_json(filename, root);
    }catch(pt::json_parser::json_parser_error& e1){
        throw std::runtime_error(e1.what());
    }
    //Making an array of transitions from a ptree
    for(const pt::ptree::value_type& state : root.get_child("transitions")){
        std::vector<Transition> arr;
        std::vector<std::string>::iterator pos;
        //Adding states into array
        try{
            if((pos = find(states.begin(), states.end(), state.first)) == states.end()){
                states.push_back(state.first);
                if(state.first == root.get<std::string>("initial_state"))
                    initial_state = states.size() - 1;
            }
            else{
                ind = pos - states.begin();
                if((size_t)(pos - states.begin()) < transitions.size()){
                    for(const pt::ptree::value_type& input : state.second){
                        Transition tr;
                        try{
                            if(find(states.begin(), states.end(), input.second.get<std::string>("state")) == states.end()){
                                states.push_back(input.second.get<std::string>("state"));
                                if(input.second.get<std::string>("state") == root.get<std::string>("initial_state"))
                                    initial_state = states.size() - 1;
                            }
                            tr.input = input.first;
                            tr.to = find(states.begin(), states.end(), input.second.get<std::string>("state")) - states.begin();
                        }catch(pt::ptree_bad_path& e2){
                            throw std::runtime_error(e2.what());
                        }
                        transitions[ind].push_back(tr);
                    }
                    for(size_t i = transitions.size(); i < states.size(); i++)
                        transitions.push_back(std::vector<Transition>());
                    continue;
                }
            }
        }catch(pt::ptree_bad_path& e3){
            throw std::runtime_error(e3.what());
        }
        //Copying each transition from ptree to structure
        for(const pt::ptree::value_type& input : state.second){
            Transition tr;
            try{
                if(find(states.begin(), states.end(), input.second.get<std::string>("state")) == states.end()){
                    states.push_back(input.second.get<std::string>("state"));
                    if(input.second.get<std::string>("state") == root.get<std::string>("initial_state"))
                        initial_state = states.size() - 1;
                }
                tr.input = input.first;
                tr.to = find(states.begin(), states.end(), input.second.get<std::string>("state")) - states.begin();
            }catch(pt::ptree_bad_path& e2){
                throw std::runtime_error(e2.what());
            }
            arr.push_back(tr);
        }
        transitions.push_back(arr);
        for(size_t i = transitions.size(); i < states.size(); i++)
            transitions.push_back(std::vector<Transition>());
    }
}
void MealyFSM::PathsInpSeqGen() const{
    //Making a tree root from initial_state
    Node root;
    root.state = initial_state;
    root.input = "";
    root.par = NULL;
    //Starting recursive search for possible transitions
    traverse(&root);
    //Printing input sequences
    print(&root);
}

void MealyFSM::StatesInpSeqGen() const{
    size_t j = 0;
    std::stack<std::string> s;
    std::vector<std::pair<size_t, std::string>> pred = BFSStates(initial_state);
    std::vector<size_t> cover;
    std::vector<std::vector<bool>> matrix(states.size());
    //Checking if there are unreacheable states
    for(size_t i = 0; i < states.size(); i++){
        if(pred[i].second == std::string() && i != initial_state){
            std::cerr<<"State "<<states[i]<<" is unreacheable"<<std::endl;
            j = 1;
        }
    }
    if(j) return;
    //Creating a matrix of input sequences and states
    for(size_t i = 0; i < states.size(); i++){
        if(i == initial_state) continue;
        for(size_t k = 0; k < states.size(); k++){
            if(k == initial_state)
                matrix[i].push_back(1);
            else
                matrix[i].push_back(0);
        }
        j = i;
        while(j != initial_state){
            matrix[i][j] = 1;
            j = pred[j].first;
        }
    }
    cover = greedy_set_cover(matrix);
    for(const auto& state : cover){
        j = state;
        while(j != initial_state){
            s.push(pred[j].second.data());
            j = pred[j].first;
        }
        while(!s.empty()){
            std::cout << s.top() << " ";
            s.pop();
        }
        std::cout << std::endl;
    }
}

void MealyFSM::TransitionsInpSeqGen() const{
    std::vector<std::vector<std::string>> input(states.size());
    //Vector of vectors where indexes mean first - index of state, second - index of transition from this state
    //Indexes in pair indicate preceding transition in this vector of vectors
    std::vector<std::vector<std::pair<size_t, size_t>>> pred = BFSTransitions(initial_state, input);
    std::stack<std::string> s;
    std::pair<size_t, size_t> k;
    std::vector<size_t> sz;
    size_t size = 0;
    size_t buf = 0;
    size_t pos = 0;
    for(size_t i = 0; i < input.size(); i++){
        for(size_t j = 0; j < input[i].size(); j++){
            if(input[i][j] == ""){
                std::cerr << "Transition " << states[i] << "->"<< transitions[i][j].input << "->" << states[transitions[i][j].to] << " is unreacheable" << std::endl;
                buf = 1;
            }
        }
    }
    if(buf) return;
    buf = 0;
    for(const auto& state : transitions){
        sz.push_back(state.size());
        size += state.size();
    }
    std::vector<std::vector<bool>> matrix;
    std::vector<size_t> p(size);
    std::vector<std::string> inp(size);
    //Creating a matrix of input sequences and transitions
    for(size_t i = 0; i < transitions.size(); i++){
        for(size_t j = 0; j < transitions[i].size(); j++){
            std::vector<bool> arr(size);
            k = std::make_pair(i, j);
            while(pred[k.first][k.second].first != k.first){
                buf = 0;
                if(!k.first)
                    buf = k.second;
                else{
                    for(size_t m = 0; m < k.first; m++)
                        buf += sz[m];
                    buf += k.second;
                }
                arr[buf] = 1;
                inp[buf] = input[k.first][k.second];
                if(k.first != i || k.second != j)
                    p[pos] = buf;
                pos = buf;
                k = pred[k.first][k.second];
            }
            buf = 0;
            if(!k.first)
                buf = k.second;
            else{
                for(size_t m = 0; m < k.first; m++)
                    buf += sz[m];
                buf += k.second;
            }
            arr[buf] = 1;
            if(i != initial_state) p[pos] = buf;
            p[buf] = buf;
            inp[buf] = input[k.first][k.second];
            matrix.push_back(arr);
        }
    }
    size_t j;
    std::vector<size_t> cover = greedy_set_cover(matrix);
    for(const auto& state : cover){
        j = state;
        while(p[j] != j){
            s.push(inp[j]);
            j = p[j];
        }
        s.push(inp[j]);
        while(!s.empty()){
            std::cout << s.top() << " ";
            s.pop();
        }
        std::cout << std::endl;
    }
}