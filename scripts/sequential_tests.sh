#!/usr/bin/env bash

# This script is used to run the tests for the ff program.
OUTPUT_DIR="../outputs/sequential"
OUTPUT_FILE="$OUTPUT_DIR/sequential.csv"

echo "n;time" > "$OUTPUT_FILE"
chmod +w "$OUTPUT_FILE"

MATRIX_N=(100 200 400 800 1600 3200)

# Run the tests
cd ..
cmake -S . -B build
cd build || exit
make

# shellcheck disable=SC2068
for n in ${MATRIX_N[@]}; do
    echo "Running test for matrix size $n"
    # shellcheck disable=SC2034
    for (( i=1; i <= 5; ++i )); do
        output=$(srun ./Sequential "$n")
        echo "$output" >> "$OUTPUT_FILE"
    done
done

cat "$OUTPUT_FILE"

cd ..