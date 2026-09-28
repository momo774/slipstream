cmake_minimum_required(VERSION 3.28)
project(slipstream LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

option(SLIPSTREAM_BUILD_TESTS "Build unit tests (GoogleTest)" OFF)
option(SLIPSTREAM_BUILD_BENCH "Build the latency benchmark harness" OFF)

add_subdirectory(src)

if(SLIPSTREAM_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(SLIPSTREAM_BUILD_BENCH)
    add_subdirectory(bench)
endif()
