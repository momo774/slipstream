cmake_minimum_required(VERSION 3.28)
project(slipstream)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(slipstream main.cpp)
