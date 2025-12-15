//
// Created by Alek on 12/14/2025.
//

#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H

#include "Matrix.h"

/**
 * @brief A class to make a spare matrix in CSR format.
 * @note : Source - https://en.wikipedia.org/wiki/Sparse_matrix#Compressed_sparse_row_(CSR,_CRS_or_Yale_format)
 * @tparam NumType A number type
 */
template <class NumType>
class SparseMatrix {
public:
    SparseMatrix(const Matrix<NumType>& other);
    ~SparseMatrix();
    void multiply(const Matrix<NumType>& other, Matrix<NumType>& res);
    void compute_rows(const Matrix<NumType>& other, Matrix<NumType>& res, unsigned int starti,
        unsigned int endi);

private:
    /**
     * @brief the values of the spare matrix stored in CSR order
     */
    NumType * v_;
    /**
     * @brief the column index of the kth value read when reading the matrix
     * from left to right, top to bottom.
     */
    unsigned int * col_index_;
    /**
     * @brief (index_to_start_each_row[k] is the first index of v which is in row k.
     */
    unsigned int * index_to_start_each_row_;
    unsigned int num_values_;
    unsigned int m_;
    unsigned int n_;
};

template <class NumType>
SparseMatrix<NumType>::~SparseMatrix()
{
    delete [] v_;
    delete [] col_index_;
    delete [] index_to_start_each_row_;
}

template <class NumType>
SparseMatrix<NumType>::SparseMatrix(const Matrix<NumType>& other)
{
    // Copy some matrix constants
    m_ = other.m_;
    n_ = other.n_;
    num_values_ = 0;

    // First count up all the nonzero values
    for (unsigned int i = 0; i < m_; ++i)
    {
        for (unsigned int j = 0; j < n_; ++j)
        {
            // Assume 0 if its close, will never have a float of 0
            if (other.matrix_[i][j] > 0.00005) ++num_values_;
        }
    }

    // Now go through and generate the arrays
    v_ = new NumType[num_values_];
    col_index_ = new unsigned int[num_values_];
    index_to_start_each_row_ = new unsigned int[m_];

    // And copy all the values over the spare rep
    unsigned int nums_placed_counter = 0;

    index_to_start_each_row_[0] = 0;
    for (unsigned int i = 0; i < m_; ++i)
    {
        for (unsigned int j = 0; j < n_; ++j)
        {
            // Assume 0 if its close, will never have a float of 0
            if (other.matrix_[i][j] > 0.00005)
            {
                v_[nums_placed_counter] = other.matrix_[i][j];
                col_index_[nums_placed_counter++] = j;
            }
        }
        index_to_start_each_row_[i+1] = nums_placed_counter;
    }
}

template <class NumType>
void SparseMatrix<NumType>::multiply(const Matrix<NumType>& other, Matrix<NumType>& res)
{
#if NUMTHREADS == 1
    for (unsigned int i = 0; i<m_; i++)
    {
        for (unsigned int j = 0; j<other.n_; ++j)
        {
            for (unsigned int k = index_to_start_each_row_[i]; k<index_to_start_each_row_[i+1]; k++)
            {
                res.matrix_[i][j] += v_[k] * other.matrix_[col_index_[k]][j];
            }
        }
    }
#endif
#if NUMTHREADS > 1
    std::thread * threads[NUMTHREADS];
    unsigned int rows_per_thread = m_ / NUMTHREADS;
    for (unsigned int threadnum = 0; threadnum < NUMTHREADS-1; ++threadnum)
    {
        threads[threadnum] = new std::thread(SparseMatrix<NumType>::compute_rows, this, std::ref(other),
            std::ref(res), threadnum*rows_per_thread, (threadnum+1)*rows_per_thread);
    }
    threads[NUMTHREADS-1] = new std::thread(SparseMatrix<NumType>::compute_rows, this, std::ref(other), std::ref(res),
        (NUMTHREADS-1)*rows_per_thread, m_);

    for (unsigned int threadnum = 0; threadnum < NUMTHREADS; ++threadnum)
    {
        threads[threadnum]->join();
        delete threads[threadnum];
    }
#endif
}

template <class NumType>
void SparseMatrix<NumType>::compute_rows(const Matrix<NumType>& other, Matrix<NumType>& res, unsigned int starti,
        unsigned int endi)
{
    for (unsigned int i = starti; i<endi; i++)
    {
        for (unsigned int j = 0; j<other.n_; ++j)
        {
            for (unsigned int k = index_to_start_each_row_[i]; k<index_to_start_each_row_[i+1]; k++)
            {
                res.matrix_[i][j] += v_[k] * other.matrix_[col_index_[k]][j];
            }
        }
    }
}






#endif //SPARSEMATRIX_H
