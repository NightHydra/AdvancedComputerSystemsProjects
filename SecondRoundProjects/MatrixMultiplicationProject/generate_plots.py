import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import copy as cp
import plotting_code as pc

DATA_FILES_BASE_PATH = "data_analysis/raw_data"

def fetch_data_from_file(file_to_read, metrics):

    datacol = pd.read_csv(file_to_read, delimiter=",").astype(float)[metrics]


    return [datacol.mean(), datacol.std()]

def get_means_from_list_of_files_2x(default_filename : str, primx_replacements : list[str], metricname,
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

    for primx in primx_replacements:
        primary_means = []
        primary_stds = []
        for secx in secx_replacements:
            if secx_first == True:
                file_to_read = default_filename.format(secx, primx)
            else:
                file_to_read = default_filename.format(primx, secx)

            datapoint = fetch_data_from_file(file_to_read, metricname)
            primary_means.append(datapoint[0])
            primary_stds.append(datapoint[0])
        all_means.append(primary_means)
        all_stds.append(primary_stds)
    return [all_means, all_stds]


def main():

    ## BASELINE PLOTS
    primx_vals = ["novectorize", "vectorize"]
    secx_vals = ["gemm", "spmm"]

    data = get_means_from_list_of_files_2x(DATA_FILES_BASE_PATH+"/simd_threading/{}_{}_nothreading.csv", primx_vals, "GFLOPS", secx_vals, True)
    print (data)

    ## Comparing SIMD vs Threading vs Both
    primx_vals = ["novectorize_nothreading", "vectorize_nothreading", "novectorize_threading", "vectorize_threading"]
    secx_vals = ["gemm", "spmm"]

    data = get_means_from_list_of_files_2x(DATA_FILES_BASE_PATH+"/simd_threading/{}_{}.csv", primx_vals, "GFLOPS", secx_vals, True)

    ## Comparing thread number
    primx_vals = ["1", "2", "4", "8"]
    secx_vals = ["gemm", "spmm"]

    data = get_means_from_list_of_files_2x(DATA_FILES_BASE_PATH+"/simd_threading/{}_threading{}.csv", primx_vals, "GFLOPS",
                                           secx_vals, True)

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
                             "Throughput (GFLOPS)", "Effects of Density Sweep using 1 thread", "sparsity_sweep_1_thread.png")

    pc.plot_double_bar_graph(data2[0], data2[1], primx_vals, ["GEMM", "SPMM"], "Density and Structure",
                             "Throughput (GFLOPS)", "Effects of Density Sweep using 2 threads", "sparsity_sweep_2_thread.png")

    pc.plot_double_bar_graph(data4[0], data4[1], primx_vals, ["GEMM", "SPMM"], "Density and Structure",
                             "Throughput (GFLOPS)", "Effects of Density Sweep using 4 thread", "sparsity_sweep_4_thread.png")

    pc.plot_double_bar_graph(data8[0], data8[1], primx_vals, ["GEMM", "SPMM"], "Density and Structure",
                             "Throughput (GFLOPS)", "Effects of Density Sweep using 8 thread", "sparsity_sweep_8_thread.png")


main()


