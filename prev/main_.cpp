#include "domino.h"

#include <bitset>
#include <iostream>
#include <stdexcept>
#include <string>


#ifdef BOOST
#include <boost/multiprecision/gmp.hpp>
#endif

#include <thread>

// g++ main.cpp -O2 -lgmp -pthread -DBOOST


std::vector<std::string> global_store;

int choose_pos(std::pair<int, int> dom, int last_number) {
    if (dom.first == last_number)  return dom.second;
    else return dom.first;
}

struct GlobalState {
    std::vector<int> dominoes; // max size == 28
    std::vector<int> last_numbers; // max size == 28
    std::bitset<28> bank;
    
    int avaible = 0;
    
    #ifdef BOOST
        boost::multiprecision::mpz_int count_ = 0;
    #endif

    #ifndef BOOST
        long long unsigned count_ = 0;
    #endif

    GlobalState() : dominoes({}), last_numbers({}) {};

    GlobalState(int domino, int last_number) : dominoes(std::vector<int>(28, -1)), 
                                               last_numbers(std::vector<int>(28, -1)) {
        
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


    void count() {
        count_++;
    }

    #ifdef BOOST
        boost::multiprecision::mpz_int get_count() const {
            return count_;
        }
    #endif

    #ifndef BOOST
        int get_count() const {
            return count_;
        }
    #endif    

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
    if (level >= max_level) {
        st.count();
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


int main(int argc, char** argv) {
    srand(time(0));

	if (argc == 1) return 1;
	

    int rand_number_dom = rand() % 28;
	int depth = std::atoi(argv[1]);
	
	if (depth == 1) return 1;

    int last_number = MAP_NUMBER_VALUES.at(rand_number_dom).second;
    
    std::cout << "START num " << rand_number_dom <<  "\n";
    std::cout << "Depth :" << depth << "\n";

    int level_current = std::atoi(argv[2]); // 2 level == 6 threads, 3 level = 35(or36) threads 

	std::vector<GlobalState> states = create_first_states(rand_number_dom, last_number, level_current);
    std::cout << states.size() << "\n";
	std::vector<std::thread> thread_holder;

	for (GlobalState& state : states) {
		thread_holder.push_back(std::thread(std::ref(rec_run_and_count), std::ref(state), std::ref(depth), level_current));
	}

    #ifdef BOOST
        boost::multiprecision::mpz_int all_count {0};
    #endif

    #ifndef BOOST
        unsigned long long all_count {0};
    #endif

    int i = 0;
	for (auto& thr : thread_holder) {
		thr.join();
		std::cout << "s" << i << ": " << states[i].get_count()  << "\n";
        all_count += states[i].get_count();
        ++i;
    }
    std::cout << "TOTAL : " << all_count << "\n";
    return 0;
}
