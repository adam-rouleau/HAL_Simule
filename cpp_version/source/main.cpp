#include <iostream>
#include <string>

#include "VCU.hpp"

int main() {
    std::string test_mode_input;
    std::cout << "Enable Test Mode ? [y/n]: " << std::flush;
    std::cin >> test_mode_input;

    bool test_mode = true;
    int nb_iterations = 706;

    if (test_mode_input == "y") {
        test_mode = true;
        std::cout << "How many iterations? [1, 707]: " << std::flush;
        std::cin >> nb_iterations;
    }
    else if (test_mode_input == "n") {
        test_mode = false;
    } 
    else {
        std::cout << "Invalid input." << std::endl;
        return 1;
    }

    VCU main_vcu;
    for (int i = 0; i < nb_iterations; ++i) {
        main_vcu.update(test_mode);
    }

    return 0;
}