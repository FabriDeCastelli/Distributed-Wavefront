//
// Created by Fabrizio De Castelli on 22/08/24.
//


#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "../../fastflow/ff/ff.hpp"
#include "../../fastflow/ff/farm.hpp"
#include "wavefront.hpp"

using namespace ff;

struct Task {
    size_t chunk_start;
    size_t chunk_end;
    size_t k;
};


// Emitter (master)
class Emitter final : public ff_node_t<Task> {
public:
    Emitter(WavefrontMatrix& matrix, const int num_workers)
        : matrix(matrix), n(matrix.size()), num_workers(num_workers), tasks(num_workers, Task()),
          spawned(0),
          k(1)
    {}

    Task* svc(Task* task) override {


        if (n - k == 1) {
            matrix(0, n - 1) = std::cbrt(dot_product(0, n - 1, matrix));
            return EOS;
        }

        if (task != nullptr) {
            spawned--;
        }

        if (spawned == 0)
        {

            const size_t elements_per_worker = (n - k) / (num_workers + 1);
            const size_t remainder = (n - k) % (num_workers + 1);
            const size_t necessary_workers = elements_per_worker == 0 ? remainder - 1 : num_workers;

            size_t chunk_start = 0;
            for (int worker_id = 0; worker_id < necessary_workers; ++worker_id) {
                const size_t local_size = elements_per_worker + (worker_id < remainder ? 1 : 0);
                tasks[worker_id].chunk_start = chunk_start;
                tasks[worker_id].chunk_end = chunk_start + local_size;
                tasks[worker_id].k = k;
                ff_send_out(&tasks[worker_id]);
                chunk_start = chunk_start + local_size;
                spawned++;
            }

            for(size_t m = chunk_start; m < n - k; ++m) {
                matrix(m, m + k) = std::cbrt(dot_product(m, k, matrix));
                matrix(m + k, m) = matrix(m, m + k);
            }

            k++;
        }

        return GO_ON;
    }

private:
    WavefrontMatrix& matrix;
    size_t n;
    size_t num_workers;
    std::vector<Task> tasks;
    size_t spawned;
    size_t k;
};


// Worker
class Worker final : public ff_node_t<Task> {

public:
    explicit Worker(WavefrontMatrix& matrix)
        : matrix(matrix)
    {}

    Task* svc(Task* task) override {

        const size_t k = task->k;
        for (size_t m = task->chunk_start; m < task->chunk_end; ++m)
        {
            matrix(m, m + k) = std::cbrt(dot_product(m, k, matrix));
            matrix(m + k, m) = matrix(m, m + k);
        }

        return task;
    }


    private:
        WavefrontMatrix& matrix;

};


int main(const int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " [matrix_size] [num_workers]";
        return EXIT_FAILURE;
    }

    const size_t n = std::stoul(argv[1]);
    const size_t num_workers = std::stoul(argv[2]);

    WavefrontMatrix matrix(n);
    matrix.initialize_diagonal();

    std::vector<std::unique_ptr<ff_node>> workers;
    for (size_t i = 0; i < num_workers; ++i) {
        workers.push_back(std::make_unique<Worker>(matrix));
    }

    ff_Farm<Task> farm(std::move(workers), std::make_unique<Emitter>(matrix, num_workers));
    farm.remove_collector();
    farm.wrap_around();
    farm.set_scheduling_ondemand();

    const auto start = std::chrono::high_resolution_clock::now();
    if (farm.run_and_wait_end() < 0) {
        std::cerr << "Error running farm" << std::endl;
        return -1;
    }
    const auto end = std::chrono::high_resolution_clock::now();
    std::cout << n << ";" << num_workers << ";" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    std::cout << "A(0, n-1) = " << matrix.top_right() << std::endl;

    if (n <= 10)
    {
        // print only if the matrix is small
        matrix.print();
    }


    return 0;
}
