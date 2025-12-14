#include <iostream>
#include <cstdlib>
#include <random>
#include <ctime>
#include <thread>
#include <limits>
#include <ranges>
#include <utility>
#include <string>
// Use intrinsics for cycle count
#include <intrin.h>

#include "ConcurrentHashtable\concurrent_hash_table.h"
#include "operation_generator.h"

#ifndef RW_RAITO
#define RW_RAITO (50)
#endif

#ifndef STARTING_SIZE
#define STARTING_SIZE (1000)
#endif

#ifndef NUM_THREADS
#define NUM_THREADS (1)
#endif


// How our tests work is we start with a certain size of table then perform operations
//     If too many inserts are done then the tests wont be different
#define MAX_INSERT_TO_STARTING_RATIO (2)

#if RW_RATIO > 80
#define NUMBER_OF_OPERATIONS (STARTING_SIZE * 10)
#endif
#if RW_RATIO <= 80
#define NUMBER_OF_OPERATIONS ((STARTING_SIZE*MAX_INSERT_TO_STARTING_RATIO*100)/(100-RW_RATIO))
#endif


#if 0
void run_job(ConcurrentHashTable<int, int> & hash_table_inst)
{
    // Seed the rng, took from cppreference.com
    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()

    // Get a uniform distributation of values
    std::uniform_int_distribution<int> value_distrib(0, std::numeric_limits<int>::max());
    std::uniform_int_distribution<int> find_key_distrib(0, TABLE_SIZE-1);
    std::bernoulli_distribution search_for_key_in_hashset_distrib(0.7);

    int keys_to_insert[TABLE_SIZE];
    int values_to_insert[TABLE_SIZE];

    for (unsigned int i = 0; i<TABLE_SIZE; i++)
    {
        keys_to_insert[i] = value_distrib(gen);
        values_to_insert[i] = value_distrib(gen);
        hash_table_inst.insert(keys_to_insert[i], values_to_insert[i]);
        std::cout << "Inserted key: " << keys_to_insert[i] << ", with  value: " << values_to_insert[i] << std::endl;
    }

    for (unsigned int i = 0; i<10000; ++i)
    {
        int key_to_search_for = 0;
        if (search_for_key_in_hashset_distrib(gen) == true)
        {
            key_to_search_for = keys_to_insert[find_key_distrib(gen)];
        }
        else
        {
            key_to_search_for = value_distrib(gen);
        }
        std::pair<bool, int> find_output = hash_table_inst.find(key_to_search_for);
        if (find_output.first == true)
        {
            std::cout << "Successfully found key: " << key_to_search_for << ", with value: " << find_output.second
                 << std::endl;
        }
        else
        {
            std::cout << "Could not find key " << key_to_search_for << std::endl;
        }
    }
}
#endif

template <class KeyT, class ValT>
void run_operations(ConcurrentHashTable<KeyT, ValT>& hash_table_inst,
    operation_t<KeyT, ValT> * oplist, int start_ind, int end_ind)
{
    for (unsigned int i = start_ind; i<end_ind; ++i)
    {
        if (oplist[i].operation_type == INSERT)
        {
            hash_table_inst.insert(oplist[i].key, oplist[i].value);
        }
        else
        {
            oplist[i].actual_result_bool = hash_table_inst.find(oplist[i].key).first;
        }
    }
}

int main(int argc, char* argv[])
{
    ConcurrentHashTable<int, int> hash_table_inst(STARTING_SIZE * 4);

    // Start by inserting a bunch of random keys before performing the test
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> initial_key_distrib(0, std::numeric_limits<int>::max());
    std::uniform_int_distribution<int> initial_value_distrib(0, std::numeric_limits<int>::max());

    int base_keys[STARTING_SIZE];
    int base_vals[STARTING_SIZE];
    for (unsigned int i = 0; i<STARTING_SIZE; ++i)
    {
        base_keys[i] = initial_key_distrib(gen);
        base_vals[i] = initial_value_distrib(gen);
        hash_table_inst.insert(base_keys[i], base_vals[i]);
    }

    // Generate all the operations to perform before dividing up the threads
    operation_t<int, int> operation_list [NUMBER_OF_OPERATIONS];

    OperationGenerator<int, int> opgen(STARTING_SIZE, base_keys);

    for (unsigned int i = 0; i<(NUMBER_OF_OPERATIONS*RW_RATIO)/100; ++i)
    {
        operation_list[i] = opgen.generate_find_operation();
    }
    for (unsigned int i = (NUMBER_OF_OPERATIONS*RW_RATIO)/100; i<NUMBER_OF_OPERATIONS; ++i)
    {
        operation_list[i] = opgen.generate_insert_operation();
    }

    // Now shuffle the operations
    std::shuffle(operation_list, operation_list + NUMBER_OF_OPERATIONS, gen);

    // Now run all the operations and record time

    int ops_per_thread = NUMBER_OF_OPERATIONS/NUM_THREADS;

    unsigned long long start_cycle_count = __rdtsc();

    std::thread * threads[NUM_THREADS];

    for (unsigned int i = 0; i<(NUM_THREADS-1); ++i)
    {
        threads[i] = new std::thread(run_operations<int, int>, std::ref(hash_table_inst),
            operation_list, ops_per_thread*i, ops_per_thread*(i+1));
    }
    // Wait till all threads end
    threads[NUM_THREADS-1] = new std::thread(run_operations<int, int>, std::ref(hash_table_inst),
        operation_list, ops_per_thread*(NUM_THREADS-1), NUMBER_OF_OPERATIONS);

    for (unsigned int i = 0; i<(NUM_THREADS); ++i)
    {
        threads[i]->join();
        // Delete the dynamically allocated thread ptr
        delete threads[i];
    }

    // Record the end time
    unsigned long long end_cycle_count = __rdtsc();

    if (argc == 2)
    {
        if (strcmp(argv[1], "--print_header") == 0)
        {
            std::cout << "Cycles Taken,Successful Reads,Number of Operations, Cycles Per Operation" << std::endl;
        }
    }

    int number_of_correct_finds = 0;
    for (unsigned int i = 0; i<NUMBER_OF_OPERATIONS; ++i)
    {
        if (operation_list[i].operation_type == FIND && operation_list[i].actual_result_bool == operation_list[i].expected_result_bool) ++number_of_correct_finds;
    }
    float number_of_total_finds = (NUMBER_OF_OPERATIONS*RW_RATIO)/100;

    std::cout << end_cycle_count - start_cycle_count << "," <<
        (((float)number_of_correct_finds) / number_of_total_finds) << ","
        << NUMBER_OF_OPERATIONS << "," << ((double)(end_cycle_count - start_cycle_count))/(NUMBER_OF_OPERATIONS) << std::endl;

    return 0;
}