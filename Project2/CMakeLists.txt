cmake_minimum_required(VERSION 3.31)
project(ACS_Project2 C)

set(CMAKE_C_STANDARD 11)

set(WALK_SIZES 1000 16000 2560000 2000000 10000000 50000000)

set(SOURCE_FILES cache_tester.c)

foreach(WALK_SIZE IN LISTS WALK_SIZES)

    set(TARGET_NAME "WALK_SIZE_${WALK_SIZE}")
    add_executable(${TARGET_NAME} ${SOURCE_FILES})

    if (MINGW)
        message(STATUS "Building with MinGW.")
        target_compile_options(${TARGET_NAME} PRIVATE -O3)
        target_link_options(${TARGET_NAME} PRIVATE "-Wl,--stack,1000000000")
    endif()
    if (MSVC)
        target_link_options(${TARGET_NAME} PRIVATE "/STACK:1000000000")
    endif()

    target_compile_definitions(${TARGET_NAME} PRIVATE "WALK_SIZE=${WALK_SIZE}")

endforeach ()