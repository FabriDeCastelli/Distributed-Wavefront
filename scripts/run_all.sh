#!/usr/bin/env bash

salloc --nodes 8 --time  00:35:00

./sequential_tests.sh
./ff_tests.sh
./MPI_collective_tests.sh

sleep 10
exit