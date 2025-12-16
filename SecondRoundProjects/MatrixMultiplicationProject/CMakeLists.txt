cmake_minimum_required(VERSION 3.31)
project(MatrixMultiplicationProject)

set(CMAKE_CXX_STANDARD 20)

set(VECTORIZATION_OPTIONS "VECTORIZE" "NO_VECTORIZE")
set(STRUCTURE_OPTIONS "GEMM" "SPMM")
set(TILE_OPTIONS 1 2 4 8 16)
set(THREAD_OPTIONS 1 2 4 8)

set(SOURCE_FILES main.cpp)

foreach(VECTORIZATION_TYPE IN LISTS VECTORIZATION_OPTIONS)
    foreach(STRUCTURE_TYPE IN LISTS STRUCTURE_OPTIONS)
        foreach(TILE_TYPE IN LISTS TILE_OPTIONS)
            foreach(THREAD_TYPE IN LISTS THREAD_OPTIONS)

                set(TARGET_NAME "${VECTORIZATION_TYPE}_${STRUCTURE_TYPE}_${TILE_TYPE}_${THREAD_TYPE}")

                add_executable(${TARGET_NAME} ${SOURCE_FILES})

                if (MSVC)
                    target_compile_options(${TARGET_NAME} PRIVATE "/FAs-")
                    target_link_options(${TARGET_NAME} PRIVATE "/STACK:1000000000")
                endif()

                if (MINGW)
                    message(STATUS "Building with MinGW.")
                    target_compile_options(${TARGET_NAME} PRIVATE -O3 -save-temps)
                    target_link_options(${TARGET_NAME} PRIVATE "-Wl,--stack,1000000000")
                endif()

                if(VECTORIZATION_TYPE STREQUAL "NO_VECTORIZE")
                    target_compile_options( ${TARGET_NAME} PRIVATE -fno-tree-vectorize)
                elseif(VECTORIZATION_TYPE STREQUAL "VECTORIZE")
                    target_compile_options(${TARGET_NAME} PRIVATE -march=native -ftree-vectorize)
                endif()

                target_compile_definitions(${TARGET_NAME} PRIVATE ${STRUCTURE_TYPE} "TILESIZE=${TILE_TYPE}" "NUMTHREADS=${THREAD_TYPE}")
                target_link_options(${TARGET_NAME} PRIVATE -static-libgcc -static-libstdc++)

            endforeach ()
        endforeach ()
    endforeach ()
endforeach ()



