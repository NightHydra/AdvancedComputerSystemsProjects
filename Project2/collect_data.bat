
setlocal enabledelayedexpansion

REM --- IMPORTANT: You MUST run this script as an administrator. ---
REM Ensure mlc.exe is in the same directory as this script.

set "LOGFILE_CSV=%~dp0mlc_test_results.csv"

REM Check if mlc.exe exists
if not exist "mlc.exe" (
echo ERROR: mlc.exe not found in this directory.
echo Please ensure the MLC executable is in the same folder as this script.
pause
exit /b 1
)

echo "Test Name","Metric","Value","Units" > "%LOGFILE_CSV%"
echo Starting MLC tests. Please wait...

REM --- Experiment 1: Zero-Queue Latency (Idle) ---
echo.
echo === Running Idle Latency Tests ===
for %%a in (20K, 400K, 8M, 200M) do (
echo Running idle latency for %%a buffer...
for /f "tokens=6" %%b in ('mlc.exe --idle_latency -b%%a -t10 ^| findstr /c:"Each iteration took"') do (
echo "Idle Latency","Buffer %%a","%%b","ns" >> "%LOGFILE_CSV%"
)
)

REM --- Experiment 2: Maximum Bandwidth ---
echo.
echo === Running Peak Bandwidth Tests ===
for %%a in (r, "1 -W 1", "3 -W 1") do (
set "TEST_NAME=%%a"
REM MLC output is inconsistent, so we'll adjust the findstr based on the test.
if "%%a" == "r" (
set "FIND_STR=ALL Reads"
set "TOKENS=5"
) else if "%%a" == "1 -W 1" (
set "FIND_STR=1:1 Reads-Writes"
set "TOKENS=5"
) else if "%%a" == "3 -W 1" (
set "FIND_STR=3:1 Reads-Writes"
set "TOKENS=5"
)
echo Running peak bandwidth test for !TEST_NAME!...
for /f "tokens=!TOKENS!" %%b in ('mlc.exe --peak_injection_bandwidth -R %%a ^| findstr /c:"!FIND_STR!"') do (
echo "Maximum Bandwidth","!TEST_NAME!","%%b","MB/s" >> "%LOGFILE_CSV%"
)
)

REM --- Experiment 3: Throughput-Latency Trade-off (Loaded Latency) ---
echo.
echo === Running Throughput-Latency Sweep ===
echo Running loaded latency sweep (this may take a few minutes)...
for /f "tokens=2,4" %%a in ('mlc.exe --loaded_latency -T -b100M -k ^| findstr "Throughput:"') do (
echo "Loaded Latency","%%a","%%b","ns" >> "%LOGFILE_CSV%"
)

echo.
echo All tests are complete.