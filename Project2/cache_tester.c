#include <stdio.h>
#include <stdlib.h> // For rand() and srand()
#include <time.h>

#ifndef WALK_SIZE
#define WALK_SIZE (2000)
#endif

#define NUM_ITERATIONS (50000000)

// Go large enough to cover the DRAM
#define ARRAY_SIZE (33554432)

int walktest()
{
    int arr[ARRAY_SIZE];

    unsigned long long ind = ARRAY_SIZE/2;

    // DO A RANDOM WALK TEST to vary cache locality
    for (unsigned int i = 0; i < NUM_ITERATIONS; ++i)
    {
        ind = (ind + ARRAY_SIZE - WALK_SIZE + (rand() % (WALK_SIZE>>1)+1))%WALK_SIZE;

        arr[ind] = rand()%100;
    }
    return arr[ind];
}

int main()
{
    srand(time(NULL));

    printf("%d\n", walktest());
}


