#include <bitset>
#include <iostream>


int main() {
    const int N = 10;
    std::bitset<N> b("0110001100");


    size_t pos = b._Find_first();
    std::cout << "first " << pos << "\n";

    pos = b._Find_next(pos);
    std::cout << pos << "\n";  

    pos = b._Find_next(pos);
    std::cout << pos << "\n";  
    
    pos = b._Find_next(pos);
    std::cout << pos << "\n"; 

    // after allways get N = 10 
    
    
    return 0;
}