#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <utility>
#include <map>
#include <set>
#include <stdexcept>
#include <bitset>


// MAX 12 depth

const std::map<uint8_t, std::vector<uint8_t>> MAP_VALUE_NUMBER( {{ 0 , {0, 1, 2, 3, 4, 5, 6, }},
                                        {1 , {1, 7, 8, 9, 10, 11, 12 }},
                                        {2 , {2, 8, 13, 14, 15, 16, 17 }},
                                        {3 , {3, 9, 14, 18, 19, 20, 21 }},
                                        {4 , {4, 10, 15, 19, 22, 23, 24 }},
                                        {5 , {5, 11, 16, 20, 23, 25, 26 }},
                                        {6 , {6, 12, 17, 21, 24, 26, 27 }}});

const std::map<uint8_t, std::pair<uint8_t, uint8_t>> MAP_NUMBER_VALUES ({{0, {0, 0}},{1, {0, 1}},{2,  {0, 2}}, {3, {0, 3}},
                                          {4, {0, 4}},{5, {0, 5}},{6,  {0, 6}}, {7, {1, 1}}, 
                                          {8, {1, 2}},{9, {1, 3}},{10, {1, 4}},{11, {1, 5}},
                                          {12,{1, 6}},{13,{2, 2}},{14, {2, 3}},{15, {2, 4}},
                                          {16,{2, 5}},{17,{2, 6}},{18, {3, 3}},{19, {3, 4}},
                                          {20,{3, 5}},{21,{3, 6}},{22, {4, 4}},{23, {4, 5}},
                                          {24,{4, 6}},{25,{5, 5}},{26, {5, 6}},{27, {6, 6}}});



uint8_t choose_pos(std::pair<uint8_t, uint8_t> dom, uint8_t last_number) {
    if (dom.first == last_number)  return dom.second;
    else return dom.first;
}

struct State {

    uint8_t last_number; // 0 ... 6
    uint8_t last_dom;

    std::string data = "";
    std::bitset<28> bank;
    
    State(uint8_t last_number_, uint8_t new_dom) : last_number(last_number_), 
                                             last_dom(new_dom), 
                                             data(std::to_string(new_dom)) {
        bank[new_dom] = 1;
    }
    
    State(State* st, uint8_t new_dom) : last_number(choose_pos(MAP_NUMBER_VALUES.at(new_dom), st->last_number)),  
                                            last_dom(new_dom), 
                                            data(st->data), 
                                            bank(st->bank) {

        if (!bank[last_dom]) {
            data += "-" + std::to_string(last_dom);
            bank[last_dom] = 1;
        } else {
            throw std::invalid_argument( "received invalid last_dom (not in bank)" );
        }
        
    }
    
};

std::ostream& operator << (std::ostream& os, State* st) {
    return os << st->data;
}


std::ostream& operator << (std::ostream& os, std::vector<uint8_t> v) {
    os << "[ ";
    for (uint8_t i = 0; i < v.size(); ++i) {
        os << v[i] << " ";
    }
    return os << "]";
}

// Non binary tree.
struct Tree {
    State* state;
    
    std::vector<Tree> children{};
    Tree(State* state_) : state(state_) {}

    void add(const Tree& child) {
        children.push_back(child);
    }

    void add(State* state_ ) {
        children.push_back(Tree{state_});
    }


    void print_leaves() const{
        if (children.size() != 0) {
            for (const auto& it: children) {
                it.print_leaves();
            }
        } else {
            std::cout << state << "\n";
        }
    }
};

std::vector<State*> leaves;


void make_tree_rec_dom(Tree& root, uint8_t depth=7, uint8_t level=1) {
    if (level == depth) { 
        leaves.push_back(root.state); 
        return;}

    const  std::vector<uint8_t>& vec_nums = MAP_VALUE_NUMBER.at(root.state->last_number);
    for (auto& num_dom : vec_nums) {          
        if (! root.state->bank[num_dom]) {
            root.add(new State(root.state, num_dom));
        }
    }

    delete root.state;

    for (auto& child : root.children) {
        make_tree_rec_dom(child, depth, level + 1);
    }
}


int main(int argc, char** argv) {
    srand(time(0));
    int depth = std::atoi(argv[1]);

    uint8_t rand_number_dom = rand() % 28;
    
    uint8_t last_number = MAP_NUMBER_VALUES.at(rand_number_dom).second;
    
    Tree root(new State( last_number, rand_number_dom)); 

    make_tree_rec_dom(root, depth);
    
    for (int i = 0; i < leaves.size(); ++i) {
        std::cout << leaves[i]->data << "\n";
        delete leaves[i];
    }

    return 0;
}
