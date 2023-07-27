#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
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
    size_t tr; //Number of transitions for current state
    std::vector<size_t> in; //Vector of input symbols for current state
    size_t symb; //New random symbol
    for(size_t i = 0; i < states_n; i++){
        std::vector<Transition> arr;
        pt::ptree state;
        tr = min + rand()%(max - min);
        for(size_t k = 0; k < tr; k++){
            symb = rand()%symb_in;
            while(find(in.begin(), in.end(), symb) != in.end())
                symb = rand()%symb_in;
            in.push_back(symb);
        }
        for(size_t j = 0; j < tr; j++){
            pt::ptree transition;
            arr.push_back({in.back(), rand()%symb_out, rand()%states_n});
            in.pop_back();
            transition.put("state", "q" + std::to_string(arr[j].to + 1));
            transition.put("output", "w" + std::to_string(arr[j].output + 1));
            state.add_child("z" + std::to_string(arr[j].input + 1), transition);
        }
        transitions.add_child("q" + std::to_string(i + 1), state);
    }
    root.add_child("transitions", transitions);
    root.put("initial_state", "q" + std::to_string(rand()%states_n + 1));
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
        ("seed,s", po::value<unsigned int>(&seed)->required(), "Provide program with a seed")
        ("states", po::value<size_t>(&states_n)->required(), "Provide program with number of states")
        ("min", po::value<size_t>(&min)->required(), "Provide program with minimal number of transitions from state")
        ("max", po::value<size_t>(&max)->required(), "Provide program with maximal number of transitions from state")
        ("in", po::value<size_t>(&symb_in)->required(), "Provide program with number of input symbols")
        ("out", po::value<size_t>(&symb_out)->required(), "Provide program with number of output symbols")
        ("file,f", po::value<std::string>(&file)->required(), "Provide program with filename to save result")
    ;
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).run();
    po::store(parsed, vm);
    po::notify(vm);
    if(vm.count("help")){std::cerr<<desc<<std::endl;return 0;}
    RandomMealyGen(seed, file, states_n, min, max, symb_in, symb_out);
    return 0;
}