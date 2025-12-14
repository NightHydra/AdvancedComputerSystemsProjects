@echo off
setlocal enabledelayedexpansion

set grain_options=COURSE_GRAINED FINE_GRAINED

set rw_ratio_options=0 70 100
set table_size_options=10000 100000 1000000
set num_thread_options=1 2 4 8 16

echo Beginning Data Collection For All Builds

IF NOT EXIST "data_analysis/raw_data" (
    MKDIR "data_analysis/raw_data"
)

for %%g in (%grain_options%) do (
    for %%r in (%rw_ratio_options%) do (
        for %%t in (%table_size_options%) do (
            for %%n in (%num_thread_options%) do (
                cmake-build-debug\%%g_%%r_%%t_%%n.exe --print_header > data_analysis/raw_data/%%g_%%r_%%t_%%n.csv
                for /L %%I in (1,1,4) do (
                    cmake-build-debug\%%g_%%r_%%t_%%n.exe >> data_analysis/raw_data/%%g_%%r_%%t_%%n.csv
                )
            )
        )
    )
)