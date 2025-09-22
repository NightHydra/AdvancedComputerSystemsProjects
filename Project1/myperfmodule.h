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

typedef struct
{
    long long endtimestamp;
    long long starttimestamp;
    LARGE_INTEGER start_perf_counter;
    LARGE_INTEGER end_perf_counter;
    LARGE_INTEGER false_freq_start;
    LARGE_INTEGER false_freq_end;
} perf_temps_t;

static void start_performace_measurement(perf_temps_t * blank_perf_entry)
{
    QueryPerformanceFrequency(&blank_perf_entry->false_freq_start);
    QueryPerformanceCounter(&blank_perf_entry->start_perf_counter);
    blank_perf_entry->starttimestamp = __rdtsc();
}

static perf_t const * end_performace_measurement(perf_temps_t * started_perf_entry)
{
    started_perf_entry->endtimestamp = __rdtsc();
    QueryPerformanceCounter(&started_perf_entry->end_perf_counter);
    QueryPerformanceFrequency(&started_perf_entry->false_freq_end);

    double false_freq_ave = (started_perf_entry->false_freq_start.QuadPart+started_perf_entry->false_freq_end.QuadPart)/2;

    double false_elapsed_sec = (double)(started_perf_entry->end_perf_counter.QuadPart - started_perf_entry->start_perf_counter.QuadPart)/(false_freq_ave);
    double actual_freq = ((double) (started_perf_entry->endtimestamp-started_perf_entry->starttimestamp))/false_elapsed_sec;

    double real_elapsed_time = (started_perf_entry->endtimestamp-started_perf_entry->starttimestamp)/actual_freq;

    global_perf.elapsed_cycles = started_perf_entry->endtimestamp - started_perf_entry->starttimestamp;
    global_perf.elapsed_time = real_elapsed_time;
    global_perf.measured_freq = actual_freq;

    return &global_perf;
}

#endif //MYPERFMODULE_H
