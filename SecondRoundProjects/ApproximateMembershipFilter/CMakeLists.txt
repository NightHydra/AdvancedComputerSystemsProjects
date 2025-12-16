# --- CMakeLists.txt ---

cmake_minimum_required(VERSION 3.10)
project(FilterTests CXX)

# Set the C++ standard to C++17 (required for advanced features like <chrono>)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define the source file for the test harness
set(HARNESS_SRC main.cpp)

# ----------------------------------------------------------------------
# 1. Define the base common includes (assuming headers are in the same directory)
# ----------------------------------------------------------------------
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ----------------------------------------------------------------------
# 2. Define a function to create a test executable for a specific filter
# ----------------------------------------------------------------------
function(add_filter_test FILTER_NAME FILTER_DEFINE)
    # The target name will be descriptive (e.g., 'test_xor_filter')
    set(TARGET_NAME "test_${FILTER_NAME}")

    # Add the executable target
    add_executable(${TARGET_NAME} ${HARNESS_SRC})

    # Add the required preprocessor definition to select the filter
    target_compile_definitions(${TARGET_NAME}
            PRIVATE
            ${FILTER_DEFINE}
    )

    # Ensure the code can be easily run using CMake's testing infrastructure
    # (Though we won't define the 'test' command here, the executable is ready)
    message(STATUS "Created target: ${TARGET_NAME} for filter: ${FILTER_NAME}")
endfunction()


# ----------------------------------------------------------------------
# 3. Create a target for each filter
# ----------------------------------------------------------------------

# Target 1: XOR Filter
add_filter_test(
        xor_filter
        "USE_XOR_FILTER"
)

# Target 2: Cuckoo Filter
add_filter_test(
        cuckoo_filter
        "USE_CUCKOO_FILTER"
)

# Target 3: Quotient Filter
add_filter_test(
        quotient_filter
        "USE_QUOTIENT_FILTER"
)

# Target 4: Blocked Bloom Filter
add_filter_test(
        blocked_bloom_filter
        "USE_BLOCKED_BLOOM_FILTER"
)

# ----------------------------------------------------------------------
# 4. Optional: Create a pseudo-target to run all tests
# ----------------------------------------------------------------------
# This is a common practice to group all test targets for easy execution.
add_custom_target(run_all_tests
        COMMAND ./test_xor_filter
        COMMAND ./test_cuckoo_filter
        COMMAND ./test_quotient_filter
        COMMAND ./test_blocked_bloom_filter
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        # Dependencies ensure all executables are built before attempting to run
        DEPENDS test_xor_filter test_cuckoo_filter test_quotient_filter test_blocked_bloom_filter
)