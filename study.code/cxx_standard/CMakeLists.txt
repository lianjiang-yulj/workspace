cmake_minimum_required(VERSION 3.12)
project(cxx_standard)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(c++11)
add_subdirectory(c++14)
add_subdirectory(c++17)
add_executable(cxx_standard main.cpp)
