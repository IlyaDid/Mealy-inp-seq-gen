#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
namespace pt = boost::property_tree;
class MealyFSM{
private:
    class Transition{
    public:
        size_t to;
        std::string input;
    };
    class Node{
    public:
        size_t state;
        std::string input;
        Node *par;
        std::vector<Node> children;
    };
    void search(Node *n){
        bool found;
        Node *ptr;
        for(auto it : transitions[n->state].second){
            ptr = n;
            found = false;
            //Checking branch possible transitions
            while(ptr != NULL){
                if(ptr->par != NULL && ptr->par->state == n->state && ptr->state == it.to && ptr->input == it.input){
                    found = true;
                    break;
                }
                ptr = ptr->par;
            }
            //If found adding new transition to the end of the branch
            if(found == false){
                Node child;
                child.state = it.to;
                child.input = it.input;
                child.par = n;
                n->children.push_back(child);
            }
        }
        //Initializing search for each child branch
        for(size_t i = 0; i < n->children.size(); i++)
           search(&n->children[i]);
    }
    void print(Node *n){
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
            if(n->par != NULL) std::cout << std::endl;
            print(&n->children[i]);
        }
    }
    std::vector<std::pair<size_t, std::string>> BFSStates(size_t u){
        size_t v;
        std::queue<size_t> q;
        std::vector<bool> color;
        std::vector<std::pair<size_t, std::string>> pred(states.size());
        for(size_t i = 0; i < states.size(); i++)
            color.push_back(0);
        color[u] = 1;
        pred[u] = std::make_pair(0, NULL);
        q.push(u);
        while(!q.empty()){
            v = q.front();
            q.pop();
            for(auto it : transitions[v].second){
                if(!color[it.to]){
                    pred[it.to] = std::make_pair(v, it.input);
                    color[it.to] = 1;
                    q.push(it.to);
                }
            }
        }
        return pred;
    }
    std::vector<std::vector<std::pair<size_t, size_t>>> BFSTransitions(size_t u, std::vector<std::vector<std::string>>& input){
        size_t to;
        std::pair<size_t, size_t> v;
        std::queue<std::pair<size_t, size_t>> q;
        std::vector<std::vector<bool>> color(states.size());
        std::vector<std::vector<std::pair<size_t, size_t>>> pred(states.size());
        for(size_t i = 0; i < transitions.size(); i++){
            for(auto t : transitions[i].second){
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
public:
    size_t initial_state;
    std::vector<std::string> states;
    std::vector<std::pair<size_t, std::vector<Transition>>> transitions;

    bool ReadFromJson(std::string filename){
        pt::ptree root;
        //Reading Json file into ptree
        try{
            pt::read_json(filename, root);
        }catch(pt::json_parser::json_parser_error& e1){
            std::cerr << e1.what() << std::endl;
            return false;
        }
        //Making an array of transitions from a ptree
        for(pt::ptree::value_type& state : root.get_child("transitions")){
            std::vector<Transition> arr;
            //Adding states into array
            if(find(states.begin(), states.end(), state.first) == states.end())
                states.push_back(state.first);
            //Copying each transition from ptree to structure
            for(pt::ptree::value_type& input : state.second){
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
            transitions.push_back(make_pair(states.size() - 1, arr));
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

    void PathsInpSeqGen(){
        //Making a tree root from initial_state
        Node root;
        root.state = initial_state;
        root.input = "";
        root.par = NULL;
        //Starting recursive search for possible transitions
        search(&root);
        //Printing input sequences
        print(&root);
        std::cout << std::endl;
    }

    void StatesInpSeqGen(){
        size_t j;
        std::stack<std::string> s;
        std::vector<std::pair<size_t, std::string>> pred = BFSStates(initial_state);
        for(size_t i = 0; i < states.size(); i++){
            if(i == initial_state) continue;
            j = i;
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

    void TransitionsInpSeqGen(){
        std::vector<std::vector<std::string>> input(states.size());
        std::vector<std::vector<std::pair<size_t, size_t>>> pred = BFSTransitions(initial_state, input);
        std::stack<std::string> s;
        std::pair<size_t, size_t> k;
        for(size_t i = 0; i < states.size(); i++){
            for(size_t j = 0; j < transitions[i].second.size(); j++){
                k = std::make_pair(i, j);
                while(pred[k.first][k.second].first != k.first){
                    s.push(input[k.first][k.second]);
                    k = pred[k.first][k.second];
                    //std::cout << k.first << " " << k.second << " " << input[k.first][k.second] << std::endl;
                }
                s.push(input[k.first][k.second]);
                while(!s.empty()){
                    std::cout << s.top() << " ";
                    s.pop();
                }
                std::cout << std::endl;
            }
        }
    }
};
int main(int argc, char *argv[]){
    if(argc > 2){std::cerr << "Too many arguments"; return 1;}
    if(argc < 2){std::cerr << "Too few arguments"; return 2;}
    MealyFSM machine;
    if(!machine.ReadFromJson(argv[1])) return 3;
    machine.TransitionsInpSeqGen();
    return 0;
}