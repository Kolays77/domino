#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <utility>
#include <map>
#include <set>
#include <stdexcept>


const std::map<char, std::vector<char>> MAP_VALUE_NUMBER( {{ 0 , {0, 1, 2, 3, 4, 5, 6, }},
                                        {1 , {1, 7, 8, 9, 10, 11, 12 }},
                                        {2 , {2, 8, 13, 14, 15, 16, 17 }},
                                        {3 , {3, 9, 14, 18, 19, 20, 21 }},
                                        {4 , {4, 10, 15, 19, 22, 23, 24 }},
                                        {5 , {5, 11, 16, 20, 23, 25, 26 }},
                                        {6 , {6, 12, 17, 21, 24, 26, 27 }}});

const std::map<char, std::pair<char, char>> MAP_NUMBER_VALUES ({{0, {0, 0}},{1, {0, 1}},{2,  {0, 2}}, {3, {0, 3}},
                                          {4, {0, 4}},{5, {0, 5}},{6,  {0, 6}}, {7, {1, 1}}, 
                                          {8, {1, 2}},{9, {1, 3}},{10, {1, 4}},{11, {1, 5}},
                                          {12,{1, 6}},{13,{2, 2}},{14, {2, 3}},{15, {2, 4}},
                                          {16,{2, 5}},{17,{2, 6}},{18, {3, 3}},{19, {3, 4}},
                                          {20,{3, 5}},{21,{3, 6}},{22, {4, 4}},{23, {4, 5}},
                                          {24,{4, 6}},{25,{5, 5}},{26, {5, 6}},{27, {6, 6}}});



char choose_pos(std::pair<char, char> dom, char last_number) {
    if (dom.first == last_number)  return dom.second;
    else return dom.first;
}

struct State {

    char last_number; // 0 ... 6
    char last_dom;

    std::string data = "";
    std::unordered_set<char> bank;
    
    State(char last_number_, char new_dom) : last_number(last_number_), 
                                             last_dom(new_dom), 
                                             data(std::to_string(new_dom)) {
        bank.insert(new_dom);
    }
    
    State(const State& st, char new_dom) : last_number(choose_pos(MAP_NUMBER_VALUES.at(new_dom), st.last_number)),  
                                            last_dom(new_dom), 
                                            data(st.data), 
                                            bank(st.bank) {

        auto it = bank.find(last_dom);
        if (it == bank.end()) {
            data += "-" + std::to_string(last_dom);
            bank.insert(last_dom);
        } else {
            throw std::invalid_argument( "received invalid last_dom (not in bank)" );
        }
        
    }
    
};

std::ostream& operator << (std::ostream& os, State st) {
    return os << st.data;
}


std::ostream& operator << (std::ostream& os, std::vector<char> v) {
    os << "[ ";
    for (char i = 0; i < v.size(); ++i) {
        os << v[i] << " ";
    }
    return os << "]";
}

// Non binary tree.
struct Tree {
    State state;
    char level = 1;
    // Tree* parent = nullptr;
    std::vector<Tree> children{};

    Tree(State state_) : state(state_) {}
    Tree(State state_, char level_) : state(state_), level(level_) {}

    void add(const Tree& child) {
        children.push_back(child);
    }

    void add(State state_ ) {
        children.push_back(Tree{state_, level+1});
    }

    void prchar() const{
        std::string space(level-1, '\t');
        std::cout << space << state << "\n";
        if (children.size()) {
            for (const auto& it: children) {
                it.prchar();
            }
        }
    }

    void prchar_leaves() const{
        if (children.size() != 0) {
            for (const auto& it: children) {
                it.prchar_leaves();
            }
        } else {
            std::cout << state << "\n";
        }
    }
};



void make_tree_rec_dom(Tree& root, char depth=7, char level=1) {
    if (level == depth) {return;}


    const  std::vector<char>& vec_nums = MAP_VALUE_NUMBER.at(root.state.last_number);
    for (auto& num_dom : vec_nums) {   
        auto it_f = root.state.bank.find(num_dom);
        
        if (it_f == root.state.bank.end()) {
            root.add(State(root.state, num_dom));
            make_tree_rec_dom(root.children.back(), depth, level + 1);
        }
    }
}


int main(int argc, char** argv) {
    srand(time(0));
    int depth = std::atoi(argv[1]);

    char rand_number_dom = rand() % 28;
    char last_number = MAP_NUMBER_VALUES.at(rand_number_dom).second;
    
    Tree root(State( last_number, rand_number_dom)); 

    make_tree_rec_dom(root, depth);
    return 0;
}
