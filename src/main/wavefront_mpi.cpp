//
// Created by Fabrizio De Castelli on 22/08/24.
//

#include <algorithm>
#include <iostream>
#include <mpi.h>
#include <ostream>
#include <unistd.h>
#include <vector>
#include <cmath>
#include "utils.hpp"

#define TERMINATION_TAG 99


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << "[matrix size]";
        return EXIT_FAILURE;
    }

    const int n = std::stoi(argv[1]);

    if (n < 1)
    {
        std::cerr << "Invalid matrix size" << std::endl;
        return EXIT_FAILURE;
    }

    MPI_Init(&argc, &argv);

    int num_processes, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (num_processes < 2)
    {
        std::cerr << "At least 2 processes are required" << std::endl;
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    std::vector matrix(n*n, 0.0);
    initialize_diagonal(matrix.data(), n);

    MPI_Barrier(MPI_COMM_WORLD);
    const double start = MPI_Wtime();

    if (rank == 0) // master
    {
        int destination_process = 0;
        for (size_t k = 1; k < n; ++k)
        {
            // std::cout << k << std::endl;
            for (size_t m = 0; m < n - k; ++m)
            {

                std::vector block(2 * k, 0.0);
                for (size_t i = 0; i < k; ++i) {
                    block[i] = matrix[m * n + (m + i)];
                    block[k + i] = matrix[(m + i + 1) * n + (m + k)];
                }

                MPI_Request request;
                MPI_Isend(
                    block.data(),
                    2 * k,
                    MPI_DOUBLE,
                    destination_process + 1,
                    0,
                    MPI_COMM_WORLD,
                    &request
                );

                MPI_Recv(
                    &matrix[m * n + (m + k)],
                    1,
                    MPI_DOUBLE,
                    destination_process + 1,
                    0,
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
                );

                // std::cout << destination_process + 1 << std::endl;
                destination_process = (destination_process + 1) % (num_processes - 1);

            }
        }

        const double end = MPI_Wtime();

        for (int k = 1; k < num_processes; ++k)
        {
            MPI_Request request;
            MPI_Isend(
                nullptr,
                0,
                MPI_DOUBLE,
                k,
                TERMINATION_TAG,
                MPI_COMM_WORLD,
                &request
            );
        }

        std::cout << "Time taken for dimension " << n << " and " << num_processes << " processes: " << end - start << "s" << std::endl;
    } else {

        MPI_Status status;
        while (true)
        {

            MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == TERMINATION_TAG)
            {
                std::cout << "Terminating..." << std::endl;
                break;
            }

            int k;
            MPI_Get_count(&status, MPI_DOUBLE, &k);

            // std::cout << "Received block of size " << k << std::endl;

            std::vector<MatrixElementType> block(k);
            MPI_Request request;
            MPI_Recv(
                block.data(),
                k,
                MPI_DOUBLE,
                0,
                MPI_ANY_TAG,
                MPI_COMM_WORLD,
                &status
            );

            MatrixElementType result = 0.0;
            for (size_t i = 0; i < k / 2; ++i)
            {
                result += block[i] * block[k / 2 + i];
            }

            result = std::cbrt(result);

            // std::cout << "Computed result " << result << std::endl;

            MPI_Isend(
                &result,
                1,
                MPI_DOUBLE,
                0,
                0,
                MPI_COMM_WORLD,
                &request
            );

            // std::cout << "Sent result to process 0" << std::endl;
        }
    }

    if (rank == 0)
    {
        std::string filename = "mpi_p2p" + std::to_string(n) + ".txt";
        std::ofstream mpi_file(Config::OUTPUTS_DIRECTORY / filename);
        print_matrix(matrix.data(), n, mpi_file);
    }


    MPI_Finalize();
    return 0;
}