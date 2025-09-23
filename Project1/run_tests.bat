@echo off

echo average cycles, average time, average clk frequency > output.txt

echo ===== Running Non-Vectorized Tests (STREAM, REDUCE, MULTIPLY) ===== >> output.txt
cmake-build-release-mingw\sample_build.exe -kernel STREAM >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\sample_build.exe -kernel REDUCE >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\sample_build.exe -kernel MULTIPLY >> output.txt 2>&1
echo. >> output.txt
echo ===== Running Vectorized Tests ===== >> output.txt
cmake-build-release-mingw\vectorized_build.exe -kernel STREAM >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\vectorized_build.exe -kernel REDUCE >> output.txt 2>&1
echo. >> output.txt
cmake-build-release-mingw\vectorized_build.exe -kernel MULTIPLY >> output.txt 2>&1
echo. >> output.txt