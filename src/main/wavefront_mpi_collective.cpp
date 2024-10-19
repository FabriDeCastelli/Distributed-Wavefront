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

#include "wavefront.hpp"


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

    WavefrontMatrix matrix(n);
    matrix.initialize_diagonal();

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

        for (int p_rank = 0; p_rank < num_processes; ++p_rank)
        {
            recv_counts[p_rank] = elements_per_process + (p_rank < remainder ? 1 : 0);
            displacements[p_rank] = p_rank == 0 ? 0 : displacements[p_rank - 1] + recv_counts[p_rank - 1];
        }

        std::vector partial_results(local_size, 0.0);

        const size_t start = displacements[rank];
        for (size_t m = start; m < start + local_size; ++m)
        {
            partial_results[m - start] = std::cbrt(dot_product(m, k, matrix));
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
            matrix(m, m + k) = results[m];
            matrix(m + k, m) = matrix(m, m + k);
        }

    }

    matrix(0, n - 1) = std::cbrt(dot_product(0, n - 1, matrix));

    const double time = (MPI_Wtime() - start_time) * 1000.0;

    std::vector<double> running_times(num_processes);
    MPI_Gather(&time, 1, MPI_DOUBLE, running_times.data(), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        std::cout << n << ";" << num_processes << ";";
        for (int i = 0; i < num_processes - 1; ++i)
            std::cout << running_times[i] << ",";
        std::cout << running_times[num_processes - 1] << std::endl;
        std::cout << "A(0, n-1) = " << matrix.top_right() << std::endl;

        if (n <= 10)
        {
            // print only if the matrix is small
            matrix.print();
        }

    }

    MPI_Finalize();
    return 0;
}