@echo off
setlocal enabledelayedexpansion

set DATAPATH = data_analysis/raw_data
set "NUMBER_OF_RUNS=10"

echo ====== Collecting Data for zero-queue-baselines
echo .
echo -- Cache Zero Queue Baseline (L1 - 40KB)

mlc --idle_latency -b40 -x20 > data_analysis/raw_data/zero_queue_l1.txt
for /L %%I in (1,1,9) do (
    mlc --idle_latency -b40 >> data_analysis/raw_data/zero_queue_l1.txt
)

echo -- Cache Zero Queue Baseline (L2 - 1MB)
mlc --idle_latency -b1000 > data_analysis/raw_data/zero_queue_l2.txt
for /L %%I in (1,1,9) do (
    mlc --idle_latency -b1000 >> data_analysis/raw_data/zero_queue_l2.txt
)

echo -- Cache Zero Queue Baseline (LLC - 16MB)
mlc --idle_latency -b16000 > data_analysis/raw_data/zero_queue_llc.txt
for /L %%I in (1,1,9) do (
    mlc --idle_latency -b16000 >> data_analysis/raw_data/zero_queue_llc.txt
)


echo -- Cache Zero Queue Baseline (DRAM - 200MB)
mlc --idle_latency -b200000 > data_analysis/raw_data/zero_queue_dram.txt
for /L %%I in (1,1,9) do (
    mlc --idle_latency -b200000 >> data_analysis/raw_data/zero_queue_dram.txt
)

echo ========== Collecting Data for Access Pattern and Stride =============

echo ====== Running Tests for Sequential Access =======
echo.
echo -- 64 Byte Stride --
mlc --loaded_latency -d0 -l64 > data_analysis/raw_data/strided64b.txt
for /L %%I in (1,1,9) do (
    mlc --loaded_latency -d0 -l64 >> data_analysis/raw_data/strided64b.txt
)
echo -- 256 Byte Stride --
mlc --loaded_latency -d0 -l256 > data_analysis/raw_data/strided256b.txt
for /L %%I in (1,1,9) do (
    mlc --loaded_latency -d0 -l256 >> data_analysis/raw_data/strided256b.txt
)
echo -- 1024 Byte Stride --
mlc --loaded_latency -d0 -l1024 > data_analysis/raw_data/strided1024b.txt
for /L %%I in (1,1,9) do (
    mlc --loaded_latency -d0 -l1024 >> data_analysis/raw_data/strided1024b.txt
)

echo.
echo ====== Running Tests for Random Access =======
echo.
echo -- 64 Byte Stride --
mlc --loaded_latency -d0 -l64 -U > data_analysis/raw_data/strided64b_ra.txt
for /L %%I in (1,1,9) do (
    mlc --loaded_latency -d0 -l64 -U >> data_analysis/raw_data/strided64b_ra.txt
)

echo -- 256 Byte Stride --
mlc --loaded_latency -d0 -l256 -U > data_analysis/raw_data/strided256b_ra.txt
for /L %%I in (1,1,9) do (
    mlc --loaded_latency -d0 -l256 -U >> data_analysis/raw_data/strided256b_ra.txt
)

echo -- 1024 Byte Stride --
mlc --loaded_latency -d0 -l1024 -U > data_analysis/raw_data/strided1024b_ra.txt
for /L %%I in (1,1,9) do (
    mlc --loaded_latency -d0 -l1024 -U >> data_analysis/raw_data/strided1024b_ra.txt
)

echo ========== Collecting Data for Differing R/W sweeps =============

echo -- 1:1 Read/Write Experiment --
mlc --loaded_latency -W5 -d0 > data_analysis/raw_data/r1w1.txt
for /L %%I in (1,1,9) do (
    mlc --loaded_latency -W5 -d0 >> data_analysis/raw_data/r1w1.txt
)

echo -- 2:1 Read/Write Experiment --
mlc --loaded_latency -W2 -d0 > data_analysis/raw_data/r2w1.txt
for /L %%I in (1,1,9) do (
    mlc --loaded_latency -W2 -d0 >> data_analysis/raw_data/r2w1.txt
)

echo -- 3:1 Read/Write Experiment --
mlc --loaded_latency -W3 -d0 > data_analysis/raw_data/r3w1.txt
for /L %%I in (1,1,9) do (
    mlc --loaded_latency -W3 -d0 >> data_analysis/raw_data/r3w1.txt
)

echo -- 100% Read Experiment --
mlc --loaded_latency -R -d0 > data_analysis/raw_data/r1.txt
for /L %%I in (1,1,9) do (
    mlc --loaded_latency -R -d0 >> data_analysis/raw_data/r1.txt
)

echo ========== Collecting Data for Intensity sweeps =============

echo -- No Delay Injection --
mlc --loaded_latency > data_analysis/raw_data/intensity_data.txt

for /L %%I in (1,1,9) do (
    mlc --loaded_latency >> data_analysis/raw_data/intensity_data.txt
)

echo ========== Working Set Size Sweep =============

set value=64
set iterations=13

mlc --loaded_latency -d0 -b64 > data_analysis/raw_data/memsweep.txt

echo Starting value: %value%
for /l %%i in (1,1,%iterations%) do (
    set /a value=value*2
    mlc --loaded_latency -d0 -b!value! >> data_analysis/raw_data/memsweep.txt
)