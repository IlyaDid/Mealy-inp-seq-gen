#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <queue>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
namespace pt  = boost::property_tree;
struct Transition{
    size_t input;
    size_t output;
    size_t to;
};
void RandomMealyGen(const unsigned int& seed, const std::string& file,const size_t& states_n, const size_t& min, const size_t& max, const size_t& symb_in, const size_t& symb_out){
    srand(seed);
    pt::ptree root;
    pt::ptree transitions;
    size_t tr_num; //Number of transitions for current state
    std::vector<size_t> in; //Vector of input symbols for current state
    size_t symb; //New random symbol
    std::queue<size_t> q;
    std::vector<std::vector<Transition>> arr(states_n);
    std::vector<bool> color;
    size_t from;
    size_t to;
    size_t initial_state = rand()%states_n;
    for(size_t i = 0; i < states_n; i++)
        color.push_back(0);
    color[initial_state] = 1;
    q.push(initial_state);
    while(!q.empty()){
        tr_num = min + rand()%(max - min);
        for(size_t k = 0; k < tr_num; k++){
            symb = rand()%symb_in;
            while(find(in.begin(), in.end(), symb) != in.end())
                symb = rand()%symb_in;
            in.push_back(symb);
        }
        for(size_t j = 0; j < tr_num; j++){
            to = rand()%states_n;
            arr[q.front()].push_back({in.back(), rand()%symb_out, to});
            if(!color[to]){
                q.push(to);
                color[to] = 1;
            }
            in.pop_back();
        }
        q.pop();
        if(q.empty()){
            for(size_t i = 0; i < color.size(); i++){
                if(!(color[i])){
                    from = rand()%states_n;
                    while(!color[from]) from = rand()%states_n;
                    symb = 0;
                    while(symb != symb_in){
                        for(size_t j = 0; j < arr[from].size(); j++){
                            if(arr[from][j].input == symb){
                                symb++;
                                if(symb == symb_in)
                                    std::cerr << "Couldn't generate valid machine with this seed. Please try another" << std::endl;
                                break;
                            }
                            if(j == arr[from].size() - 1){
                                arr[from].push_back({symb, rand()%symb_out, i});
                                q.push(i);
                                color[i] = 1;
                                symb = states_n;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    for(size_t i = 0; i < arr.size(); i++){
        pt::ptree state;
        for(const auto& tr : arr[i]){
            pt::ptree transition;
            transition.put("state", "q" + std::to_string(tr.to + 1));
            transition.put("output", "w" + std::to_string(tr.output + 1));
            state.add_child("z" + std::to_string(tr.input + 1), transition);
        }
        transitions.add_child("q" + std::to_string(i + 1), state);
    }
    root.add_child("transitions", transitions);
    root.put("initial_state", "q" + std::to_string(initial_state + 1));
    pt::write_json(file, root);
}
int main(int argc, char *argv[]){
    po::options_description desc("Options");
    unsigned int seed;
    size_t states_n;
    size_t min;
    size_t max;
    size_t symb_in;
    size_t symb_out;
    std::string file;
    po::variables_map vm;
    desc.add_options()
        ("help,h", "Show help")
        ("seed,s", po::value<unsigned int>(&seed)->required(), "Seed")
        ("states", po::value<size_t>(&states_n)->required(), "Number of states")
        ("min", po::value<size_t>(&min)->required(), "Minimal number of transitions from state")
        ("max", po::value<size_t>(&max)->required(), "Maximal number of transitions from state")
        ("in", po::value<size_t>(&symb_in)->required(), "Number of input symbols")
        ("out", po::value<size_t>(&symb_out)->required(), "Number of output symbols")
        ("file,f", po::value<std::string>(&file)->required(), "Filename to save result")
    ;
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).run();
    po::store(parsed, vm);
    if(vm.count("help")){std::cerr<<desc<<std::endl;return 0;}
    po::notify(vm);
    RandomMealyGen(seed, file, states_n, min, max, symb_in, symb_out);
    return 0;
}