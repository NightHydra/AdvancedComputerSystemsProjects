@echo off
set LOGFILE="mlc_results.txt"

echo === Running MLC tests for Project 2 ===
echo. >> %LOGFILE%

REM --- Experiment 1: Zero-Queue Latency (Idle) ---
REM Measures the latency of the memory hierarchy with no other load.
echo --- Measuring Zero-Queue Latencies (Idle) ---
echo.
echo === Experiment 1: Zero-Queue Latency (Idle) === >> %LOGFILE%
REM Latency for L1, L2, L3 caches. Buffer size must be smaller than cache size.
echo Running L1/L2/L3 cache latency test...
mlc.exe -idle_latency -b20K -t10 >> %LOGFILE%
mlc.exe -idle_latency -b400K -t10 >> %LOGFILE%
mlc.exe -idle_latency -b8M -t10 >> %LOGFILE%
REM Latency for DRAM. Buffer size should be larger than last-level cache.
echo Running DRAM latency test...
mlc.exe -idle_latency -b200M >> %LOGFILE%
echo. >> %LOGFILE%

REM --- Experiment 2: Maximum Bandwidth ---
REM Measures the peak data transfer rate for various read/write ratios.
echo --- Measuring Maximum Bandwidths ---
echo.
echo === Experiment 2: Maximum Bandwidth === >> %LOGFILE%
echo Measuring All Reads bandwidth...
mlc.exe -peak_injection_bandwidth -r >> %LOGFILE%
echo Measuring 1:1 Reads-Writes bandwidth...
mlc.exe -peak_injection_bandwidth -R 1 -W 1 >> %LOGFILE%
echo Measuring 3:1 Reads-Writes bandwidth...
mlc.exe -peak_injection_bandwidth -R 3 -W 1 >> %LOGFILE%
echo. >> %LOGFILE%

REM --- Experiment 3: Throughput-Latency Trade-off (Loaded Latency) ---
REM Measures how latency increases with memory load.
echo --- Measuring Loaded Latencies (Throughput-Latency Sweep) ---
echo.
echo === Experiment 3: Loaded Latency Sweep === >> %LOGFILE%
REM This test will take a few minutes as it varies the load.
REM The -T flag enables a detailed report of latency at various throughput levels.
echo Running loaded latency sweep...
mlc.exe -loaded_latency -T -b100M -k >> %LOGFILE%
echo. >> %LOGFILE%

echo All tests complete. Results are saved in %LOGFILE%
pause