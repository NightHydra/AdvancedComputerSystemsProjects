#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <intrin.h> // For __rdtsc()
#include "myperfmodule.h"
#ifndef NUMBER_TYPE
#define NUMBER_TYPE float
#endif
#define NUMMIN (-100)
#define NUMMAX (100)

#ifndef TESTSIZE
#define TESTSIZE (10000)
#endif

#ifndef STRIDE_LEN
#define STRIDE_LEN (2)
#endif

#define NUM_ITERATIONS_TO_RUN (20)


#define scalar_value (3.72)

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




NUMBER_TYPE dot_product(NUMBER_TYPE const * a, NUMBER_TYPE const * b, int len);
void elementwise_multiply(NUMBER_TYPE const * a, NUMBER_TYPE const * b, NUMBER_TYPE * result, int len);
void read_command_line_args();
void fill_in_array_with_random_numbers(NUMBER_TYPE * arr, unsigned int len);
perf_t const * run_test(command_args_t const * runtime_options);

int main(int argc, char * argv[])
{
    // Seed the RNG
    srand((unsigned int) time(NULL));

    command_args_t runtime_options;
    read_command_line_args(argc, argv, &runtime_options);

    perf_t perfrun;

    for (unsigned int i = 0; i<10; ++i)
    {
        perfrun = *run_test(&runtime_options);
    }
    printf("Cycles,Time,CPU Frequency,GFLOPS, CPE\n");
    for (unsigned int i = 0; i<NUM_ITERATIONS_TO_RUN; ++i)
    {
        perfrun = *run_test(&runtime_options);
        printf("%lf, %.10lf, %lf, %lf, %.10lf\n", (double) perfrun.elapsed_cycles,
            perfrun.elapsed_time, perfrun.measured_freq, TESTSIZE / perfrun.elapsed_time, ((double)perfrun.elapsed_cycles)/TESTSIZE);
    }


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

    for (unsigned int stride_num = 0; stride_num < STRIDE_LEN; ++stride_num)
    {
        for (unsigned int i = stride_num; i < len; i += STRIDE_LEN)
        {
            result += a[i] * b[i];
        }
    }
    return result;
}
void elementwise_multiply(NUMBER_TYPE const * a, NUMBER_TYPE const * b, NUMBER_TYPE * result, int len)
{
    for (unsigned int stride_num = 0; stride_num < STRIDE_LEN; ++stride_num)
    {
        for (unsigned int i = stride_num; i < len; i += STRIDE_LEN)
        {
            result[i] = a[i] * b[i];
        }
    }
}

NUMBER_TYPE stream(NUMBER_TYPE const * a, int len)
{
    double y = 0.0;
    for (unsigned int stride_num = 0; stride_num < STRIDE_LEN; ++stride_num)
    {
        for (unsigned int i = stride_num; i < len; i += STRIDE_LEN)
        {
            y += scalar_value*a[i];
        }
    }
    return y;
}


void fill_in_array_with_random_numbers(NUMBER_TYPE * arr, unsigned int len)
{
    for (unsigned int i = 0; i<len; ++i)
    {
        arr[i] = ((float)rand())/((float)RAND_MAX) * (NUMMAX - NUMMIN) + NUMMIN;
    }
}

perf_t const * run_test(command_args_t const * runtime_options)
{
    FILE* logfile = fopen("log.txt", "a");

    perf_t const * runtimeperformance;

#ifndef ALIGN_ARRAYS
    NUMBER_TYPE arr1[TESTSIZE];
    NUMBER_TYPE arr2[TESTSIZE];
#endif

#ifdef ALIGN_ARRAYS
    NUMBER_TYPE arr1[TESTSIZE] __attribute__((aligned(64)));
    NUMBER_TYPE arr2[TESTSIZE] __attribute__((aligned(64)));
#endif

    fill_in_array_with_random_numbers(arr1, sizeof(arr1)/sizeof(NUMBER_TYPE));
    fill_in_array_with_random_numbers(arr2, sizeof(arr2)/sizeof(NUMBER_TYPE));

    if (runtime_options->test_to_run == STREAM)
    {
        start_performace_measurement();

        NUMBER_TYPE out = stream(arr1, sizeof(arr1)/sizeof(NUMBER_TYPE));

        runtimeperformance = end_performace_measurement();

        fprintf(logfile, "Performance Freq: %lf\n", runtimeperformance->measured_freq);
        fprintf(logfile, "Calculated %lf in %lld cycles (%.9lf seconds)", out, runtimeperformance->elapsed_cycles,
            runtimeperformance->elapsed_time);
    }
    if (runtime_options->test_to_run == REDUCE)
    {
        start_performace_measurement();

        NUMBER_TYPE out = dot_product(arr1, arr2, sizeof(arr1)/sizeof(NUMBER_TYPE));

        runtimeperformance = end_performace_measurement();

        fprintf(logfile, "Performance Freq: %lf\n", runtimeperformance->measured_freq);
        fprintf(logfile, "Calculated %lf in %lld cycles (%.9lf seconds)", out, runtimeperformance->elapsed_cycles,
            runtimeperformance->elapsed_time);
    }

    if (runtime_options->test_to_run == MULTIPLY)
    {
#ifndef ALIGN_ARRAYS
        NUMBER_TYPE res[TESTSIZE];
#endif

#ifdef ALIGN_ARRAYS
        NUMBER_TYPE res[TESTSIZE] __attribute__((aligned(64)));
#endif

        start_performace_measurement();

        elementwise_multiply(arr1, arr2, res, sizeof(arr1)/sizeof(NUMBER_TYPE));

        runtimeperformance = end_performace_measurement();

        fprintf(logfile,"Performance Freq: %lf\n", runtimeperformance->measured_freq);
        fprintf(logfile, "Calculated [%lf .... %lf] in %lld cycles (%.9lf seconds)", res[0],
            res[sizeof(res)/sizeof(NUMBER_TYPE)-1],
            runtimeperformance->elapsed_cycles,
            runtimeperformance->elapsed_time);
    }
    fclose(logfile);
    return runtimeperformance;
}