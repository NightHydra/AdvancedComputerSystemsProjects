import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import copy as cp
import plotting_code as pc

DATA_FILES_BASE_PATH = "data_analysis/raw_data"

DATA_TABLE_BASE_PATH = "data_analysis/tables"

def generate_size_tables(f, target_memory_size, structure):
    '''
    :param f: The filehandler to write to
    :return: None
    '''

    sizes = [16, 64, 256, 1024, 2048, 4096]

    md_table = "| "

    for s in sizes:
        md_table += f"|{s}"
    md_table += "|\n"
    md_table += "|" + "---|"*7 + "\n"

    for srow in sizes:
        md_table += f"|{srow}|"
        for scol in sizes:
            # Given the target size compute which value of k gives the best esitmate of that size
            target_k = target_memory_size/(8*(srow+scol))
            closest_value = min(sizes, key=lambda x: abs(x - target_k))

            data = fetch_data_from_file(DATA_FILES_BASE_PATH + "/size_sweep/"+
                                        f"{structure}_sizesweep_{srow}_{closest_value}_{scol}.csv", "GFLOPS")

            md_table += f"(k = {closest_value}) : {data[0]:.2f}|"
        md_table += "\n"
    md_table += "\n"
    f.write(md_table)



def fetch_data_from_file(file_to_read, metrics):

    datacol = pd.read_csv(file_to_read, delimiter=",").astype(float)[metrics]

    if (metrics == "GFLOPS"):
        datacol /= 1e9

    return [datacol.mean(), datacol.std()]

def get_means_from_list_of_files_2x(default_filename : str, primx_replacements : list[str], metricname : str,
                                 secx_replacements : list[str] = None, secx_first : bool =False):
    """
    :param default_filename: The base filename to read from
    :param primx_replacements: The primary axis to collect data from
    :param secx_replacements: The secondary unit to collect data from
    :return: A 3d list - First list is by the primary data axis, first inner list is based on secondary unit,
         and most inner is [mean, stdev]
    """

    all_means = []
    all_stds = []

    for secx in secx_replacements:
        primary_means = []
        primary_stds = []
        for primx in primx_replacements:
            if secx_first == True:
                file_to_read = default_filename.format(secx, primx)
            else:
                file_to_read = default_filename.format(primx, secx)

            datapoint = fetch_data_from_file(file_to_read, metricname)
            primary_means.append(datapoint[0])
            primary_stds.append(datapoint[1])
        all_means.append(primary_means)
        all_stds.append(primary_stds)
    return [all_means, all_stds]


def main():

    ## BASELINE PLOTS
    primx_vals = ["gemm", "spmm"]
    secx_vals = ["novectorize", "vectorize"]

    data = get_means_from_list_of_files_2x(DATA_FILES_BASE_PATH+"/simd_threading/{}_{}_nothreading.csv", primx_vals, "GFLOPS", secx_vals, False)

    pc.plot_double_bar_graph(data[0], data[1], ["GEMM", "SPMM"],["NO SIMD", "SIMD"],  "Scalar vs SIMD Baselines",
                             "GFLOPS", "Scalar vs SIMD Baselines", "scalar_simd_baselines.png")


    ## Comparing SIMD vs Threading vs Both
    primx_vals = ["gemm", "spmm"]
    secx_vals = ["novectorize_nothreading", "vectorize_nothreading", "novectorize_threading", "vectorize_threading"]

    data = get_means_from_list_of_files_2x(DATA_FILES_BASE_PATH+"/simd_threading/{}_{}.csv", primx_vals, "GFLOPS", secx_vals, False)
    pc.plot_double_bar_graph(data[0], data[1], ["GEMM", "SPMM"],["Neither", "Vectorized", "Threading", "Both"],  "Scalar vs SIMD Baselines",
                             "GFLOPS", "Scalar vs SIMD vs Threading", "scalar_threading.png")

    ## Comparing thread number
    primx_vals = ["1", "2", "4", "8"]
    secx_vals = ["gemm", "spmm"]

    data = get_means_from_list_of_files_2x(DATA_FILES_BASE_PATH+"/simd_threading/{}_threading{}.csv", primx_vals, "GFLOPS",
                                           secx_vals, True)
    pc.plot_double_bar_graph(data[0], data[1],  primx_vals, ["GEMM", "SPMM"],   "Number of Threads",
                             "GFLOPS", "Effects of Thread Count on Performance", "thread_sweep.png")

    ## Comparing Density
    primx_vals = ["0.1", "0.5", "1", "2", "5", "10", "20", "50"]
    secx_vals = ["gemm", "spmm"]


    data1 = get_means_from_list_of_files_2x(DATA_FILES_BASE_PATH+"/density_sweep/{}_density_{}_thread1.csv", primx_vals,
                                            "GFLOPS", secx_vals, True)

    data2 = get_means_from_list_of_files_2x(DATA_FILES_BASE_PATH+"/density_sweep/{}_density_{}_thread2.csv", primx_vals,
                                            "GFLOPS", secx_vals, True)
    data4 = get_means_from_list_of_files_2x(DATA_FILES_BASE_PATH+"/density_sweep/{}_density_{}_thread4.csv", primx_vals,
                                            "GFLOPS", secx_vals, True)
    data8 = get_means_from_list_of_files_2x(DATA_FILES_BASE_PATH+"/density_sweep/{}_density_{}_thread8.csv", primx_vals,
                                            "GFLOPS", secx_vals, True)


    pc.plot_double_bar_graph(data1[0], data1[1], primx_vals, ["GEMM", "SPMM"], "Density and Structure",
                             "GFLOPS", "Effects of Density Sweep using 1 thread", "sparsity_sweep_1_thread.png")

    pc.plot_double_bar_graph(data2[0], data2[1], primx_vals, ["GEMM", "SPMM"], "Density and Structure",
                             "GFLOPS", "Effects of Density Sweep using 2 threads", "sparsity_sweep_2_thread.png")

    pc.plot_double_bar_graph(data4[0], data4[1], primx_vals, ["GEMM", "SPMM"], "Density and Structure",
                             "GFLOPS", "Effects of Density Sweep using 4 thread", "sparsity_sweep_4_thread.png")

    pc.plot_double_bar_graph(data8[0], data8[1], primx_vals, ["GEMM", "SPMM"], "Density and Structure",
                             "GFLOPS", "Effects of Density Sweep using 8 thread", "sparsity_sweep_8_thread.png")


    # ----- SIZE SWEEEP ---- #
    memsize_range = [32000, 2000000, 20000000, 60000000]

    with open (DATA_TABLE_BASE_PATH+"/sizes_sweep.txt", "w") as f:
        for s in memsize_range:
            generate_size_tables(f, s, "gemm")
            generate_size_tables(f, s, "spmm")



main()


