#include <map>
#include <utility>
#include <vector>
#include <cstdint>
#include <bitset>
#include <queue>

#include <bitset>
#include <iostream>
#include <stdexcept>
#include <string>
#include <array>
#include <thread>

#include <bit>

std::bitset<28> make_bitset(std::vector<int>&& v) {
    std::bitset<28> b;
    for (int vv : v) b[vv] = true;
    return b; 
}



const std::map<int, std::bitset<28>> MAP_VALUE_NUMBER({{ 0 , make_bitset({0, 1, 2, 3, 4, 5, 6})},
                                        {1 , make_bitset({1, 7, 8, 9, 10, 11, 12 })},
                                        {2 , make_bitset({2, 8, 13, 14, 15, 16, 17 })},
                                        {3 , make_bitset({3, 9, 14, 18, 19, 20, 21 })},
                                        {4 , make_bitset({4, 10, 15, 19, 22, 23, 24 })},
                                        {5 , make_bitset({5, 11, 16, 20, 23, 25, 26 })},
                                        {6 , make_bitset({6, 12, 17, 21, 24, 26, 27 })}});

const std::map<int, std::pair<int, int>> MAP_NUMBER_VALUES ({{0, {0, 0}},{1, {0, 1}},{2,  {0, 2}}, {3, {0, 3}},
                                        {4, {0, 4}},{5, {0, 5}},{6,  {0, 6}}, {7, {1, 1}}, 
                                        {8, {1, 2}},{9, {1, 3}},{10, {1, 4}},{11, {1, 5}},
                                        {12,{1, 6}},{13,{2, 2}},{14, {2, 3}},{15, {2, 4}},
                                        {16,{2, 5}},{17,{2, 6}},{18, {3, 3}},{19, {3, 4}},
                                        {20,{3, 5}},{21,{3, 6}},{22, {4, 4}},{23, {4, 5}},
                                        {24,{4, 6}},{25,{5, 5}},{26, {5, 6}},{27, {6, 6}}});


// g++ main.cpp -O2 -lgmp -pthread -DBOOST

class Timer {
    std::chrono::time_point<std::chrono::steady_clock> timePoint;
    size_t value;
public:
    void start() { timePoint = std::chrono::steady_clock::now(); }
    void finish() {
        auto curr = std::chrono::steady_clock::now();    
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(curr - timePoint);
        value = elapsed.count();
    }
    size_t operator()() const { return value; }
};


inline int choose_pos(const std::pair<int, int>& dom, int last_number) {
    return (dom.first == last_number) ? dom.second : dom.first;
}

struct GlobalState {
    std::vector<int> dominoes; // max size == 28
    std::vector<int> last_numbers; // max size == 28
    std::bitset<28> bank;
    std::vector<long long unsigned> count_;

    int avaible = 0;

    // GlobalState() : dominoes({}), last_numbers({}) {};

    GlobalState(int domino, int last_number) : dominoes(std::vector<int>(28, -1)), 
                                               last_numbers(std::vector<int>(28, -1)),
                                               count_(std::vector<long long unsigned>(28, 0)) {
        
        dominoes[avaible] = domino;
        last_numbers[avaible] = last_number;
        bank[domino] = 1;
        ++avaible; 
    }

    int back() {
        return last_numbers[avaible-1];
    }

    void count(int level) {
        count_[level-1]++;
    }


    unsigned long long get_count(int level) {
        return count_[level-1];
    }


    void add(int domino_number) {
        bank[domino_number] = 1;
        dominoes[avaible] = domino_number;
        last_numbers[avaible] = choose_pos(MAP_NUMBER_VALUES.at(domino_number), back());
        ++avaible;
    } 


    void pop() {
        bank[dominoes[avaible-1]] = 0;
        --avaible;
    }
};



void rec_run_and_count(GlobalState& st,  int max_level=7, int level=1) {
    st.count(level);
    if (level >= max_level) return;

    // [0, 1, 0, 0, ... ] = bank
    // [1, 0, 1, 1, ... ] = ~bank
    // [1, 0, 0, 1, ... ] = MAP
    // [1, 0, 0, 1, ... ] = MAP & ~bank : avaible nodes

    auto next_moves = ~st.bank & MAP_VALUE_NUMBER.at(st.back());
    for (size_t pos = next_moves._Find_first(); pos < 28; pos = next_moves._Find_next(pos)) {
        st.add(pos); 
        rec_run_and_count(st, max_level, level+1);
        st.pop();
    }
}



void rec_run_and_save(GlobalState& st, 
                        std::vector<GlobalState>& states_holder,  
                        int max_level=7, 
                        int level=1) {
    
    if (level >= max_level) {
        states_holder.push_back(st);
        return;
    }
    
    // [0, 1, 0, 0, ... ] = bank
    // [1, 0, 1, 1, ... ] = ~bank
    // [1, 0, 0, 1, ... ] = MAP
    // [1, 0, 0, 1, ... ] = MAP & ~bank : avaible nodes

    auto next_moves = ~st.bank & MAP_VALUE_NUMBER.at(st.back());
    size_t pos = next_moves._Find_first();
    
    while (pos - 28 != 0)  {
        st.add(pos); 
            rec_run_and_save(st, states_holder,  max_level, level+1);
        st.pop();
        pos = next_moves._Find_next(pos);
    }
}


std::vector<GlobalState> create_first_states(int domino, int last_number, int depth=2) {
	GlobalState st(domino, last_number);
	std::vector<GlobalState> res;
    rec_run_and_save(st, res, depth);
    return res;
}

void print_stat(const std::vector<unsigned long long>& count, int level, int max_depth=28) {
    for (int i = level; i < max_depth; ++i) {
        std::cout << i+1 << " : " << count[i] << "\n";
    }
    std::cout << "=====================================\n";
}


int main(int argc, char** argv) {
    srand(time(0));
	if (argc == 1) return 1;
	

    int rand_number_dom = rand() % 28;
	int max_depth = std::atoi(argv[1]);
	
	if (max_depth == 1) return 1;

    int last_number = MAP_NUMBER_VALUES.at(rand_number_dom).second;
    
    std::cout << "Start number domino : " << rand_number_dom <<  "\n";
    std::cout << "Max Depth :" << max_depth << "\n";

    int level_current = std::atoi(argv[2]); // 2 level == 6 threads, 3 level = 35(or36) threads 

	std::vector<GlobalState> states = create_first_states(rand_number_dom, last_number, level_current);
    std::cout << "Num of States : " << states.size() << "\n";

    // for (auto& state: states) {
    //     std::cout << state.bank << "\n";
    // }

	std::vector<std::thread> thread_holder;

    Timer timer;
    timer.start();

	for (GlobalState& state : states) {
		thread_holder.push_back(std::thread(std::ref(rec_run_and_count), std::ref(state), std::ref(max_depth), level_current));
	}

    std::vector<unsigned long long> all_count(28, 0);


    int i = 0;
	for (auto& thr : thread_holder) {
		thr.join();
        for (int j = level_current; j <= max_depth; ++j) {
            all_count[j-1] += states[i].get_count(j);
        }
        ++i;
    }

    timer.finish();

    std::cout <<  "Time : " << timer() << "ms " << "\n";
    print_stat(all_count, level_current);
    return 0;
}
