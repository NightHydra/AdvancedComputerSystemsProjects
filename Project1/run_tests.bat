@echo off

echo average cycles, average time, average clk frequency > output.txt

echo ========= Part 1: Baseline vs Auto-Vectorized ======== >> output.txt
echo ===== Running Non-Vectorized Tests (STREAM, REDUCE, MULTIPLY) ===== >> output.txt
echo. >> output.txt
echo L1 Cache Size >> output.txt

cmake-build-release-mingw\NO_VECTORIZE_double_L1.exe -kernel STREAM >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\NO_VECTORIZE_double_L1.exe -kernel REDUCE >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\NO_VECTORIZE_double_L1.exe -kernel MULTIPLY >> output.txt 2>&1
echo. >> output.txt

echo. >> output.txt
echo L2 Cache Size >> output.txt

cmake-build-release-mingw\NO_VECTORIZE_double_L2.exe -kernel STREAM >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\NO_VECTORIZE_double_L2.exe -kernel REDUCE >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\NO_VECTORIZE_double_L2.exe -kernel MULTIPLY >> output.txt 2>&1
echo. >> output.txt

echo. >> output.txt
echo LLC Cache Size >> output.txt

cmake-build-release-mingw\NO_VECTORIZE_double_LLC.exe -kernel STREAM >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\NO_VECTORIZE_double_LLC.exe -kernel REDUCE >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\NO_VECTORIZE_double_LLC.exe -kernel MULTIPLY >> output.txt 2>&1
echo. >> output.txt

echo. >> output.txt
echo DRAM Cache Size >> output.txt

cmake-build-release-mingw\NO_VECTORIZE_double_DRAM.exe -kernel STREAM >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\NO_VECTORIZE_double_DRAM.exe -kernel REDUCE >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\NO_VECTORIZE_double_DRAM.exe -kernel MULTIPLY >> output.txt 2>&1
echo. >> output.txt


echo ===== Running Vectorized Tests ===== >> output.txt

echo. >> output.txt
echo L1 Cache Size >> output.txt

cmake-build-release-mingw\VECTORIZE_double_L1.exe -kernel STREAM >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\VECTORIZE_double_L1.exe -kernel REDUCE >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\VECTORIZE_double_L1.exe -kernel MULTIPLY >> output.txt 2>&1
echo. >> output.txt

echo. >> output.txt
echo L2 Cache Size >> output.txt

cmake-build-release-mingw\VECTORIZE_double_L2.exe -kernel STREAM >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\VECTORIZE_double_L2.exe -kernel REDUCE >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\VECTORIZE_double_L2.exe -kernel MULTIPLY >> output.txt 2>&1
echo. >> output.txt

echo. >>output.txt
echo LLC Cache Size >> output.txt

cmake-build-release-mingw\VECTORIZE_double_LLC.exe -kernel STREAM >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\VECTORIZE_double_LLC.exe -kernel REDUCE >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\VECTORIZE_double_LLC.exe -kernel MULTIPLY >> output.txt 2>&1
echo. >> output.txt

echo. >> output.txt
echo DRAM Cache Size >> output.txt

cmake-build-release-mingw\VECTORIZE_double_DRAM.exe -kernel STREAM >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\VECTORIZE_double_DRAM.exe -kernel REDUCE >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\VECTORIZE_double_DRAM.exe -kernel MULTIPLY >> output.txt 2>&1
echo. >> output.txt