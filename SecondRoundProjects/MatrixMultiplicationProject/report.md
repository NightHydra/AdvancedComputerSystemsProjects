# Project 1 Report
## Alek Krupka

## Introduction

This report outlines a testing procedure to test the performance of dense x dense (GEMM) and
spare x dense (SPMM) matrix multiplication under a variety of varying parameters.  The
varied parameters include the following.

1. SPMM vs GEMM multiplication
2. SIMD and VECTORIZATION
3. Number of threads used to compute the result
4. Tile size for cache-aware tiling of GEMM multiplication
5. Sparsity of the matrix (sparsity = 20) implies only 20% of values
in the matrix are nonzero.
6. Matrix size ie(1024 x 8) times (8x 256)

The code written allows the first four parameters to be changed using different build
defines while the last two parameters are changed using command line arguments.

The reason the parameters are changed this way is to maximize performance when running the code
and #defines allow things like thread count to be more easily optimized into the code.  For the
sparsity, this only affects how the matrix is produced, meaning that there is no performance
loss by assigning these values to variables and branching.  For things like SPMM vs GEMM I did not
want any branching to cause a decrease in performance, which is why that setting is a #define.

## Testing Done To Ensure Correctness

A basic matrix of all 1s was used for the tiling options and threading
to ensure correctness of the algorithm.  Separate tests were conducted to ensure these matrices
were evaluated correctly.

## A Note on Tiling
For the GEMM algorithm, tiling was used to reduce cache misses.
No data was needed for the effects of different tile sizes so all tests were conducted with a tile size
of 4 even though the built applications for different tiling sizes are included within the build.

## Scalar vs SIMD Single Thread Baselines

## Effects of SIMD and Threading on Performance

## Effects of Size and Memory on Performance

## Arithmetric Intensity Analysis and Roofline Performance

## Density Sweep and SPMM performance


