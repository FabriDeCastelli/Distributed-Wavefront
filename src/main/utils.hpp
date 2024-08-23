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



inline MatrixElementType dotProduct(const std::vector<MatrixElementType>& v1, const std::vector<MatrixElementType>& v2) {
    MatrixElementType result = 0.0;
    for (size_t i = 0; i < v1.size(); ++i) {
        result += v1[i] * v2[i];
    }
    return result;
}

inline void initialize_diagonal(MatrixElementType* matrix, const size_t n) {
    for (size_t i = 0; i < n; ++i) {
        matrix[i * n + i] = static_cast<MatrixElementType> (i + 1) / static_cast<MatrixElementType> (n);
    }
}

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

inline void wavefrontComputation(MatrixElementType* matrix, const size_t n) {
    initialize_diagonal(matrix, n);

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
