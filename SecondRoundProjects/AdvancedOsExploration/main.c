//
// Created by Alek on 12/14/2025.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "windows.h"

#include <direct.h>
// Windows uses _getcwd
#define GetCurrentDir _getcwd

#ifndef FEATURE_TO_TEST
#define FEATURE_TO_TEST (ZERO_COPY)
#endif

#if FEATURE_TO_TEST == ZERO_COPY
#define TEST_TO_CALL (run_zero_copy_tests())
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
        , 0);


    return mapped_address;
}

void zero_copy_tests(char const * const file_to_use)
{
    LPVOID fmaphandle = create_file_mapping_for_read(file_to_use);
    printf("%s\n", ((char*)fmaphandle));
}

int main()
{
    char buf[1024];

    GetCurrentDir(buf, 512);
    printf("%s\n", buf);

    zero_copy_tests("../testfiles/testfiles.txt");

}
