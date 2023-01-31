#include "domino2.h"
#include "timer.h"

// compile:
// g++ generate2.cpp -Ofast -std=c++20  -funroll-loops

constexpr int N_level = 28;

int main(int argc, char** argv) {
    srand(time(0));
	if (argc == 1) return 1;
    int rand_number_dom = rand() % N;
    
    int max_level = std::atoi(argv[1]);
	if (max_level < 2 && max_level > N_level) return 1;

    std::cout << "Max Depth :" << max_level << "\n";
    
    if (argc == 2) {
        std::cout << "Add param level_current: 2lvl = 6threads, 3lvl = 35/36 threads ...\n";
        return 1;
    }

    int level_current = std::atoi(argv[2]); // 2 level == 6 threads, 3 level = 35(or36) threads

	std::vector<GlobalState> states = create_first_states(rand_number_dom, level_current);
    std::cout << states.size() << "\n";

    std::cout << "Num of States (threads): " << states.size() << "\n";


	std::vector<std::thread> thread_holder;

    Timer timer;
    timer.start();

	for (GlobalState& state : states) {
		thread_holder.push_back(std::thread(std::ref(rec_run_and_count), std::ref(state), max_level));
	}

    std::vector<uint64_t> all_count(N_level, 0);

    int i = 0;
	for (auto& thr : thread_holder) {
		thr.join();
        for (int j = level_current; j <= max_level; ++j) {
            all_count[j-1] += states[i].get_count(j);
        }
        ++i;
    }

    timer.finish();

    std::cout <<  "Time : " << timer() << "ms " << "\n";
    print_stat(all_count, level_current, max_level);
    return 0;
}
