cmake_minimum_required(VERSION 3.31)
project(ApproximateMembershipFilter CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(ApproximateMembershipFilter main.cpp
        xor_filter.h
        cuckoo_filter.h
        quotient_filter.h)

find_package(xxHash CONFIG REQUIRED)
target_link_libraries(ApproximateMembershipFilter PRIVATE xxHash::xxhash)