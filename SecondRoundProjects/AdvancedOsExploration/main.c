//
// Created by Alek on 12/14/2025.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "windows.h"
#include "intrin.h"
#include "zero_copy_io_tests.h"

#ifndef FEATURE_TO_TEST
#define FEATURE_TO_TEST (ZERO_COPY)
#endif

#if FEATURE_TO_TEST == ZERO_COPY
#define TEST_TO_CALL (run_zero_copy_tests())
#endif






int main(int argc, char * argv[])
{
#ifdef RUN_ZERO_COPY_TESTS
    zero_copy_test(argv[1], argv[2]);
#endif
}
