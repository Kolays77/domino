#include "domino2.h"

int main() {
    auto res = create_first_states(0, 3);
    std::cout << res.size() << "\n";
    for (auto& v : res) {
        std::cout << v.bank << "\n";
        v.print(); 
    }
    return 0;
}