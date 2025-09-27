cmake_minimum_required(VERSION 3.31)
project(ACS_Project1 C)

set(CMAKE_C_STANDARD 11)

set(VECTORIZATION_OPTIONS "VECTORIZE" "NO_VECTORIZE" "AVX")
set(FLOAT_OPTIONS "double" "float")

# 48 KB,
# 2048, 131072, 2097152, 33554432
set(WORKING_SET_OPTIONS "L1" "L2" "LLC" "DRAM")

set(STRIDE_OPTIONS 1 2 4 8)

set(ALIGN_OPTIONS "NO_ALIGN" "ALIGN_ARRAYS")


# Your source files
set(SOURCE_FILES main.c)

# Loop through all combinations
foreach(VECTORIZATION_TYPE IN LISTS VECTORIZATION_OPTIONS)
    foreach(FLOAT_TYPE IN LISTS FLOAT_OPTIONS)
        foreach(WORKING_SET_SIZE IN LISTS WORKING_SET_OPTIONS)
            foreach(STRIDE_LEN IN LISTS STRIDE_OPTIONS)
                foreach (ALIGN_OPTION IN LISTS ALIGN_OPTIONS)

                    set(TARGET_NAME "${VECTORIZATION_TYPE}_${FLOAT_TYPE}_${WORKING_SET_SIZE}_${STRIDE_LEN}_${ALIGN_OPTION}")

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
                    elseif(VECTORIZATION_TYPE STREQUAL "AVX")
                        target_compile_options(${TARGET_NAME} PRIVATE -march=native -ftree-vectorize)
                    endif()

                    target_compile_definitions(${TARGET_NAME} PRIVATE "NUMBER_TYPE=${FLOAT_TYPE}" "STRIDE_LEN=${STRIDE_LEN}" ${ALIGN_OPTION})

                    if(WORKING_SET_SIZE STREQUAL "L1")
                        target_compile_definitions(${TARGET_NAME} PRIVATE "TESTSIZE=2048")
                    elseif(WORKING_SET_SIZE STREQUAL "L2")
                        target_compile_definitions(${TARGET_NAME} PRIVATE "TESTSIZE=131072")
                    elseif(WORKING_SET_SIZE STREQUAL "LLC")
                        target_compile_definitions(${TARGET_NAME} PRIVATE "TESTSIZE=2097152")
                    else()
                        target_compile_definitions(${TARGET_NAME} PRIVATE "TESTSIZE=33554432")
                    endif()
                endforeach ()
            endforeach()
        endforeach()
    endforeach()
endforeach()