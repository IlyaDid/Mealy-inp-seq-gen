#include <iostream>
#include <string>
#include <stack>
#include <list>
#include <queue>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
namespace pt = boost::property_tree;
namespace po = boost::program_options;
class MealyFSM{
private:
    struct Transition{
        size_t to;
        std::string input;
    };
    //Node is needed only to make a tree for MealyFSM::traverse. A "path" is a branch of a tree or a subtree 
    struct Node{
        size_t state;
        std::string input; //Input symbol which led to this state from preceding, if initial_state then equals ""
        Node *par; //Parent state which means preceding state in path
        std::vector<Node> children; //Vector of adjacent states which weren`t reached from this state in this path before
    };
    void traverse(Node *n) const{
        bool found;
        Node *ptr;
        for(const auto& transition : transitions[n->state].second){
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
    void print(Node *n, bool start) const{
        Node *ptr = n;
        std::stack<std::string> s;
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
        //Initializing print for each child branch
        for(size_t i = 0; i < n->children.size(); i++){
            if(!start) std::cout << std::endl;
            start = false;
            print(&n->children[i], start);
        }
    }
    std::vector<std::pair<size_t, std::string>> BFSStates(const size_t& u) const{
        size_t v;
        std::queue<size_t> q;
        std::vector<bool> color;
        //Vector of preceding states and input symbols to reach any state
        std::vector<std::pair<size_t, std::string>> pred(states.size());
        for(size_t i = 0; i < states.size(); i++)
            color.push_back(0);
        color[u] = 1;
        pred[u] = std::make_pair(0, std::string());
        q.push(u);
        while(!q.empty()){
            v = q.front();
            q.pop();
            for(const auto& transition : transitions[v].second){
                if(!color[transition.to]){
                    pred[transition.to] = std::make_pair(v, transition.input);
                    color[transition.to] = 1;
                    q.push(transition.to);
                }
            }
        }
        return pred;
    }
    std::vector<std::vector<std::pair<size_t, size_t>>> BFSTransitions(const size_t& u, std::vector<std::vector<std::string>>& input) const{
        size_t to;
        std::pair<size_t, size_t> v;
        std::queue<std::pair<size_t, size_t>> q;
        std::vector<std::vector<bool>> color(states.size());
        //Vector of vectors where indexes mean first - index of state, second - index of transition from this state
        //Indexes in pair indicate preceding transition in this vector of vectors
        std::vector<std::vector<std::pair<size_t, size_t>>> pred(states.size());
        for(size_t i = 0; i < transitions.size(); i++){
            for(size_t j = 0; j < transitions[i].second.size(); j++){
                color[i].push_back(0);
                pred[i].push_back(std::make_pair(0,0));
                input[i].push_back("");
            }
        }
        for(size_t i = 0; i < transitions[u].second.size(); i++){
            color[u][i] = 1;
            pred[u][i] = std::make_pair(u, i);
            input[u][i] = transitions[u].second[i].input;
            q.push(std::make_pair(u, i));
        }
        while(!q.empty()){
            v = q.front();
            q.pop();
            to = transitions[v.first].second[v.second].to;
            if(to < transitions.size()){
                for(size_t i = 0; i < transitions[to].second.size(); i++){
                    if(!color[to][i]){
                        input[to][i] = transitions[to].second[i].input;
                        color[to][i] = 1;
                        pred[to][i] = std::make_pair(v.first, v.second);
                        q.push(std::make_pair(to, i));
                    }
                }
            }
        }
        return pred;
    }
    std::vector<size_t> greedy_set_cover(const std::vector<std::vector<bool>>& matrix) const{
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
public:
    size_t initial_state;
    std::vector<std::string> states;
    std::vector<std::pair<size_t, std::vector<Transition>>> transitions;

    bool ReadFromJson(const std::string& filename){
        pt::ptree root;
        //Reading Json file into ptree
        try{
            pt::read_json(filename, root);
        }catch(pt::json_parser::json_parser_error& e1){
            std::cerr << e1.what() << std::endl;
            return false;
        }
        //Making an array of transitions from a ptree
        for(const pt::ptree::value_type& state : root.get_child("transitions")){
            std::vector<Transition> arr;
            //Adding states into array
            if(find(states.begin(), states.end(), state.first) == states.end())
                states.push_back(state.first);
            //Copying each transition from ptree to structure
            for(const pt::ptree::value_type& input : state.second){
                Transition tr;
                try{
                    if(find(states.begin(), states.end(), input.second.get<std::string>("state")) == states.end())
                        states.push_back(input.second.get<std::string>("state"));
                    tr.input = input.first;
                    tr.to = find(states.begin(), states.end(), input.second.get<std::string>("state")) - states.begin();
                }catch(pt::ptree_bad_path& e2){
                    std::cerr << e2.what() << std::endl;
                    return false;
                }
                arr.push_back(tr);
            }
            transitions.push_back(std::make_pair(find(states.begin(), states.end(), state.first) - states.begin(), arr));
        }
        bool found;
        //Adding states that don't have transitions to transitions array
        for(size_t i = 0; i < states.size(); i++){
            std::vector<Transition> arr;
            found = 0;
            for(const auto& state : transitions){
                if(state.first == i)
                    found = 1;
            }
            if(!found)
                transitions.push_back(std::make_pair(i, arr));
        }
        //Seeking position of initial_state in states array
        try{
            if(find(states.begin(), states.end(), root.get<std::string>("initial_state")) != states.end())
                initial_state = find(states.begin(), states.end(), root.get<std::string>("initial_state")) - states.begin();
            else{
                std::cerr << "Invalid initial_state" << std::endl;
                return false;
            }
        }catch(pt::ptree_bad_path& e3){
            std::cerr << e3.what() << std::endl;
            return false;
        }
        return true;
    }

    void PathsInpSeqGen() const{
        //Making a tree root from initial_state
        Node root;
        root.state = initial_state;
        root.input = "";
        root.par = NULL;
        //Starting recursive search for possible transitions
        traverse(&root);
        //Printing input sequences
        print(&root, true);
        std::cout << std::endl;
    }

    void StatesInpSeqGen() const{
        size_t j;
        std::stack<std::string> s;
        std::vector<std::pair<size_t, std::string>> pred = BFSStates(initial_state);
        std::vector<size_t> cover;
        std::vector<std::vector<bool>> matrix(states.size());
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

    void TransitionsInpSeqGen() const{
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
        for(const auto& state : transitions){
            sz.push_back(state.second.size());
            size += state.second.size();
        }
        std::vector<std::vector<bool>> matrix;
        std::vector<size_t> p(size);
        std::vector<std::string> inp(size);
        //Creating a matrix of input sequences and transitions
        for(size_t i = 0; i < transitions.size(); i++){
            for(size_t j = 0; j < transitions[i].second.size(); j++){
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
};
int main(int argc, char *argv[]){
    po::options_description desc("Options");
    po::positional_options_description pos_desc;
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