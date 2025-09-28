@echo off

set "kernel_options=STREAM REDUCE MULTIPLY"

set vectorize_options=NO_VECTORIZE VECTORIZE
set float_type_options=float double
set working_set_options=L1 L2 LLC DRAM
set stride_len_options=1 2 4 8
set align_options=NO_ALIGN ALIGN_ARRAYS

echo Beginning Data Collection For All Builds

for %%v in (%vectorize_options%) do (
    for %%f in (%float_type_options%) do (
        for %%w in (%working_set_options%) do (
            for %%s in (%stride_len_options%) do (
                for %%k in (%kernel_options%) do (
                    for %%a in (%align_options%) do (
                        IF NOT EXIST "data_analysis/raw_data/%%v_%%f_%%w_%%s_%%a" (
                            MKDIR "data_analysis/raw_data/%%v_%%f_%%w_%%s_%%a"
                        )
                        cmake-build-release-mingw\%%v_%%f_%%w_%%s_%%a.exe -kernel %%k > data_analysis/raw_data/%%v_%%f_%%w_%%s_%%a/%%k.csv
                    )
                )
            )
        )
    )
)