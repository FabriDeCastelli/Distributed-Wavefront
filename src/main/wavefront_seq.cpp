//
// Created by Fabrizio De Castelli on 22/08/24.
//

#include <chrono>
#include <iostream>
#include "utils.hpp"

int main(const int argc, char** argv) {

    if (argc != 2){
        std::cout << "Usage: " << argv[0] << " [matrix size]";
        return EXIT_FAILURE;
    }

    const size_t n = std::stoul(argv[1]);
	WavefrontMatrix matrix(n);
    initialize_diagonal(matrix);

    const auto start = std::chrono::high_resolution_clock::now();
    wavefrontComputation(matrix);
    const auto end = std::chrono::high_resolution_clock::now();

    std::cout << n << ";" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
	std::cout << "A(0, n-1) = " << matrix(0, n - 1) << std::endl;

    matrix.print();


    return 0;
}
