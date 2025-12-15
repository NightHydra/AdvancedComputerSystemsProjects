//
// Created by Alek on 12/14/2025.
//

#include <stdio.h>
#include <stdlib.h>

#ifndef FEATURE_TO_TEST
#define FEATURE_TO_TEST (ZERO_COPY)
#endif

#if FEATURE_TO_TEST == ZERO_COPY
#define TEST_TO_CALL (run_zero_copy_tests())
#endif

void zero_copy_tests()
{

}

int main()
{

}
