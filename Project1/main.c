#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <intrin.h> // For __rdtsc()



#ifndef NUMBER_TYPE
#define NUMBER_TYPE float
#endif
#define NUMMIN (-100)
#define NUMMAX (100)
#define TESTSIZE (16384)

#ifndef ALIGN_ARRAYS
#define ALIGN_ARRAYSx
#endif

typedef enum
{
    STREAM,
    REDUCE,
    MULTIPLY,
    STENCIL
} kernel_type_t;

typedef struct
{
    kernel_type_t test_to_run;
} command_args_t;

/*
 * The below code is global for a reason, I want it statically inlined for fast performance
 * measurements.
 */
//LARGE_INTEGER start_perf_counter, end_perf_counter, false_freq;



NUMBER_TYPE dot_product(NUMBER_TYPE const * a, NUMBER_TYPE const * b, int len);
void elementwise_multiply(NUMBER_TYPE const * a, NUMBER_TYPE const * b, NUMBER_TYPE * result, int len);
void read_command_line_args();
void fill_in_array_with_random_numbers(NUMBER_TYPE * arr, unsigned int len);
void run_test(command_args_t const * runtime_options);

int main(int argc, char * argv[])
{
    // Seed the RNG
    srand((unsigned int) time(NULL));

    command_args_t runtime_options;
    read_command_line_args(argc, argv, &runtime_options);

    run_test(&runtime_options);

    return 0;
}

void read_command_line_args(int argc, char const * const * const args, command_args_t * const args_struct)
{
    for (unsigned int i = 1; i<argc; ++i)
    {
        char const * current_arg = args[i];

        if (current_arg[0] == '-')
        {
            if (strcmp(current_arg, "-kernel") == 0)
            {
                char const * kern_type = args[++i];
                if (strcmp(kern_type, "STREAM") == 0) args_struct->test_to_run = STREAM;
                if (strcmp(kern_type, "REDUCE") == 0) args_struct->test_to_run = REDUCE;
                if (strcmp(kern_type, "MULTIPLY") == 0) args_struct->test_to_run = MULTIPLY;
                if (strcmp(kern_type, "STENCIL") == 0) args_struct->test_to_run = STENCIL;
            }
        }
    }
}

NUMBER_TYPE dot_product(NUMBER_TYPE const * a, NUMBER_TYPE const * b, int len)
{
    NUMBER_TYPE result = 0;
    for (unsigned int i = 0; i < len; i++)
    {
        result += a[i] * b[i];
    }
    return result;
}
void elementwise_multiply(NUMBER_TYPE const * a, NUMBER_TYPE const * b, NUMBER_TYPE * result, int len)
{
    for (unsigned int i = 0; i < len; ++i)
    {
        result[i] = a[i] * b[i];
    }
}


void fill_in_array_with_random_numbers(NUMBER_TYPE * arr, unsigned int len)
{
    for (unsigned int i = 0; i<len; ++i)
    {
        arr[i] = ((float)rand())/((float)RAND_MAX) * (NUMMAX - NUMMIN) + NUMMIN;
    }
}

void run_test(command_args_t const * runtime_options)
{
#ifndef ALIGN_ARRAYS
    NUMBER_TYPE arr1[TESTSIZE];
    NUMBER_TYPE arr2[TESTSIZE];
#endif

#ifdef ALIGN_ARRAYS
    NUMBER_TYPE arr1[TESTSIZE] __attribute__((aligned(4*sizeof(NUMBER_TYPE))));
    NUMBER_TYPE arr2[TESTSIZE] __attribute__((aligned(4*sizeof(NUMBER_TYPE))));
#endif

    fill_in_array_with_random_numbers(arr1, sizeof(arr1)/sizeof(NUMBER_TYPE));
    fill_in_array_with_random_numbers(arr2, sizeof(arr2)/sizeof(NUMBER_TYPE));

    if (runtime_options->test_to_run == REDUCE)
    {
        LARGE_INTEGER start_perf_counter, end_perf_counter;
        LARGE_INTEGER perfmormance_freq;
        QueryPerformanceFrequency(&perfmormance_freq);

        QueryPerformanceCounter(&start_perf_counter);
        unsigned long long startstamp = __rdtsc();

        NUMBER_TYPE out = dot_product(arr1, arr2, sizeof(arr1)/sizeof(NUMBER_TYPE));

        QueryPerformanceCounter(&end_perf_counter);
        unsigned long long endstamp = __rdtsc();

        double elapsed_sec = (double)(end_perf_counter.QuadPart - start_perf_counter.QuadPart)/(perfmormance_freq.QuadPart);
        double frequency = ((double)(endstamp-startstamp))/elapsed_sec;
        printf("Performance Freq: %lf\n", frequency);

        double timediff = ((double)(end_perf_counter.QuadPart - start_perf_counter.QuadPart))/(double)(frequency);
        printf("Calculated %lf in %lld cycles (%.9lf seconds)", out, endstamp - startstamp, timediff);
    }

    if (runtime_options->test_to_run == MULTIPLY)
    {
#ifndef ALIGN_ARRAYS
        NUMBER_TYPE res[TESTSIZE];
#endif

#ifdef ALIGN_ARRAYS
        NUMBER_TYPE res[TESTSIZE] __attribute__((aligned(4*sizeof(NUMBER_TYPE))));
#endif

        LARGE_INTEGER start_perf_counter, end_perf_counter;
        LARGE_INTEGER frequency;

        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start_perf_counter);
        unsigned long long startstamp = __rdtsc();

        elementwise_multiply(arr1, arr2, res, sizeof(arr1)/sizeof(NUMBER_TYPE));

        printf("%d\n", res[0]);

        QueryPerformanceCounter(&end_perf_counter);
        unsigned long long endstamp = __rdtsc();

        double timediff = ((double)(end_perf_counter.QuadPart - start_perf_counter.QuadPart))/(double)(frequency.QuadPart);
        printf("Elementwise Multiply Test: %lld cycles (%.9lf seconds)", endstamp - startstamp, timediff);
    }
}