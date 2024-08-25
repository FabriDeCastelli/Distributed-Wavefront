//
// Created by Fabrizio De Castelli on 22/08/24.
//


#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

#include "../main/config.hpp"

// Function to read a matrix from a file
std::vector<std::vector<double>> matrix_from_file(const std::string& filename, int& n) {
    std::ifstream file(Config::OUTPUTS_DIRECTORY / filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    // Read the size of the matrix
    file >> n;

    std::vector matrix(n, std::vector<double>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            file >> matrix[i][j];
        }
    }

    file.close();
    return matrix;
}

// Function to compare two matrices
bool compareMatrices(
    const std::vector<std::vector<double>>& matrix1,
    const std::vector<std::vector<double>>& matrix2,
    const std::vector<std::vector<double>>& matrix3
    ) {
    const int n = matrix1.size();
    if (matrix2.size() != n) return false;

    for (int i = 0; i < n; ++i) {
        if (matrix2[i].size() != n) return false;
        for (int j = 0; j < n; ++j) {
            if (matrix1[i][j] != matrix2[i][j] || matrix1[i][j] != matrix3[i][j])
                return false;
        }
    }
    return true;
}

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " n " << std::endl;
        return 1;
    }

    const int n = std::stoi(argv[1]);

    const std::string seq_filename = "sequential_" + std::to_string(n) + ".txt";
    const std::string mpi_filename = "mpi_collective_" + std::to_string(n) + ".txt";
    const std::string fastflow_filename = "fastflow_" + std::to_string(n) + ".txt";

    try {
        int n1, n2, n3;
        const std::vector<std::vector<double>> sequential = matrix_from_file(seq_filename, n1);
        const std::vector<std::vector<double>> mpi = matrix_from_file(mpi_filename, n2);
        const std::vector<std::vector<double>> fastflow = matrix_from_file(fastflow_filename, n3);

        if (n1 != n2 || n2 != n3 || n1 != n3) {
            std::cerr << "Matrices are of different sizes." << std::endl;
            return 1;
        }

        if (compareMatrices(sequential, mpi, fastflow)) {
            std::cout << "The matrices are equal." << std::endl;
        } else {
            std::cout << "The matrices are not equal." << std::endl;
        }

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
