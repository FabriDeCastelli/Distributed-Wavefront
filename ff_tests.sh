#!/usr/bin/env bash

# This script is used to run the tests for the ff program.
OUTPUT_DIR="./outputs/fastflow"
OUTPUT_FILE="$OUTPUT_DIR/ff_execution_times.txt"
# mkdir -p $OUTPUT_DIR
echo "n;w;time" > "$OUTPUT_FILE"
chmod +w "$OUTPUT_FILE"

MATRIX_N=(10 100 500 1000 5000 10000)
NUM_WORKERS=(1 2 4 8 12 16 20 24 28)

# Run the tests
cmake .
make

# shellcheck disable=SC2068
for n in ${MATRIX_N[@]}; do
    for w in ${NUM_WORKERS[@]}; do
        echo "Running test for matrix size $n with $w workers"
        ./Fastflow "$n" "$w" >> $OUTPUT_FILE 2>&1
    done
done

cat "$OUTPUT_FILE"