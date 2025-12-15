//
// Created by Alek on 12/14/2025.
//

#ifndef MATRIX_H
#define MATRIX_H
#include <iostream>
#include <thread>

#ifndef TILESIZE
#define TILESIZE (4)
#endif

#ifndef NUMTHREADS
#define NUMTHREADS (1)
#endif

template <class NumType>
class SparseMatrix;

template <class NumType>
class Matrix {
public:
    Matrix(unsigned int m, unsigned int n);
    ~Matrix();
    void multiply(const Matrix& other, Matrix& res);
    bool assign_values_using_vector(const std::vector<std::vector<NumType>>& v);
    void zero_matrix();
    void randomize_matrix(std::mt19937 gen, std::uniform_real_distribution<NumType>& dist,
        float sparsity = 50);
    void print_matrix(std::ostream& ostr);

private:
    void multiply_small(const Matrix& other, Matrix& res, unsigned int starti,
        unsigned int startj, unsigned int startk);
    void compute_rows(const Matrix& other, Matrix& res, unsigned int startr, unsigned int endr);

    NumType ** matrix_;
    unsigned int m_, n_;

    std::uniform_real_distribution<NumType> percentage_dist;

    friend class SparseMatrix<NumType>;
};

template <class NumType>
Matrix<NumType>::Matrix(unsigned int m, unsigned int n)
{
    matrix_ = new NumType*[m];
    for (unsigned int i = 0; i < m; i++)
    {
        matrix_[i] = new NumType[n];
    }
    m_ = m;
    n_ = n;

    // Distribution for sparsity
    percentage_dist = std::uniform_real_distribution<NumType>(0.0, 100.0);
}

template <class NumType>
Matrix<NumType>::~Matrix()
{
    for (unsigned int i = 0; i < m_; i++)
    {
        delete [] matrix_[i];
    }
    delete [] matrix_;
}

template<class NumType>
void Matrix<NumType>::multiply(const Matrix& other, Matrix &res)
{
#if NUMTHREADS > 1
    // Calculate how many threads to use
    std::thread * threads[NUMTHREADS];
    unsigned int rows_per_thread = m_ / NUMTHREADS;
    for (unsigned int threadnum = 0; threadnum < NUMTHREADS-1; ++threadnum)
    {
        threads[threadnum] = new std::thread(Matrix<NumType>::compute_rows, this, std::ref(other),
            std::ref(res), threadnum*rows_per_thread, (threadnum+1)*rows_per_thread);
    }
    threads[NUMTHREADS-1] = new std::thread(Matrix<NumType>::compute_rows, this, std::ref(other), std::ref(res),
        (NUMTHREADS-1)*rows_per_thread, m_);

    for (unsigned int threadnum = 0; threadnum < NUMTHREADS; ++threadnum)
    {
        threads[threadnum]->join();
        delete threads[threadnum];
    }
#endif

#if NUMTHREADS == 1
    // USE GEMM algorithm for dense multiplication as an inner product approach
    for (unsigned int i = 0; i < m_; i += TILESIZE)
    {
        for (unsigned int j = 0; j < other.n_; j += TILESIZE)
        {
            for (unsigned int k = 0; k < n_; k += TILESIZE)
            {
#if TILESIZE == 1
                res.matrix_[i][j] += matrix_[i][k] * other.matrix_[k][j];
#endif
#if TILESIZE > 1
                multiply_small(other, res, i, j, k);
#endif
            }
        }
    }
#endif

}

template <class NumType>
void Matrix<NumType>::multiply_small(const Matrix& other, Matrix& res,
    unsigned int starti, unsigned int startj, unsigned int startk)
{
    for (unsigned int i = starti; i < starti+TILESIZE; ++i)
    {
        for (unsigned int j = startj; j < startj+TILESIZE; ++j)
        {
            for (unsigned int k = startk; k < startk+TILESIZE; ++k)
            {
                res.matrix_[i][j] += matrix_[i][k] * other.matrix_[k][j];
            }
        }
    }
}

template <class NumType>
void Matrix<NumType>::compute_rows(const Matrix& other, Matrix& res, unsigned int startr, unsigned int endr)
{
    for (unsigned int i = startr; i < endr; i += TILESIZE)
    {
        for (unsigned int j = 0; j < other.n_; j += TILESIZE)
        {
            for (unsigned int k = 0; k < n_; k += TILESIZE)
            {
#if TILESIZE == 1
                res.matrix_[i][j] += matrix_[i][k] * other.matrix_[k][j];
#endif
#if TILESIZE > 1
                multiply_small(other, res, i, j, k);
#endif
            }
        }
    }
}

template <class NumType>
bool Matrix<NumType>::assign_values_using_vector(const std::vector<std::vector<NumType>>& v)
{
    if (m_ != v.size() || n_ != v[0].size()) return false;
    for (unsigned int i = 0; i < m_; i++)
    {
        for (unsigned int j = 0; j < n_; j++)
        {
            matrix_[i][j] = v[i][j];
        }
    }
    return true;
}

template<class NumType>
void Matrix<NumType>::zero_matrix()
{
    for (unsigned int i = 0; i < m_; i++)
    {
        for (unsigned int j = 0; j < n_; j++)
        {
            matrix_[i][j] = 0;
        }
    }
}

template<class NumType>
void Matrix<NumType>::randomize_matrix(std::mt19937 gen, std::uniform_real_distribution<NumType>& dist,
    float sparsity)
{
    for (unsigned int i = 0; i < m_; i++)
    {
        for (unsigned int j = 0; j < n_; j++)
        {
            if (percentage_dist(gen) > sparsity)
            {
                matrix_[i][j] = 0;
            }
            else
            {
                matrix_[i][j] = dist(gen);
            }
        }
    }
}

template<class NumType>
void Matrix<NumType>::print_matrix(std::ostream& ostr)
{
    for (unsigned int i = 0; i < m_; i++)
    {
        ostr << "[";
        for (unsigned int j = 0; j < n_; j++)
        {
            ostr << matrix_[i][j] << " ";
        }
        ostr << "]" << std::endl;
    }
    ostr << std::endl;
}




#endif //MATRIX_H
