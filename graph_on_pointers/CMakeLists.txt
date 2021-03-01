cmake_minimum_required(VERSION 3.15)
project(code_snippets)

set(CMAKE_CXX_STANDARD 14)

add_library(graph_implementations SHARED
        graph.cpp
        graph.hpp
        node.hpp
        node.cpp
        ops.hpp
        tensor.hpp)

target_include_directories(graph_implementations PRIVATE ..)

add_executable(run tests/main.cpp)
target_link_libraries(run graph_implementations)
