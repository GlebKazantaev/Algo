cmake_minimum_required(VERSION 3.15)
project(AlgoLibrary C CXX)

set(LIB_FILES dsu.hpp
        aho_korasik.hpp
        max_flow.hpp
        tmp.cpp)

add_library(algo SHARED ${LIB_FILES})
target_include_directories(algo PUBLIC .)

set(UNIT_TESTS_FILES tests/unit-tests.cpp)
add_executable(unit-tests ${UNIT_TESTS_FILES})