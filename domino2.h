#pragma once
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
#include <cassert>

static constexpr size_t N = 56;

std::bitset<N> make_bitset(std::vector<int>&& v) {
    std::bitset<N> b;
    for (int vv : v) b[vv] = true;
    return b;
}

const auto twins_doms_not = ~make_bitset({2, 16, 28, 38, 46, 52, 56});

using domino_t = int;

const std::bitset<N> MAP_VALUE_NUMBER[7] = {make_bitset({0, 1, 2, 4, 6, 8, 10, 12}),
                                            make_bitset({3, 14, 15, 16, 18, 20, 22, 24}),
                                            make_bitset({5, 17, 26, 27, 28, 30, 32, 34}),
                                            make_bitset({7, 19, 29, 36, 37, 38, 40, 42}),
                                            make_bitset({9, 21, 31, 39, 44, 45, 46, 48}),
                                            make_bitset({11, 23, 33, 41, 47, 50, 51, 52}),
                                            make_bitset({13, 25, 35, 43, 49, 53, 54, 55})};

const  domino_t MAP_NUMBER_VALUES[N][2] ={ {0, 0}, {0, 0}, {0, 1}, {1, 0}, {0, 2}, {2, 0}, {0, 3}, {3, 0}, 
                                                {0, 4}, {4, 0}, {0, 5}, {5, 0}, {0, 6}, {6, 0}, {1, 1}, {1, 1}, 
                                                {1, 2}, {2, 1}, {1, 3}, {3, 1}, {1, 4}, {4, 1}, {1, 5}, {5, 1},
                                                {1, 6}, {6, 1}, {2, 2}, {2, 2}, {2, 3}, {3, 2}, {2, 4}, {4, 2},
                                                {2, 5}, {5, 2}, {2, 6}, {6, 2}, {3, 3}, {3, 3}, {3, 4}, {4, 3},
                                                {3, 5}, {5, 3}, {3, 6}, {6, 3}, {4, 4}, {4, 4}, {4, 5}, {5, 4},
                                                {4, 6}, {6, 4}, {5, 5}, {5, 5}, {5, 6}, {6, 5}, {6, 6}, {6, 6}};

// В завимости от четности убрать элемент

// if n == 2k+1 -> n + 1
// if n == 2k   -> n - 1

// n + 2 * (n & 1) - 1


// we have [3, 4]
// [4, 1], [4, 4],     [4, 4]

inline int neighbour(int n) {
    return n - 2 * (n & 1) +  1;
}




struct GlobalState {
    std::array<domino_t, N> dominoes;
    std::bitset<N> bank;
    size_t curr_pos = 0;
    std::array<uint64_t, N> count_;

    GlobalState(domino_t domino, domino_t last_number) {
        std::fill(count_.begin(), count_.end(), 0);
        dominoes[curr_pos] = domino;
        bank[domino] = 1;
        bank[neighbour(domino)] = 1;

        ++curr_pos;
    }

    domino_t back() {
        return dominoes[curr_pos-1];
    }

    void count(size_t level) {
        count_[level-1]++;
    }


    uint64_t get_count(size_t level) {
        return count_[level-1];
    }

    void add(domino_t domino_number) {
        bank[domino_number] = 1;
        bank[neighbour(domino_number)] = 1;

        dominoes[curr_pos] = domino_number;
        ++curr_pos;
    }


    void pop() {
        bank[dominoes[curr_pos-1]] = 0;
        bank[neighbour(dominoes[curr_pos-1])] = 0;
        --curr_pos;
    }

    void print() {
        for (size_t i = 0; i < curr_pos - 1; ++i) {
            std::cout << dominoes[i] << "-";
        }
        std::cout << dominoes[curr_pos-1] << '\n';
    }
};


void rec_run_and_count(GlobalState& st, const int max_level) {
    st.count(st.curr_pos);
    if (st.curr_pos == max_level) return;

    // [0, 1, 0, 0, ... ] = bank
    // [1, 0, 1, 1, ... ] = ~bank

    // [1, 0, 0, 1, ... ] = MAP
    // [1, 0, 1, 0, ... ] = ~twins_doms

    // [1, 0, 0, 1, ... ] = (MAP & ~twins) & ~bank : curr_pos nodes

    auto next_moves = ~st.bank & (MAP_VALUE_NUMBER[st.back()] & twins_doms_not) ;
    
    for (size_t pos = next_moves._Find_first(); pos < N; pos = next_moves._Find_next(pos)) {
        st.add(pos);
            rec_run_and_count(st, max_level);
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
    // [1, 0, 0, 1, ... ] = MAP & ~bank : curr_pos nodes

    auto next_moves = ~st.bank & MAP_VALUE_NUMBER[st.back()] & twins_doms_not;
    // std::cout << "N : " << next_moves << "\n"; 
    for (size_t pos = next_moves._Find_first(); pos < N; pos = next_moves._Find_next(pos)) {
        st.add(pos);
            rec_run_and_save(st, states_holder, max_level, level+1);
        st.pop();
    }
}

std::vector<GlobalState> create_first_states(int domino, int depth=2) {
	GlobalState st(domino, MAP_NUMBER_VALUES[domino][1]);
	std::vector<GlobalState> res;
    rec_run_and_save(st, res, depth);
    return res;
}


template<typename T>
void print_stat(const std::vector<T>& count, int level, int max_depth=28) {
    for (int i = level; i < max_depth; ++i) {
        std::cout << i+1 << " : " << count[i] << "\n";
    }
    std::cout << "=====================================\n";
}
