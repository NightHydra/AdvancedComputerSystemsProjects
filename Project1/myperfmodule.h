//
// Created by Alek on 9/22/2025.
//

#include <time.h>
#include <intrin.h> // For __rdtsc()

#ifndef MYPERFMODULE_H
#define MYPERFMODULE_H

/*
 * The below code is global for a reason, I want it statically inlined for fast performance
 * measurements.
 */
typedef struct
{
    long long elapsed_cycles;
    double elapsed_time;
    double measured_freq;
} perf_t;

perf_t global_perf;
LARGE_INTEGER start_perf_counter, end_perf_counter, false_freq_start, false_freq_end;
long long endtimestamp, starttimestamp;

static void start_performace_measurement()
{
    QueryPerformanceCounter(&start_perf_counter);
    starttimestamp = __rdtsc();
    QueryPerformanceFrequency(&false_freq_start);
}

static perf_t const * end_performace_measurement()
{
    QueryPerformanceCounter(&end_perf_counter);
    endtimestamp = __rdtsc();
    QueryPerformanceFrequency(&false_freq_end);

    double false_freq_ave = (false_freq_start.QuadPart+false_freq_end.QuadPart)/2;

    double false_elapsed_sec = (double)(end_perf_counter.QuadPart - start_perf_counter.QuadPart)/(false_freq_ave);
    double actual_freq = ((double) (endtimestamp-starttimestamp))/false_elapsed_sec;

    double real_elapsed_time = (endtimestamp-starttimestamp)/actual_freq;

    global_perf.elapsed_cycles = endtimestamp - starttimestamp;
    global_perf.elapsed_time = real_elapsed_time;
    global_perf.measured_freq = actual_freq;

    return &global_perf;
}

#endif //MYPERFMODULE_H
