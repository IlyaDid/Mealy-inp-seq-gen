#include <iostream>
#include <string>
#include <stack>
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
        for(size_t i = 0; i < transitions[n->state].second.size(); i++){
            ptr = n;
            found = false;
            //Checking branch possible transitions
            while(ptr != NULL){
                if(ptr->par != NULL && ptr->par->state == n->state && ptr->state == transitions[n->state].second[i].to && ptr->input == transitions[n->state].second[i].input){
                    found = true;
                    break;
                }
                ptr = ptr->par;
            }
            //If found adding new transition to the end of the branch
            if(found == false){
                Node child;
                child.state = transitions[n->state].second[i].to;
                child.input = transitions[n->state].second[i].input;
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
    void DFS(size_t u, std::vector<bool>& visited, std::vector<std::vector<std::string>>& pred){
        std::vector<Transition>::iterator it;
        visited[u] = true;
        for(it = transitions[u].second.begin(); it != transitions[u].second.end(); it++){
            if(visited[it->to] == false){
                pred[it->to] = pred[u];
                pred[it->to].push_back(it->input);
                DFS(it->to, visited, pred);
            }
        }
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
            //Checking if there were duplicate states in Json file as well as adding them into array
            if(find(states.begin(), states.end(), state.first) == states.end())
                states.push_back(state.first);
            else{
                std::cerr << "Duplicate state: " << state.first << std::endl;
                return false;
            }
            //Copying each transition from ptree to structure
            for(pt::ptree::value_type& input : state.second){
                Transition tr;
                try{
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
        std::vector<bool> visited;
        std::vector<std::vector<std::string>> pred(states.size());
        std::vector<std::string>::iterator it;
        for(size_t i = 0; i < states.size(); i++)
            visited.push_back(false);
        for(size_t i = 0; i < states.size(); i++)
            DFS(i, visited, pred);
        for(size_t i = 0; i < states.size(); i++){
            for(it = pred[i].begin(); it != pred[i].end(); it++)
                std::cout << *it << " ";
            std::cout << std::endl;
        }
    }

    void TransitionsInpSeqGen(){

    }
};
int main(int argc, char *argv[]){
    if(argc > 2){std::cerr << "Too many arguments"; return 1;}
    if(argc < 2){std::cerr << "Too few arguments"; return 2;}
    MealyFSM machine;
    if(!machine.ReadFromJson(argv[1])) return 3;
    machine.StatesInpSeqGen();
    return 0;
}