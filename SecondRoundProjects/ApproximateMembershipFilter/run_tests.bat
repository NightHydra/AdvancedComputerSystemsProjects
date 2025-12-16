@echo off
setlocal

:: --- Configuration ---
set "BUILD_DIR=build"
set "DATA_DIR=data_analysis\raw_data"
set "TARGETS=test_xor_filter test_cuckoo_filter test_quotient_filter test_blocked_bloom_filter"
:: ---------------------

echo --- Filter Testing Automation Script ---

:: 1. Setup Data Directory
echo 1. Setting up data directory: %DATA_DIR%
if not exist "%DATA_DIR%" (
    mkdir "%DATA_DIR%"
)

:: 2. Build the Targets using CMake
echo 2. Building all filter targets in %BUILD_DIR%...
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

:: Change to the build directory
pushd "%BUILD_DIR%"

:: Check if CMake is available
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: CMake is not found in your system's PATH. Please install it or add it to PATH.
    popd
    exit /b 1
)

:: Configure CMake
echo Configuring CMake...
cmake ..
if %errorlevel% neq 0 (
    echo Error: CMake configuration failed.
    popd
    exit /b 1
)

:: Build all targets (using the default generator, e.g., Visual Studio or MinGW/NMake)
echo Building targets...
cmake --build .
if %errorlevel% neq 0 (
    echo Error: Build failed.
    popd
    exit /b 1
)

:: Return to the root directory
popd

:: 3. Execute Tests and Collect Data
echo 3. Executing tests and collecting raw data...

for %%t in (%TARGETS%) do (
    set "EXECUTABLE=%BUILD_DIR%\%%t.exe"
    set "OUTPUT_FILE=%DATA_DIR%\%%t.txt"

    echo     -> Running %%t...

    :: Check if the executable exists
    if not exist "%EXECUTABLE%" (
        echo     !!! WARNING: %EXECUTABLE% not found. Skipping.
        continue
    )

    :: Execute the target and redirect its standard output to the file
    "%EXECUTABLE%" > "%OUTPUT_FILE%"
    if %errorlevel% neq 0 (
        echo     !!! WARNING: %%t failed to run. Check %OUTPUT_FILE% for partial output/errors.
    ) else (
        echo     -> Data collected in %OUTPUT_FILE%
    )
)

echo --- Automation Complete ---
echo All raw data files are located in the %DATA_DIR% folder.

endlocal