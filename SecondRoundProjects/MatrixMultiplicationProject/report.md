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

Below is the baseline performance plot for the scalar vs SIMD
performance of GEMM and SPMM.

![Baseline](data_analysis/plots/scalar_simd_baselines.png)

As shown, SIMD performed slightly better than the scalar implementation
for sparse matrix multiplication while performing slightly worse for dense matrix
multiplication.  Overall, sparse matrix multiplcation performed far better than
dense for both tests, most likely because far fewer operations
needed to be performed for dense multiplication.

## Effects of SIMD and Threading on Performance

Now, we include the data when threading is added to the implementation.
For this implementation, each operation was completed by 4 threads allowing
the job to be done much faster.

![scalar_simd_vectorization](data_analysis/plots/scalar_threading.png)

As shown, adding multithreading almost doubles the speed of the process and has a far
greater impact than just SIMD.  However, SPMM still far outperforms GEMM
by a wide margin.

Next, the following plot shows the results when the number of threads used are
swept from 1-8.

![threading_plot](data_analysis/plots/thread_sweep.png)

As shown, for both the GEMM and SPMM algorithms, adding more
threads vastly increased the efficiency of computing the multiplication.
This again shows that increased thread count yields better results.


## Effects of Size and Memory on Performance

## Arithmetric Intensity Analysis and Roofline Performance

## Density Sweep and SPMM performance

The following 4 plots show the results when the sparsity of the matrix is swept from
0.1% dense to 50% dense.  Note that lower percents imply the matrix is more sparse.

![sparsity_t1](data_analysis/plots/sparsity_sweep_1_thread.png)
![sparsity_t2](data_analysis/plots/sparsity_sweep_2_thread.png)
![sparsity_t4](data_analysis/plots/sparsity_sweep_4_thread.png)
![sparsity_t8](data_analysis/plots/sparsity_sweep_8_thread.png)

The plots shown above clearly show that matrices which are more sparse
can be computed far more efficiently than dense matrices.  One interesting
note is that the tests run on sparser matrices perform better with lower
thread counts while higher thread counts outperform when
the matrix is less sparse.  These results may due to the fact that more sparse matrices
can be computed faster meaning the thread overhead is more costly on these tests.

## Conclusion



