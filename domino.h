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

static constexpr size_t N = 28;
using domino_t = int;

std::bitset<N> make_bitset(std::vector<int>&& v) {
    std::bitset<N> b;
    for (int vv : v) b[vv] = true;
    return b;
}

const std::bitset<N> MAP_VALUE_NUMBER[7] = {make_bitset({0, 1, 2, 3, 4, 5, 6}),
                                            make_bitset({1, 7, 8, 9, 10, 11, 12 }),
                                            make_bitset({2, 8, 13, 14, 15, 16, 17 }),
                                            make_bitset({3, 9, 14, 18, 19, 20, 21 }),
                                            make_bitset({4, 10, 15, 19, 22, 23, 24 }),
                                            make_bitset({5, 11, 16, 20, 23, 25, 26 }),
                                            make_bitset({6, 12, 17, 21, 24, 26, 27 })};

const std::pair<domino_t, domino_t> MAP_NUMBER_VALUES[N] = {{0, 0},{0, 1},{0, 2},{0, 3},
                                                            {0, 4},{0, 5},{0, 6},{1, 1},
                                                            {1, 2},{1, 3},{1, 4},{1, 5},
                                                            {1, 6},{2, 2},{2, 3},{2, 4},
                                                            {2, 5},{2, 6},{3, 3},{3, 4},
                                                            {3, 5},{3, 6},{4, 4},{4, 5},
                                                            {4, 6},{5, 5},{5, 6},{6, 6}};



inline domino_t choose_pos(const std::pair<domino_t, domino_t>& dom, domino_t last_number) {
    return (dom.first == last_number) ? dom.second : dom.first;
}

struct GlobalState {
    size_t curr_pos = 0;
    std::bitset<N> bank;
    std::array<domino_t, N> dominoes;
    std::array<domino_t, N> last_numbers;
    std::array<uint64_t, N> count_;
    
    // GlobalState() : dominoes({}), last_numbers({}) {};

    GlobalState(domino_t domino, domino_t last_number)  {
        std::fill(count_.begin(), count_.end(), 0);
        dominoes[curr_pos] = domino;
        last_numbers[curr_pos] = last_number;
        bank[domino] = 1;
        ++curr_pos;
    }

    domino_t back() {
        return last_numbers[curr_pos-1];
    }

    void count(size_t level) {
        count_[level-1]++;
    }


    uint64_t get_count(size_t level) {
        return count_[level-1];
    }

    void add(domino_t domino_number) {
        bank[domino_number] = 1;
        dominoes[curr_pos] = domino_number;
        last_numbers[curr_pos] = choose_pos(MAP_NUMBER_VALUES[domino_number], back());
        ++curr_pos;
    }



    void pop() {
        bank[dominoes[curr_pos-1]] = 0;
        --curr_pos;
    }

    void print() {
        for (size_t i = 0; i < curr_pos - 1; ++i) {
            std::cout << dominoes[i] << "-";
        }
        std::cout << dominoes[curr_pos-1] << '\n';
    }

};


size_t find_first(const std::bitset<N>& bitset) {
    for (size_t i = 0; i < N; ++i) {
	if (bitset[i]) return i;
    }
    return N;
}


size_t find_next(const std::bitset<N>& bitset, size_t start_pos) {
   for (size_t i = start_pos+1; i < N; ++i) {
   	if (bitset[i]) return i;
   } return N;

}

void rec_run_and_count(GlobalState& st, const int max_level) {
    st.count(st.curr_pos);
    if (st.curr_pos == max_level) return;

    // [0, 1, 0, 0, ... ] = bank
    // [1, 0, 1, 1, ... ] = ~bank
    // [1, 0, 0, 1, ... ] = MAP
    // [1, 0, 0, 1, ... ] = MAP & ~bank : curr_pos nodes

    auto next_moves = ~st.bank & MAP_VALUE_NUMBER[st.back()];
    for (size_t pos = find_first(next_moves); pos < 28; pos = find_next(next_moves, pos)) {
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

    auto next_moves = ~st.bank & MAP_VALUE_NUMBER[st.back()];
    for (size_t pos = find_first(next_moves); pos < 28; pos = find_next(next_moves, pos)) {
        st.add(pos);
        rec_run_and_save(st, states_holder,  max_level, level+1); 
        st.pop();
    }
}


std::vector<GlobalState> create_first_states(domino_t domino, domino_t last_number, int depth=2) {
	GlobalState st(domino, last_number);
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
