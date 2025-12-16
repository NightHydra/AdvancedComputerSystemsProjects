@echo off

echo --- Running Zero Copy Tests ---

IF NOT EXIST "data_analysis/raw_data/zero_copy" (
    MKDIR "data_analysis/raw_data/zero_copy"
)

echo Zero-Copy Cycles,Normal-Copy Cyles,Zero-Copy CMP Result,Normal-Copy CMP Result > data_analysis/raw_data/zero_copy/small_output.csv
echo Zero-Copy Cycles,Normal-Copy Cyles,Zero-Copy CMP Result,Normal-Copy CMP Result > data_analysis/raw_data/zero_copy/medium_output.csv
echo Zero-Copy Cycles,Normal-Copy Cyles,Zero-Copy CMP Result,Normal-Copy CMP Result > data_analysis/raw_data/zero_copy/large_output.csv
echo Zero-Copy Cycles,Normal-Copy Cyles,Zero-Copy CMP Result,Normal-Copy CMP Result > data_analysis/raw_data/zero_copy/extra_large_output.csv

for /L %%i in (1, 1, 9) do (
    cmake-build-release\ZeroCopy.exe testfiles/small_file.txt testfiles/small_file2.txt >> data_analysis/raw_data/zero_copy/small_output.csv

    cmake-build-release\ZeroCopy.exe testfiles/medium_file.txt testfiles/medium_file2.txt >> data_analysis/raw_data/zero_copy/medium_output.csv

    cmake-build-release\ZeroCopy.exe testfiles/large_file.txt testfiles/large_file2.txt >> data_analysis/raw_data/zero_copy/large_output.csv

    cmake-build-release\ZeroCopy.exe testfiles/extra_large_file.txt testfiles/extra_large_file2.txt >> data_analysis/raw_data/zero_copy/extra_large_output.csv
)

