#!/usr/bin/env bash

# This script is used to run the tests for the ff program.
OUTPUT_DIR="./outputs/sequential"
OUTPUT_FILE="$OUTPUT_DIR/sequential_execution_times.txt"
# mkdir -p $OUTPUT_DIR
echo "n;time" > "$OUTPUT_FILE"
chmod +w "$OUTPUT_FILE"

MATRIX_N=(10 100 500 1000 5000 10000)

# Run the tests
cmake .
make

# shellcheck disable=SC2068
for n in ${MATRIX_N[@]}; do
    echo "Running test for matrix size $n"
    ./Sequential "$n" >> $OUTPUT_FILE 2>&1
done

cat "$OUTPUT_FILE"