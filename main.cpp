#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace std;
namespace pt = boost::property_tree;
class MealyFSM{
private:
    class Transition{
    public:
        int to;
        string input;
    };
    class Node{
    public:
        int state;
        string input;
        Node *par;
        vector<Node> children;
    };
    void search(Node *n){
        bool found;
        Node *ptr;
        for(int i = 0; i < transitions[n->state].second.size(); i++){
            ptr = n;
            found = false;
            //Checking branch possible transitions
            while(ptr != NULL){
                if(ptr->state == transitions[n->state].second[i].to && ptr->input == transitions[n->state].second[i].input){
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
        for(int i = 0; i < n->children.size(); i++)
           search(&n->children[i]);
    }
    void print(Node *n){ 
        Node *ptr = n;
        stack<string> s;
        //Adding all transitions in branch to stack
        while(ptr != NULL){
            s.push(ptr->input);
            ptr = ptr->par;
        }
        //Printing input symbols
        while(!s.empty()){
            cout << s.top() << " ";
            s.pop();
        }
        //Initializing print for each child branch
        for(int i = 0; i < n->children.size(); i++){
            cout << endl;
            print(&n->children[i]);
        }
    }
public:
    int initial_state;
    vector<string> states;
    vector<pair<int, vector<Transition>>> transitions;

    bool ReadFromJson(string filename){
        pt::ptree root;
        //Reading Json file into ptree
        try{
            pt::read_json(filename, root);
        }catch(pt::json_parser::json_parser_error& e1){
            cerr << e1.what() << endl;
            return false;
        }
        //Making an array of transitions from a ptree
        for(pt::ptree::value_type& state : root.get_child("transitions")){
            vector<Transition> arr;
            //Checking if there were duplicate states in Json file as well as adding them into array
            if(find(states.begin(), states.end(), state.first) == states.end())
                states.push_back(state.first);
            else{
                cerr << "Duplicate state: " << state.first << endl;
                return false;
            }
            //Copying each transition from ptree to structure
            for(pt::ptree::value_type& input : state.second){
                Transition tr;
                try{
                    tr.input = input.first;
                    tr.to = find(states.begin(), states.end(), input.second.get<string>("state")) - states.begin();
                }catch(pt::ptree_bad_path& e2){
                    cerr << e2.what() << endl;
                    return false;
                }
                arr.push_back(tr);
            }
            transitions.push_back(make_pair(find(states.begin(), states.end(), state.first) - states.begin(), arr));
        }
        //Seeking position of initial_state in states array
        try{
            if(find(states.begin(), states.end(), root.get<string>("initial_state")) != states.end())
                initial_state = find(states.begin(), states.end(), root.get<string>("initial_state")) - states.begin();
            else{
                cerr << "Invalid initial_state" << endl;
                return false;
            }
        }catch(pt::ptree_bad_path& e3){
            cerr << e3.what() << endl;
            return false;
        }
        return true;
    }

    void InpSeqGen(){
        //Making a tree root from initial_state
        Node root;
        root.state = initial_state;
        root.input = "";
        root.par = NULL;
        //Starting recursive search for possible transitions
        search(&root);
        //Printing input sequences
        print(&root);
    }
};
int main(int argc, char *argv[]){
    if(argc > 2){cerr << "Too many arguments"; return 1;}
    if(argc < 2){cerr << "Too few arguments"; return 2;}
    MealyFSM machine;
    if(!machine.ReadFromJson(argv[1])) return 3;
    machine.InpSeqGen();
    return 0;
}