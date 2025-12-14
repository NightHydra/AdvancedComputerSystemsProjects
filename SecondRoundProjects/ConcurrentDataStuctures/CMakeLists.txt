cmake_minimum_required(VERSION 3.31)
project(ConcurrentDataStuctures)

set(CMAKE_CXX_STANDARD 20)

set(LOCKING_TYPE "COURSE_GRAINED" "FINE_GRAINED")
# Read/Write Ratio
set(WORKLOAD 100 70 0)
set(DATASET_SIZES 10000 100000 1000000)
set(THREAD_COUNT 1 2 4 8 16)

set(SOURCE_FILES main.cpp
        operation_generator.h
)

find_package(xxHash CONFIG REQUIRED)

foreach(LOCKING IN LISTS LOCKING_TYPE)
    foreach(WORKLD IN LISTS WORKLOAD)
        foreach(SET_SIZE IN LISTS DATASET_SIZES)
            foreach(THREAD_NUM IN LISTS THREAD_COUNT)

                set (TARGET_NAME "${LOCKING}_${WORKLD}_${SET_SIZE}_${THREAD_NUM}")
                add_executable(${TARGET_NAME} ${SOURCE_FILES})
                target_include_directories(${TARGET_NAME} PRIVATE ConcurrentHashtable)

                target_compile_definitions(${TARGET_NAME} PRIVATE ${LOCKING} "RW_RATIO=${WORKLD}"
                        "STARTING_SIZE=${SET_SIZE} " "NUM_THREADS=${THREAD_NUM}")

                target_link_libraries(${TARGET_NAME} PRIVATE xxHash::xxhash)
                target_link_options(${TARGET_NAME} PRIVATE -static-libgcc -static-libstdc++)

                if (MINGW)
                    message(STATUS "Building with MinGW.")
                    # Give myself a big stack for storing insert ops
                    target_link_options(${TARGET_NAME} PRIVATE "-Wl,--stack,1000000000")
                endif()

            endforeach ()
        endforeach ()
    endforeach ()
endforeach ()

