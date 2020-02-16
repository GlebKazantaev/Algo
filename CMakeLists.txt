cmake_minimum_required(VERSION 3.15)
project(AlgoLibrary C CXX)

set(LIB_FILES libalgo/dsu.hpp
              libalgo/aho_korasik.hpp
              libalgo/max_flow.hpp
              libalgo/tmp.cpp)

add_library(algo SHARED ${LIB_FILES})
target_include_directories(algo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libalgo)

set(UNIT_TESTS_FILES tests/unit-tests.cpp)
add_executable(unit-tests ${UNIT_TESTS_FILES})