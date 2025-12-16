#include <iostream>
#include <vector>
#include <random>

#include "Matrix.h"
#include "SparseMatrix.h"
#include "myperfmodule.h"

#define NUM_TESTS (10)

#define M1_DEFAULT_WIDTH (1024)
#define M1_DEFAULT_HEIGHT (1024)
#define M2_DEFAULT_WIDTH (2048)
#define DEFAULT_SPARSITY (5.0F)

void run_tests(unsigned int matrix1_width, unsigned int matrix1_height, unsigned int matrix2_width, float sparsity)
{
    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()

    std::uniform_real_distribution<double> dist(1.0, 2.0);

    Matrix<double> m1(matrix1_height,matrix1_width);
    Matrix<double> m2(matrix1_width,matrix2_width);
    Matrix<double> m3(matrix1_height,matrix2_width);

    std::cout << "Cycle Count,Elpased Time(s),GFLOPS" << std::endl;

    for (unsigned int i = 0; i < NUM_TESTS; ++i)
    {
        // Generate the matrices
        m1.randomize_matrix(gen, dist, sparsity);
        m2.randomize_matrix(gen, dist, sparsity);
        m3.zero_matrix();
#ifdef SPMM
        SparseMatrix<double> sp_m1(m1);
#endif

        start_performace_measurement();

#ifndef SPMM
        m1.multiply(m2, m3);
#endif
#ifdef SPMM
        sp_m1.multiply(m2, m3);
#endif
        perf_t results = *end_performace_measurement();

        double flop = (double)(matrix1_height) * matrix1_width * matrix2_width;

        std::cout << results.elapsed_cycles << ',' << results.elapsed_time << "," << flop / results.elapsed_time << std::endl;
    }
}

int main(int argc, char* argv[])
{
    unsigned int matrix1_width = M1_DEFAULT_WIDTH;
    unsigned int matrix1_height = M1_DEFAULT_HEIGHT;
    unsigned int matrix2_width = M2_DEFAULT_WIDTH;
    float sparsity = DEFAULT_SPARSITY;

    for (unsigned int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--m1_width") == 0)
        {
            matrix1_width = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--m1_height") == 0)
        {
            matrix1_height = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--m2_width") == 0)
        {
            matrix2_width = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--sparsity") == 0)
        {
            sparsity = atof(argv[++i]);
        }
    }

    run_tests(matrix1_width, matrix1_height, matrix2_width, sparsity);

    return 0;
}