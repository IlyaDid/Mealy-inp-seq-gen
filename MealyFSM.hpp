#ifndef Mealy_FSM_H_
#define Mealy_FSM_H_
#include <string>
#include <vector>
class MealyFSM{
private:
    struct Transition{
        size_t to;
        std::string input;
        std::string output;
    };
    //Node is needed only to make a tree for MealyFSM::traverse. A "path" is a branch of a tree or a subtree 
    struct Node{
        size_t state;
        std::string input; //Input symbol which led to this state from preceding, if initial_state then equals ""
        Node *par; //Parent state which means preceding state in path
        std::vector<Node> children; //Vector of adjacent states which weren`t reached from this state in this path before
    };
    void traverse(Node *n) const;
    void print(Node *n) const;
    std::vector<std::pair<size_t, std::string>> BFSStates(const size_t& u) const;
    std::vector<std::vector<std::pair<size_t, size_t>>> BFSTransitions(const size_t& u, std::vector<std::vector<std::string>>& input) const;
    std::vector<size_t> greedy_set_cover(const std::vector<std::vector<bool>>& matrix) const;
public:
    size_t initial_state;
    std::vector<std::string> states;
    std::vector<std::vector<Transition>> transitions;
    void PathsInpSeqGen() const;
    void StatesInpSeqGen() const;
    void TransitionsInpSeqGen() const;
    explicit MealyFSM(const std::string& filename);
};
#endif