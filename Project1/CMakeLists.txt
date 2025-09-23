cmake_minimum_required(VERSION 3.31)
project(ACS_Project1 C)

set(CMAKE_C_STANDARD 11)


add_executable(sample_build main.c
        myperfmodule.h)
add_executable(vectorized_build main.c
        myperfmodule.h)

if (MSVC)
    target_compile_options(sample_build PRIVATE "/FAs-")

    target_link_options(sample_build PRIVATE "/STACK:67108864")
endif()

if (MINGW)
    message(STATUS "Building with MinGW.")

    target_compile_options(sample_build PRIVATE -O3 -fno-tree-vectorize -save-temps)
    target_compile_options(vectorized_build PRIVATE -O3 -save-temps)

    target_link_options(sample_build PRIVATE "-Wl,--stack,67108864")
    target_link_options(vectorized_build PRIVATE "-Wl,--stack,67108864")
endif()