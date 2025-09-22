cmake_minimum_required(VERSION 3.31)
project(ACS_Project1 C)

set(CMAKE_C_STANDARD 11)

add_executable(sample_build main.c)

add_compile_options("-fno-vectorize")

target_compile_options(sample_build PRIVATE -save-temps)

target_link_options(sample_build PRIVATE "/STACK:67108864")

