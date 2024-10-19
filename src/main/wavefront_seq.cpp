//
// Created by Fabrizio De Castelli on 22/08/24.
//

#include <chrono>
#include <iostream>
#include "wavefront.hpp"

int main(const int argc, char** argv) {

    if (argc != 2){
        std::cout << "Usage: " << argv[0] << " [matrix size]";
        return EXIT_FAILURE;
    }

    const size_t n = std::stoul(argv[1]);
	WavefrontMatrix matrix(n);
    matrix.initialize_diagonal();

    const auto start = std::chrono::high_resolution_clock::now();
    matrix.wavefront_computation();
    const auto end = std::chrono::high_resolution_clock::now();

    std::cout << n << ";" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
	std::cout << "A(0, n-1) = " << matrix.top_right() << std::endl;

    if (n <= 10)
    {
        // print only if the matrix is small
        matrix.print();
    }


    return 0;
}
