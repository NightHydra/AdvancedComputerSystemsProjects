//
// Created by Alek on 12/16/2025.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "windows.h"
#include "intrin.h"

#ifndef ZERO_COPY_IO_TESTS_H
#define ZERO_COPY_IO_TESTS_H

#ifndef FILE_SIZE
#define FILE_SIZE (100000000)
#endif

/**
 * @brief This function creates a shared file using the windows.h library and this will eventually be
 *     used for zero copy IO
 * @param fname A pointer to a character array representing the name of the file
 *    to read from
 */
LPVOID create_file_mapping_for_read(char const * const fname)
{

    HANDLE file_handle = CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    HANDLE file_mapping = CreateFileMapping(file_handle, NULL, PAGE_READONLY,
        0, 0, NULL);

    LPVOID mapped_address = MapViewOfFile(file_mapping, FILE_MAP_READ, 0, 0
        ,0);

    return mapped_address;
}

void zero_copy_test(char const * const file_to_use, char const * const file2_to_use)
{
    LPVOID fmaphandle = create_file_mapping_for_read(file_to_use);
    LPVOID fmaphandle2 = create_file_mapping_for_read(file2_to_use);

    unsigned long long start_cycle_nocopy = __rdtsc();
    int comp1 = strcmp((char*)fmaphandle, (char*)fmaphandle2);
    unsigned long long end_cycle_nocopy = __rdtsc();


    FILE * f1_ptr = fopen(file_to_use, "r");
    FILE * f2_ptr = fopen(file2_to_use, "r");

    char buf1[FILE_SIZE+1];
    char buf2[FILE_SIZE+1];

    unsigned long long start_cycle_copy = __rdtsc();
    fgets(buf1, FILE_SIZE, f1_ptr);
    fgets(buf2, FILE_SIZE, f2_ptr);
    int comp2 = strcmp((char*)buf1, (char*)buf2);
    unsigned long long end_cycle_copy = __rdtsc();


    fclose(f1_ptr);
    fclose(f2_ptr);

    printf("%llu,%llu,%d,%d\n", end_cycle_nocopy - start_cycle_nocopy,end_cycle_copy - start_cycle_copy, comp1, comp2);

    if (comp1 != comp2)
    {
        printf("SOMETHING WENT WRONG");
    }
}

#endif //ZERO_COPY_IO_TESTS_H
