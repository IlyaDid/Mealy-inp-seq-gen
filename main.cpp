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
            cerr << e1.what();
            return false;
        }

        for(pt::ptree::value_type& state : root.get_child("transitions")){
            for(pt::ptree::value_type& input : state.second){
                Transition tr;
                try{
                    tr.from = state.first;
                    tr.input = input.first;
                    tr.to = input.second.get<string>("state", 0);
                    tr.output = input.second.get<string>("output", 0);
                }catch(pt::ptree_bad_path& e2){
                    cerr << e2.what();
                    return false;
                }
                transitions.push_back(tr);
                if(find(states.begin(), states.end(), tr.from) == states.end())
                    states.push_back(tr.from);
                else{
                    cerr << "Duplicate state" << tr.from;
                    return false;
                }
                if(find(inputs.begin(), inputs.end(), tr.input) == inputs.end())
                    inputs.push_back(tr.input);
                if(find(outputs.begin(), outputs.end(), tr.output) == outputs.end())
                    outputs.push_back(tr.output);
            }
        }

        try{
            initial_state = root.get<string>("initial_state", 0);
        }catch(pt::ptree_bad_path& e3){
            cerr << e3.what();
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
    machine.ReadFromJson(argv[1]);
    for(it = machine.states.begin(); it != machine.states.end(); it++)
        cout << *it << " ";
    cout << "\n";
    for(it = machine.inputs.begin(); it != machine.inputs.end(); it++)
        cout << *it << " ";
    cout << "\n";
    for(it = machine.outputs.begin(); it != machine.outputs.end(); it++)
        cout << *it << " ";
    cout << "\n";
    vector<Transition>::iterator i;
    for(i = machine.transitions.begin(); i != machine.transitions.end(); i++){
        cout << "From: " << i->from << "\n";
        cout << "Input: " << i->input << "\n";
        cout << "To: " << i->to << "\n";
        cout << "Output: " << i->output << "\n\n";
    }
    return 0;
}