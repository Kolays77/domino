 #include "domino.h"

#include <bitset>
#include <iostream>
#include <stdexcept>
#include <string>
#include <array>

#include <thread>


// g++ main.cpp -O2 -pthread --std=c++20


std::vector<std::string> global_store;
using namespace std::chrono_literals;

int choose_pos(std::pair<int, int> dom, int last_number) {
    if (dom.first == last_number)  return dom.second;
    else return dom.first;
}

struct GlobalState {
    std::vector<int> dominoes; // max size == 28
    std::vector<int> last_numbers; // max size == 28
    std::bitset<28> bank;
    std::vector<long long unsigned> count_;

    int avaible = 0;
    
    GlobalState() : dominoes({}), last_numbers({}) {};

    GlobalState(int domino, int last_number) : dominoes(std::vector<int>(28, -1)), 
                                               last_numbers(std::vector<int>(28, -1)),
                                               count_(std::vector<long long unsigned>(28, 0)) {
        
        if (MAP_NUMBER_VALUES.at(domino).first != last_number && 
        MAP_NUMBER_VALUES.at(domino).second != last_number) {
            throw std::invalid_argument("last_number != domino.first OR .second");
        }
        
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
        if (avaible) {
            bank[dominoes[avaible-1]] = 0;
            --avaible;
        }
    }
};



void rec_run_and_count(GlobalState& st,  int max_level=7, int level=1) {
    st.count(level);

    if (level >= max_level) {
        return;
    }

    for (int next: MAP_VALUE_NUMBER.at(st.back())) {
        if (!st.bank[next])  {
            st.add(next);
            rec_run_and_count(st, max_level, level+1);
            st.pop();
        } 
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

    for (int next: MAP_VALUE_NUMBER.at(st.back())) {
        if (!st.bank[next])  {
            st.add(next);
            rec_run_and_save(st, states_holder,  max_level, level+1);
            st.pop();
        } 
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



bool some_threads_works(const std::vector<std::thread>& thread_holder) {
    for (auto& t : thread_holder) {
        if (t.joinable()) {
            return true;
        }
    }
    return false;
}


void log_function(const std::vector<std::thread>& thread_holder, std::vector<GlobalState>& states, int level_current, int max_depth) {
    int size = states.size();
    while (some_threads_works(thread_holder)) {
        std::vector<unsigned long long> curr_count(28, 0);
        for (int i = 0; i < size; ++i) {
            for (int j = level_current; j <= max_depth; ++j) {
                curr_count[j-1] += states[i].get_count(j);
            }    
        }

        print_stat(curr_count, level_current);
        std::this_thread::sleep_for(300ms);
    }
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
    std::cout << states.size() << "\n";
	std::vector<std::thread> thread_holder;
    
	for (GlobalState& state : states) {
		thread_holder.push_back(std::thread(std::ref(rec_run_and_count), std::ref(state), std::ref(max_depth), level_current));
	}

	std::jthread thread_logger(log_function, std::ref(thread_holder), std::ref(states), level_current, max_depth);
    

    std::vector<unsigned long long> total_count(28, 0);

    int i = 0;

    for (auto& thr : thread_holder) {
        thr.join();         
        for (int j = level_current; j <= max_depth; ++j) {
            total_count[j-1] += states[i].get_count(j);
        }
        ++i;
    }


    std::cout << "TOTAL SUM : \n";
    print_stat(total_count, level_current);
    return 0;
}
