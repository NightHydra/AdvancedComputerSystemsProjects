@echo off
set LOGFILE="mlc_results.txt"

echo ====== Collecting Data for zero-queue-baselines ====== > %LOGFILE%
@REM
@REM echo -- Cache Zero Queue Baseline (L1 - 40KB) >> %LOGFILE%
@REM mlc --idle_latency -b40 >> %LOGFILE%
@REM
@REM echo -- Cache Zero Queue Baseline (L2 - 1MB) >> %LOGFILE%
@REM mlc --idle_latency -b1000 >> %LOGFILE%
@REM
@REM echo -- Cache Zero Queue Baseline (LLC - 16MB) >> %LOGFILE%
@REM mlc --idle_latency -b16000 >> %LOGFILE%
@REM
@REM echo -- Cache Zero Queue Baseline (DRAM - 200MB) >> %LOGFILE%
@REM mlc --idle_latency -b200000 >> %LOGFILE%
@REM
@REM echo ========== Collecting Data for Access Pattern and Stride ============= >> %LOGFILE%

@REM echo ====== Running Tests for Sequential Access ======= >> %LOGFILE%
@REM echo. >> %LOGFILE%
@REM echo -- 64 Byte Stride -- >> %LOGFILE%
@REM mlc --loaded_latency -l64 >> %LOGFILE%
@REM echo -- 256 Byte Stride -- >> %LOGFILE%
@REM mlc --loaded_latency -l256 >> %LOGFILE%
@REM echo -- 1024 Byte Stride -- >> %LOGFILE%
@REM mlc --loaded_latency -l1024 >> %LOGFILE%
@REM
@REM echo. >> LOGFILE
@REM echo ====== Running Tests for Random Access ======= >> %LOGFILE%
@REM echo. >> %LOGFILE%
@REM echo -- 64 Byte Stride -- >> %LOGFILE%
@REM mlc --loaded_latency -l64 -U >> %LOGFILE%
@REM echo -- 256 Byte Stride -- >> %LOGFILE%
@REM mlc --loaded_latency -l256 -U >> %LOGFILE%
@REM echo -- 1024 Byte Stride -- >> %LOGFILE%
@REM mlc --loaded_latency -l1024 -U >> %LOGFILE%

echo ========== Collecting Data for Differing R/W sweeps ============= >> %LOGFILE%

echo -- 1:1 Read/Write Experiment -- >> %LOGFILE%
mlc --idle_latency -W2 >> %LOGFILE%
echo -- 2:1 Read/Write Experiment -- >> %LOGFILE%
mlc --idle_latency -W2 >> %LOGFILE%
echo -- 3:1 Read/Write Experiment -- >> %LOGFILE%
mlc --idle_latency -W3 >> %LOGFILE%