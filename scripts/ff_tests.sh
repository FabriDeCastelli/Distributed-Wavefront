#!/usr/bin/env bash

# This script is used to run the tests for the ff program.
OUTPUT_DIR="../outputs/fastflow"
OUTPUT="$OUTPUT_DIR/fastflow.csv"
# mkdir -p $OUTPUT_DIR
echo "n;w;time" > "$OUTPUT"
chmod +w "$OUTPUT"

MATRIX_N=(200 400 800 1600 3200)
NUM_WORKERS=(1 2 4 8 12 16 20)

# Run the tests
cd ..
cmake -S . -B build
cd build || exit
make

# shellcheck disable=SC2068
for n in ${MATRIX_N[@]}; do
    for nw in ${NUM_WORKERS[@]}; do
        echo "Running test for matrix size $n with $nw workers"
        srun ./Fastflow "$n" "$nw" >> $OUTPUT 2>&1
    done
done

echo "Fastflow test finished"
