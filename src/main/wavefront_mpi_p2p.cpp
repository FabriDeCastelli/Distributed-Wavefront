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
    const double start_time = MPI_Wtime();

    if (rank == 0) // master
    {
        std::vector<MPI_Request> send_requests(n * (n - 1) / 2);
        std::vector<MPI_Request> recv_requests(n * (n - 1) / 2);
        int send_count = 0;
        int recv_count = 0;
        int destination_process = 0;
        for (size_t k = 1; k < n; ++k)
        {
            for (size_t m = 0; m < n - k; ++m)
            {
                std::vector block(2 * k, 0.0);
                for (size_t i = 0; i < k; ++i) {
                    block[i] = matrix[m * n + (m + i)];
                    block[k + i] = matrix[(m + i + 1) * n + (m + k)];
                }

                MPI_Isend(
                    block.data(),
                    2 * k,
                    MPI_DOUBLE,
                    destination_process + 1,
                    0,
                    MPI_COMM_WORLD,
                    &send_requests[send_count++]
                );

                MPI_Irecv(
                    &matrix[m * n + (m + k)],
                    1,
                    MPI_DOUBLE,
                    destination_process + 1,
                    0,
                    MPI_COMM_WORLD,
                    &recv_requests[recv_count++]
                );

                destination_process = (destination_process + 1) % (num_processes - 1);

            }
        }

        MPI_Waitall(recv_count, recv_requests.data(), MPI_STATUSES_IGNORE);

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

    } else { // worker

        MPI_Status status;
        while (true)
        {

            MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == TERMINATION_TAG)
                break;

            int k;
            MPI_Get_count(&status, MPI_DOUBLE, &k);

            std::vector<MatrixElementType> block(k);
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
                result += block[i] * block[k / 2 + i];

            result = std::cbrt(result);

            MPI_Request request;
            MPI_Isend(
                &result,
                1,
                MPI_DOUBLE,
                0,
                0,
                MPI_COMM_WORLD,
                &request
            );

        }
    }

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

    std::string filename = "mpi_p2p_" + std::to_string(n) + ".txt";
    std::ofstream mpi_p2p_file(Config::OUTPUTS_DIRECTORY / filename);
    print_matrix(matrix.data(), n, mpi_p2p_file);

    MPI_Finalize();
    return 0;
}