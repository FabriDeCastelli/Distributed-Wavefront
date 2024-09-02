//
// Created by Fabrizio De Castelli on 22/08/24.
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cmath>
#include "config.hpp"


// Use double as the type for matrix elements
using MatrixElementType = double;


/**
 * Initializes the diagonal of the matrix. The diagonal elements are initialized with the value (i + 1) / n.
 *
 * @param matrix the pointer to the matrix, stored as a contiguous 1d vector of dimension n x n
 * @param n the size of the matrix
 */
inline void initialize_diagonal(MatrixElementType* matrix, const size_t n) {
    for (size_t i = 0; i < n; ++i) {
        matrix[i * n + i] = static_cast<MatrixElementType> (i + 1) / static_cast<MatrixElementType> (n);
    }
}

/**
 * Prints the matrix to an output stream.
 *
 * @param matrix the input matrix pointer
 * @param n the size of the matrix
 * @param file the output file stream
 */
inline void print_matrix(const MatrixElementType* matrix, const size_t n, std::ofstream &file) {

    if (!file.is_open()) {
        std::cerr << "File is not open!" << std::endl;
        return;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j)
        {
            constexpr int width = 10;
            file << std::setw(width) << std::fixed << std::setprecision(2)
                      << matrix[i * n + j] << " ";
        }
        file << std::endl;
    }
}

/**
 * Performs the Wavefront computation pattern sequentially.
 *
 * @param matrix a contiguous n x n pointer, assuming row-major order and initialized with the diagonal values
 * @param n the size of the matrix
 */
inline void wavefrontComputation(MatrixElementType* matrix, const size_t n) {

    for (size_t k = 1; k < n; ++k) {
        for (size_t m = 0; m < n - k; ++m) {
            double res = 0.0;
            for (int i = 0; i < k; i++) {
                res += matrix[m * n + m + i] * matrix[(m + 1 + i) * n + m + k];
            }
            matrix[m * n + m + k] = std::cbrt(res);
        }
    }
}

#endif //UTILS_HPP
