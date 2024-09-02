//
// Created by Fabrizio De Castelli on 22/08/24.
//

#include <chrono>
#include <iostream>
#include <vector>
#include "utils.hpp"

int main(int argc, char** argv) {

    if (argc != 2){
        std::cout << "Usage: " << argv[0] << " [matrix size]";
        return EXIT_FAILURE;
    }

    const size_t n = std::stoul(argv[1]);
    std::vector matrix(n*n, 0.0);
    initialize_diagonal(matrix.data(), n);

    const auto start = std::chrono::high_resolution_clock::now();
    wavefrontComputation(matrix.data(), n);
    const auto end = std::chrono::high_resolution_clock::now();
    std::cout << n << ";" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

    // std::string filename = "sequential_" + std::to_string(n) + ".txt";
    // std::ofstream sequential_file(Config::OUTPUTS_DIRECTORY / filename);
    // print_matrix(matrix.data(), n, sequential_file);

    return 0;
}
