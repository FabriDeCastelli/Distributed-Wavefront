#!/usr/bin/env bash

# This script is used to run the tests for the ff program.
OUTPUT_DIR="./outputs/MPI"
OUTPUT_FILE="$OUTPUT_DIR/collective_execution_times.txt"
# mkdir -p $OUTPUT_DIR
echo "nodes;n;p;time" > "$OUTPUT_FILE"
chmod +w "$OUTPUT_FILE"

MATRIX_N=(10 100 500 1000 5000 10000)
NUM_PROCESSES=(2 4 8 12 16 20 24)
NUM_NODES=(1 2 4 8)

# Run the tests
cmake .
make

# shellcheck disable=SC2068
for n in ${MATRIX_N[@]}; do
    for p in ${NUM_PROCESSES[@]}; do
        for nodes in ${NUM_NODES[@]}; do
            if [ "$nodes" -gt "$p" ]; then
                continue
            fi
            echo -n "$nodes;" >> "$OUTPUT_FILE"
            echo "Running test for matrix size $n with $p processes and $nodes nodes"
            srun --mpi=pmix --nodes="$nodes" -n "$p" ./MPI_Collective "$n" >> $OUTPUT_FILE 2>&1
        done
    done
done

cat "$OUTPUT_FILE"