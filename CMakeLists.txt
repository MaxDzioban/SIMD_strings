cmake_minimum_required(VERSION 3.30)
project(SIMD_strings)
set(CMAKE_CXX_STANDARD 20)


add_executable(SIMD_time_comparing main.cpp mystring_simd.h mystring.cpp mystring.hpp time_counnt.cpp)
add_executable(SIMD_str_tests main.cpp mystring_simd.h own_tests_simd.cpp)
add_executable(str_tests mystring.cpp mystring.hpp own_tests.cpp)

#ifdef __amd64__
add_compile_options(-mavx -mavx2)
add_executable(AVX2_str_tests mystring_avx.cpp mystring_avx.h own_tests_avx.cpp)
#endif
