//
// Created by Fabrizio De Castelli on 22/08/24.
//


#include <iostream>
#include <vector>
#include <mutex>
#include <chrono>
#include <random>
#include "../../fastflow/ff/ff.hpp"
#include "../../fastflow/ff/farm.hpp"
#include "utils.hpp"

using namespace ff;

struct Task {
    MatrixElementType* matrix;
    int chunk_start;
    int chunk_end;
    int n;
    int k;
};


// Emitter (master)
class Emitter final : public ff_node_t<Task> {
public:
    Emitter(MatrixElementType* matrix, const int n, const int num_workers)
        : matrix(matrix), n(n), num_workers(num_workers), spawned(0), feedback_counter(0), k(1) {}

    Task* svc(Task* task) override {

        if (k == n) {
            return EOS;
        }

        if (task != nullptr) {
            spawned--;
        }

        if (spawned == 0)
        {
            const int elements_per_worker = (n - k) / num_workers;
            const int remainder = (n - k) % num_workers;

            const int necessary_workers = elements_per_worker == 0 ? remainder : num_workers;
            int block_begin = 0;
            for (int worker_id = 0; worker_id < necessary_workers; ++worker_id) {
                const int local_size = elements_per_worker + (worker_id < remainder ? 1 : 0);

                // if (worker_id == 1 && elements_per_worker == 1) {
                //     std::cout << "Start: " << block_begin << " End: " << block_begin + local_size << std::endl;
                // }

                ff_send_out(new Task{matrix, block_begin, block_begin + local_size, n, k});
                block_begin = block_begin + local_size;
                spawned++;
            }
            // std::cout << "Wavefront iteration " << k << " spawned " << spawned << " tasks" << std::endl;
            k++;

        }

        return GO_ON;
    }


private:
    MatrixElementType* matrix;
    int n;
    int num_workers;
    int spawned;
    int feedback_counter;
    int k;
};

// Worker
class Worker final : public ff_node_t<Task> {
public:
    Task* svc(Task* task) override {
        const int start = task->chunk_start;
        const int end = task->chunk_end;
        const int n = task->n;
        const int k = task->k;
        for (size_t m = start; m < end; ++m)
        {
            double res = 0.0;
            for (int i = 0; i < k; ++i)
            {
                res += task->matrix[m * n + m + i] * task->matrix[(m + 1 + i) * n + m + k];
            }
            task->matrix[m * n + m + k] = std::cbrt(res);
        }
        return task;
    }
};


int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " [matrix_size] [num_workers]";
        return EXIT_FAILURE;
    }

    const size_t n = std::stoul(argv[1]);
    const size_t num_workers = std::stoul(argv[2]);

    std::vector matrix(n * n, 0.0);
    initialize_diagonal(matrix.data(), n);

    std::vector<std::unique_ptr<ff_node>> workers;
    for (size_t i = 0; i < num_workers; ++i) {
        workers.push_back(std::make_unique<Worker>());
    }

    ff_Farm<Task> farm(std::move(workers), std::make_unique<Emitter>(matrix.data(), n, num_workers));
    farm.remove_collector();
    farm.wrap_around();

    auto start = std::chrono::high_resolution_clock::now();
    if (farm.run_and_wait_end() < 0) {
        std::cerr << "Error running farm" << std::endl;
        return -1;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << n << ";" << num_workers << ";" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

    // std::cout << "Matrix computation completed successfully." << std::endl;

    // std::string filename = "fastflow_" + std::to_string(n) + ".txt";
    // std::ofstream fastflow_file(Config::OUTPUTS_DIRECTORY / filename);
    // print_matrix(matrix.data(), n, fastflow_file);

    return 0;
}
