cmake_minimum_required(VERSION 3.30)
project(SIMD_strings)
set(CMAKE_CXX_STANDARD 20)

add_executable(SIMD_time_comparing main.cpp mystring_simd.h mystring.cpp mystring.hpp time_counnt.cpp)
add_executable(SIMD_str_tests main.cpp mystring_simd.h own_tests_simd.cpp)
add_executable(str_tests mystring.cpp mystring.hpp own_tests.cpp)
