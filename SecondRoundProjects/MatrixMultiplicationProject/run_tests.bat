@echo off

echo --- BEGIN RUNNING SIMD AND THREADING TESTS

IF NOT EXIST "data_analysis/raw_data" (
    MKDIR "data_analysis/raw_data"
)

echo --- COLLECTING BASELINES ---

IF NOT EXIST "data_analysis/raw_data/simd_threading" (
    MKDIR "data_analysis/raw_data/simd_threading"
)

cmake-build-release\NO_VECTORIZE_GEMM_4_1.exe > data_analysis/raw_data/simd_threading/gemm_novectorize_nothreading.csv
cmake-build-release\NO_VECTORIZE_SPMM_4_1.exe > data_analysis/raw_data/simd_threading/spmm_novectorize_nothreading.csv

cmake-build-release\VECTORIZE_GEMM_4_1.exe > data_analysis/raw_data/simd_threading/gemm_vectorize_nothreading.csv
cmake-build-release\VECTORIZE_SPMM_4_1.exe > data_analysis/raw_data/simd_threading/spmm_vectorize_nothreading.csv

cmake-build-release\NO_VECTORIZE_GEMM_4_4.exe > data_analysis/raw_data/simd_threading/gemm_novectorize_threading.csv
cmake-build-release\NO_VECTORIZE_SPMM_4_4.exe > data_analysis/raw_data/simd_threading/spmm_novectorize_threading.csv

cmake-build-release\VECTORIZE_GEMM_4_4.exe > data_analysis/raw_data/simd_threading/gemm_vectorize_threading.csv
cmake-build-release\VECTORIZE_SPMM_4_4.exe > data_analysis/raw_data/simd_threading/spmm_vectorize_threading.csv

cmake-build-release\VECTORIZE_GEMM_4_1.exe > data_analysis/raw_data/simd_threading/gemm_threading1.csv
cmake-build-release\VECTORIZE_SPMM_4_1.exe > data_analysis/raw_data/simd_threading/spmm_threading1.csv

cmake-build-release\VECTORIZE_GEMM_4_2.exe > data_analysis/raw_data/simd_threading/gemm_threading2.csv
cmake-build-release\VECTORIZE_SPMM_4_2.exe > data_analysis/raw_data/simd_threading/spmm_threading2.csv

cmake-build-release\VECTORIZE_GEMM_4_4.exe > data_analysis/raw_data/simd_threading/gemm_threading4.csv
cmake-build-release\VECTORIZE_SPMM_4_4.exe > data_analysis/raw_data/simd_threading/spmm_threading4.csv

cmake-build-release\VECTORIZE_GEMM_4_8.exe > data_analysis/raw_data/simd_threading/gemm_threading8.csv
cmake-build-release\VECTORIZE_SPMM_4_8.exe > data_analysis/raw_data/simd_threading/spmm_threading8.csv

echo --- COLLECTING SPARSITY SWEEP ---

IF NOT EXIST "data_analysis/raw_data/density_sweep" (
    MKDIR "data_analysis/raw_data/density_sweep"
)

set density_options=0.1 0.5 1 2 5 10 20 50

for %%d in (%density_options%) do (
    cmake-build-release\NO_VECTORIZE_GEMM_4_1.exe --sparsity %%d > data_analysis/raw_data/density_sweep/gemm_density_%%d_thread1.csv
    cmake-build-release\NO_VECTORIZE_SPMM_4_1.exe --sparsity %%d > data_analysis/raw_data/density_sweep/spmm_density_%%d_thread1.csv

    cmake-build-release\NO_VECTORIZE_GEMM_4_2.exe --sparsity %%d > data_analysis/raw_data/density_sweep/gemm_density_%%d_thread2.csv
    cmake-build-release\NO_VECTORIZE_SPMM_4_2.exe --sparsity %%d > data_analysis/raw_data/density_sweep/spmm_density_%%d_thread2.csv

    cmake-build-release\NO_VECTORIZE_GEMM_4_4.exe --sparsity %%d > data_analysis/raw_data/density_sweep/gemm_density_%%d_thread4.csv
    cmake-build-release\NO_VECTORIZE_SPMM_4_4.exe --sparsity %%d > data_analysis/raw_data/density_sweep/spmm_density_%%d_thread4.csv

    cmake-build-release\NO_VECTORIZE_GEMM_4_8.exe --sparsity %%d > data_analysis/raw_data/density_sweep/gemm_density_%%d_thread8.csv
    cmake-build-release\NO_VECTORIZE_SPMM_4_8.exe --sparsity %%d > data_analysis/raw_data/density_sweep/spmm_density_%%d_thread8.csv
)
echo --- COLLECTING SIZE SWEEP DATA ---

IF NOT EXIST "data_analysis/raw_data/size_sweep" (
    MKDIR "data_analysis/raw_data/size_sweep"
)

set size_options=16 64 256 1024 2048 4096

for %%m in (%size_options%) do (
    for %%k in (%size_options%) do (
         for %%n in (%size_options%) do (
             cmake-build-release\NO_VECTORIZE_GEMM_4_1.exe --m1_height %%m --m1_width %%k --m2_width %%n > data_analysis/raw_data/size_sweep/gemm_sizesweep_%%m_%%k_%%n.csv
             cmake-build-release\NO_VECTORIZE_SPMM_4_1.exe --m1_height %%m --m1_width %%k --m2_width %%n > data_analysis/raw_data/size_sweep/spmm_sizesweep_%%m_%%k_%%n.csv
         )
    )
)
