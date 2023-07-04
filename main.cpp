#include <iostream>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace std;
namespace pt = boost::property_tree;
class Transition{
public:
    string from;
    string to;
    string input;
    string output;
};
class MealyFSM{
public:
    string initial_state;
    vector<string> states;
    vector<string> inputs;
    vector<string> outputs;
    vector<Transition> transitions;

    bool ReadFromJson(string filename){
        pt::ptree root;

        try{
            pt::read_json(filename, root);
        }catch(pt::json_parser::json_parser_error& e1){
            cerr << e1.what() << endl;
            return false;
        }

        for(pt::ptree::value_type& state : root.get_child("transitions")){
            for(pt::ptree::value_type& input : state.second){
                Transition tr;
                try{
                    tr.from = state.first;
                    tr.input = input.first;
                    tr.to = input.second.get<string>("state");
                    tr.output = input.second.get<string>("output");
                }catch(pt::ptree_bad_path& e2){
                    cerr << e2.what() << endl;
                    return false;
                }
                transitions.push_back(tr);
                if(find(inputs.begin(), inputs.end(), tr.input) == inputs.end())
                    inputs.push_back(tr.input);
                if(find(outputs.begin(), outputs.end(), tr.output) == outputs.end())
                    outputs.push_back(tr.output);
            }
            if(find(states.begin(), states.end(), state.first) == states.end())
                states.push_back(state.first);
            else{
                cerr << "Duplicate state: " << state.first << endl;
                return false;
            }
        }

        try{
            initial_state = root.get<string>("initial_state");
        }catch(pt::ptree_bad_path& e3){
            cerr << e3.what() << endl;
            return false;
        }
        return true;
    }
};
int main(int argc, char *argv[]){
    vector<string>::iterator it;
    if(argc > 2){cerr << "Too many arguments"; return 1;}
    if(argc < 2){cerr << "Too few arguments"; return 2;}
    MealyFSM machine;
    if(!machine.ReadFromJson(argv[1])) return 3;
    for(it = machine.states.begin(); it != machine.states.end(); it++)
        cout << *it << " ";
    cout << endl;
    for(it = machine.inputs.begin(); it != machine.inputs.end(); it++)
        cout << *it << " ";
    cout << endl;
    for(it = machine.outputs.begin(); it != machine.outputs.end(); it++)
        cout << *it << " ";
    cout << endl;
    vector<Transition>::iterator i;
    for(i = machine.transitions.begin(); i != machine.transitions.end(); i++){
        cout << "From: " << i->from << endl;
        cout << "Input: " << i->input << endl;
        cout << "To: " << i->to << endl;
        cout << "Output: " << i->output << endl << endl;
    }
    cout << "Initial state: " << machine.initial_state << endl;
    return 0;
}