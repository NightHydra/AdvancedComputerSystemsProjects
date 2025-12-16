#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <cstdint>

// --- 1. FILTER SELECTION DEFINES ---

// Define which filter to use for the test. Uncomment only ONE of the following:
#define USE_XOR_FILTER
// #define USE_CUCKOO_FILTER
// #define USE_QUOTIENT_FILTER
// #define USE_BLOCKED_BLOOM_FILTER

// --- 2. TEST PARAMETER DEFINES ---

// The total number of keys to insert into the filter (N)
#ifndef N_KEYS
#define N_KEYS (500000)
#endif

// The number of *non-present* keys to check for the False Positive Rate (FPR) test
#ifndef N_TEST_KEYS
#define N_TEST_KEYS (5000000)
#endif

// The number of insertion/query rounds for throughput measurement
#ifndef N_ITERATIONS
#define N_ITERATIONS (10) 
#endif

// --- 3. FILTER TYPE SELECTION AND INCLUSION ---

using KeyT = uint64_t; // Standard key type for testing

#if defined(USE_XOR_FILTER)
#include "xor_filter.h"
#define FilterClass XORFilter<KeyT>
const char* FILTER_NAME = "XOR Filter";

#elif defined(USE_CUCKOO_FILTER)
#include "cuckoo_filter.h"
#define FilterClass CuckooFilter<KeyT>
const char* FILTER_NAME = "Cuckoo Filter";

#elif defined(USE_QUOTIENT_FILTER)
#include "quotient_filter.h"
#define FilterClass QuotientFilter<KeyT>
const char* FILTER_NAME = "Quotient Filter";

#elif defined(USE_BLOCKED_BLOOM_FILTER)
#include "blocked_bloom_filter.h"
#define FilterClass BlockedBloomFilter<KeyT>
const char* FILTER_NAME = "Blocked Bloom Filter";

#else
#error "You must define one filter to use (e.g., #define USE_XOR_FILTER) from the provided set."
#endif

// --- 4. DATA GENERATION AND HARNESS FUNCTIONS ---

/**
 * @brief Generates two sets of keys: one to insert and one for testing non-present keys.
 * @param keys_to_insert Output vector for keys to be inserted (N_KEYS).
 * @param keys_to_test Output vector for keys guaranteed NOT to be present (N_TEST_KEYS).
 */
void generate_test_data(std::vector<KeyT>& keys_to_insert, std::vector<KeyT>& keys_to_test) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    
    // Generator for keys (e.g., 64-bit integers)
    std::uniform_int_distribution<KeyT> distrib; 

    // 1. Generate N_KEYS for insertion
    keys_to_insert.reserve(N_KEYS);
    for (size_t i = 0; i < N_KEYS; ++i) {
        keys_to_insert.push_back(distrib(gen));
    }

    // 2. Generate N_TEST_KEYS (non-present keys)
    // To guarantee non-presence, we ensure a large separation in the key space.
    const KeyT SEPARATION_GAP = 100000000000000ULL; 
    
    // Use a new seed or offset to generate a non-overlapping set
    std::mt19937_64 gen_test(rd() + 1); // Use a slightly different seed

    keys_to_test.reserve(N_TEST_KEYS);
    for (size_t i = 0; i < N_TEST_KEYS; ++i) {
        // Generate a new random number and add the offset
        keys_to_test.push_back(distrib(gen_test) + SEPARATION_GAP);
    }
}

/**
 * @brief Measures the False Positive Rate (FPR) of the filter.
 * @param filter The filter instance to test.
 * @param keys_to_test Vector of non-present keys.
 * @return The calculated FPR as a double.
 */
double measure_fpr(const FilterClass& filter, const std::vector<KeyT>& keys_to_test) {
    size_t false_positives = 0;
    
    // Check for existence of keys guaranteed not to be present
    for (const auto& key : keys_to_test) {
        if (filter.exist(key)) {
            false_positives++;
        }
    }
    return (double)false_positives / keys_to_test.size();
}

/**
 * @brief Measures the throughput (keys/second) for construction and querying.
 * We measure the combined time for construction and query and report the total rate.
 * @param keys_to_insert Vector of keys to insert.
 * @param keys_to_query Vector of keys to query (can be the inserted set).
 * @return The measured throughput in M Keys/second.
 */
double measure_throughput(const std::vector<KeyT>& keys_to_insert, const std::vector<KeyT>& keys_to_query) {
    long double total_duration_ns = 0;
    
    // Total operations includes N_KEYS for construction and N_KEYS for query in each iteration
    const size_t total_ops_per_iteration = keys_to_insert.size() + keys_to_query.size();
    const size_t total_operations = total_ops_per_iteration * N_ITERATIONS;

    for (int i = 0; i < N_ITERATIONS; ++i) {
        // --- Combined Time Measurement (Construction + Query) ---
        auto start = std::chrono::high_resolution_clock::now();
        
        // Construction Time
        FilterClass filter(keys_to_insert);
        
        // Query Time
        for (const auto& key : keys_to_query) {
            // Note: exist() is const, so no state change occurs here
            filter.exist(key);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        
        total_duration_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }
    
    // Calculate throughput: Operations / Time (keys/second)
    double total_duration_s = total_duration_ns / 1e9L;
    double throughput_ops_per_s = total_operations / total_duration_s;
    
    // Output in Mega Keys/second (M keys/s)
    return throughput_ops_per_s / 1e6;
}

// --- 5. MAIN HARNESS ---

int main() {
    std::cout << "--- Filter Testing Harness ---\n";
    std::cout << "Selected Filter: " << FILTER_NAME << "\n";
    std::cout << "Keys to Insert (N): " << N_KEYS << "\n";
    std::cout << "Keys to Test (FPR Check): " << N_TEST_KEYS << "\n";
    std::cout << "Throughput Iterations: " << N_ITERATIONS << "\n";
    std::cout << "------------------------------\n";

    // 1. Generate Data
    std::vector<KeyT> keys_to_insert;
    std::vector<KeyT> keys_to_test_fpr;
    generate_test_data(keys_to_insert, keys_to_test_fpr);
    
    std::cout << "Data generation complete. Starting tests...\n";

    // 2. Measure Throughput (Construction + Query)
    // We use the inserted keys for the query part of the throughput test
    double throughput = measure_throughput(keys_to_insert, keys_to_insert); 

    // 3. Measure FPR
    // Construct the final filter instance once for the FPR test
    try {
        FilterClass final_filter(keys_to_insert);
        double fpr = measure_fpr(final_filter, keys_to_test_fpr);

        // 4. Output Results
        std::cout << "\n--- Results for " << FILTER_NAME << " ---\n";
        std::cout << "Measured False Positive Rate (FPR): " << fpr << "\n";
        std::cout << "Throughput (Insert + Query): " << throughput << " M keys/sec\n";
        std::cout << "--------------------------------\n";
    } catch (const std::runtime_error& e) {
        std::cerr << "\n--- ERROR ---\n";
        std::cerr << FILTER_NAME << " Construction failed: " << e.what() << "\n";
        std::cerr << "Cannot measure FPR.\n";
        std::cout << "Throughput (Insert + Query): " << throughput << " M keys/sec\n";
        std::cout << "--------------------------------\n";
        return 1;
    }

    return 0;
}