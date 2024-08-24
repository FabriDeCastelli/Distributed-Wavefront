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
#include <unordered_map>

#include "utils.hpp"


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
    const double start_time = MPI_Wtime();

    for (int k = 1; k < n - 1; ++k)
    {
        std::vector results(n - k, 0.0);

        const int elements_per_process = (n - k) / num_processes;
        const int remainder = (n - k) % num_processes;

        const int local_size = elements_per_process + (rank < remainder ? 1 : 0);

        std::vector recv_counts(num_processes, 0);
        std::vector displacements(num_processes, 0);

        for (int p_rank = 0; p_rank < num_processes; p_rank++)
        {
            recv_counts[p_rank] = elements_per_process + (p_rank < remainder ? 1 : 0);
            displacements[p_rank] = p_rank == 0 ? 0 : displacements[p_rank - 1] + recv_counts[p_rank - 1];
        }

        std::vector partial_results(local_size, 0.0);

        const size_t start = local_size * rank;
        for (size_t m = start; m < start + local_size; ++m)
        {
            for (int i = 0; i < k; ++i)
            {
                partial_results[m - start] += matrix[m * n + m + i] * matrix[(m + 1 + i) * n + m + k];
            }
            partial_results[m - start] = std::cbrt(partial_results[m - start]);
        }

        MPI_Allgatherv(
            partial_results.data(),
            local_size,
            MPI_DOUBLE,
            results.data(),
            recv_counts.data(),
            displacements.data(),
            MPI_DOUBLE,
            MPI_COMM_WORLD
        );

        for (int m = 0; m < n - k; ++m)
        {
            matrix[m * n + m + k] = results[m];
        }

    }

    for (int i = 0; i < n - 1; i++)
        matrix[n - 1] += matrix[i] * matrix[(i + 1) * n + n - 1];

    const double time = (MPI_Wtime() - start_time) * 1000.0;

    std::vector<double> running_times(num_processes);
    MPI_Gather(&time, 1, MPI_DOUBLE, running_times.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        std::cout << n << ";" << num_processes << ";";
        for (int i = 0; i < num_processes - 1; ++i)
            std::cout << running_times[i] << ",";
        std::cout << running_times[num_processes - 1] << std::endl;

    }

    MPI_Finalize();
    return 0;
}