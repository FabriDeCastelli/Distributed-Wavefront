cmake_minimum_required(VERSION 3.20)
project(Distributed_Wavefront)

# Sets the C++ standard to be used for the project to C++17.
set(CMAKE_CXX_STANDARD 17)

# Specifies that the MPI C++ compiler (mpic++) should be used as the C++ compiler.
set(CMAKE_CXX_COMPILER mpic++)

# Sets the compilation flags
set(CMAKE_CXX_FLAGS "-O3")

# If MPI package is not found, the configuration process will stop with an error.
find_package(MPI REQUIRED)
if (MPI_FOUND)
    include_directories(${MPI_INCLUDE_PATH})
else()
    message(FATAL_ERROR "MPI not found")
endif()

# Adds the MPI include directories to the compiler's search path (used to find mpi.h).
include_directories(${MPI_INCLUDE_PATH}, ./fastflow)

add_executable(Sequential
        src/main/wavefront_seq.cpp
        src/main/utils.hpp
        src/main/config.hpp
)
add_executable(MPI
        src/main/wavefront_mpi_p2p.cpp
        src/main/utils.hpp
        src/main/config.hpp
        src/main/wavefront_mpi_p2p.cpp
)
add_executable(MPI_Collective
        src/main/wavefront_mpi_collective.cpp
        src/main/utils.hpp
        src/main/config.hpp
)
add_executable(Fastflow
        src/main/wavefront_fastflow.cpp
        src/main/utils.hpp
        src/main/config.hpp
)
add_executable(Correctness
        src/tests/test_correctness.cpp
        src/main/utils.hpp
        src/main/config.hpp
)

# Links the MPI libraries to the executable.
 target_link_libraries(MPI ${MPI_LIBRARIES})