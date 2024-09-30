//
// Created by Fabrizio De Castelli on 22/08/24.
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "config.hpp"



/**
 * A class representing a matrix with the Wavefront computation pattern.
 *
 * @tparam T the type of the matrix elements
 * @tparam dim the dimension of the matrix
 */
class WavefrontMatrix
{
    double* data;  // Pointer to dynamically allocated array
    size_t dim;

public:
    // Constructor: dynamically allocate memory for the matrix
    explicit WavefrontMatrix(const size_t dim) : dim(dim) {
        data = new double[dim * dim];  // Allocate a 1D array to represent the matrix
    }

    // Destructor: free allocated memory
    ~WavefrontMatrix() {
        delete[] data;
    }

    double& operator()(const size_t y, const size_t x) const
    {
        return data[y * dim + x];
    }


    // Get the size (dimension) of the matrix
    [[nodiscard]] size_t size() const {
        return dim;
    }

    void print() const
    {
        const size_t n = size();
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j)
            {
                constexpr int width = 10;
                std::cout << std::setw(width) << std::fixed << std::setprecision(2)
                          << data[i * n + j] << " ";
            }
            std::cout << std::endl;
        }
    }
};


/**
 * Initializes the diagonal of the matrix. The diagonal elements are initialized with the value (i + 1) / n.
 *
 * @param matrix the pointer to the matrix, stored as a contiguous 1d vector of dimension n x n
 */

inline void initialize_diagonal(WavefrontMatrix &matrix) {
    const size_t dim = matrix.size();
    for (size_t i = 0; i < dim; ++i) {
        matrix(i, i) = static_cast<double>(1 + i) / static_cast<double>(dim);
    }
}


/**
 * Computes the dot product of two vectors.
 *
 * @param m the starting index of the first vector
 * @param k the size of the vectors
 * @param matrix the input matrix
 * @return the dot product of the two vectors
 */
__attribute__((always_inline)) inline double dotProduct(const size_t m, const size_t k, WavefrontMatrix& matrix) {
    double res = 0.0;
    for (size_t i = 0; i < k; ++i) {
        res += matrix(m, m + k - i - 1) * matrix(m + k, m + i + 1);
    }
    return res;
}

/**
 * Performs the Wavefront computation pattern sequentially.
 *
 * @param matrix a contiguous n x n pointer, assuming row-major order and initialized with the diagonal values
 */
inline void wavefrontComputation(WavefrontMatrix& matrix) {

    const size_t n = matrix.size();
    for (size_t k = 1; k < n; ++k) {
        for (size_t m = 0; m < n - k; ++m) {
            matrix(m, m + k) = std::cbrt(dotProduct(m, k, matrix));
            matrix(m + k, m) = matrix(m, m + k);
        }
    }
}



#endif //UTILS_HPP




