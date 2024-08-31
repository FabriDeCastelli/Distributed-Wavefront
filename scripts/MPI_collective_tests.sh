#!/usr/bin/env bash

# This script is used to run the tests for the ff program.
OUTPUT_DIR="../outputs/MPI"
OUTPUT="$OUTPUT_DIR/collective.csv"
# mkdir -p $OUTPUT_DIR
echo "nodes;n;p;time" > "$OUTPUT"
chmod +w "$OUTPUT"

MATRIX_N=(200 400 800 1600 3200)
NUM_NODES=(1 2 4 8)
NUM_PROCESSES=(2 4 8 12 16 20)

# Run the tests
cd ..
cmake -S . -B build
cd build || exit
make

# shellcheck disable=SC2068

for nodes in ${NUM_NODES[@]}; do
    for n in ${MATRIX_N[@]}; do
        for np in ${NUM_PROCESSES[@]}; do
            if [ "$nodes" -gt "$np" ]; then
                continue
            fi
            echo "Running test for matrix size $n with $np processes and $nodes nodes"
            for (( i=1; i <= 5; ++i )); do
                output=$(srun --mpi=pmix --nodes="$nodes" -n "$np" ./MPI_Collective "$n")
                echo "$nodes;$output" >> "$OUTPUT"
            done
        done
    done
done


echo "MPI test finished"
