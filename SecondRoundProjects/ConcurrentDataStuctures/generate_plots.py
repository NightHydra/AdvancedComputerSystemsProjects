import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import copy as cp

print ("STARTING")

DATA_FILES_BASE_PATH = "data_analysis/raw_data"
PLOT_FILES_PATH = "data_analysis/plots/"

lock_grain_options = ["COURSE_GRAINED", "FINE_GRAINED"]
rw_ratio_options = ["0", "70", "100"]
datset_size_options = ["10000", "100000", "1000000"]
num_threads_options = ["1", "2", "4", "8", "16"]

option_names = {"lock" : lock_grain_options, "rw" : rw_ratio_options, "size" : datset_size_options,
                "thread" : num_threads_options}

option_axis_names = {"lock" : "Lock Grain Type", "rw" : "Read-Write Ratio",
                     "size" : "Initial Size of Table", "thread": "Number of Threads Used"}

control_options = {"lock" : "COURSE_GRAINED", "rw" : "70", "size" : "100000", "thread" : "4"}

def get_option_list_from_name(name : str):
    return option_names[name]
def get_xaxis_label_from_name(name: str):
    return option_axis_names[name]

def fetch_data_from_file(options, metrics):
    file_to_read = "data_analysis/raw_data/{}_{}_{}_{}.csv".format(*options.values())

    datacol = pd.read_csv(file_to_read, delimiter=",").astype(float)[metrics]


    return datacol.mean()

def read_data_for_plot(primary_x : str, primary_y : str, secondary_x : str,
                       default_overrides : dict[str, str] = None):

    primx_list = get_option_list_from_name(primary_x)
    secondx_list = get_option_list_from_name(secondary_x)

    current_data_control_options = cp.copy(control_options)

    if default_overrides:
        for (override_field,override_option) in default_overrides.items():
            current_data_control_options[override_field] = override_option

    full_means = []
    full_stds = []

    for secx in secondx_list:

        current_means = []
        current_stds = []

        current_data_control_options[secondary_x] = secx

        for primx in primx_list:

            current_data_control_options[primary_x] = primx

            filedata = fetch_data_from_file(current_data_control_options, primary_y)

            current_means.append(filedata)
        full_means.append(current_means)
        full_stds.append(current_stds)
    return full_means

def plot_double_line_graph(means : list[list[float]], x_cat: str, x_cat2 : str, perf_metric: str, plottitle: str, savename: str):
    """
    Plots multiple lines with error bars from 2D arrays of means and stds.

    Parameters:
        means (float): 2D array (n_lines, n_points) of mean values.
        x_cat (str): Category for the x-axis.
        x_cat2 (str): Subcategories
        perf_metric (str): Label for the y-axis.
        plottitle (str): Title of the plot.
        savename (str): File name to save the plot.
    """
    n_lines, n_points = [len(means), len(means[0])]
    x = np.arange(n_points)

    plt.figure(figsize=(8, 5))

    for i in range(n_lines):
        plt.errorbar(
            x,
            means[i],
            fmt='-o',
            capsize=5,
            label=get_option_list_from_name(x_cat2)[i]
        )

    plt.xticks(x, get_option_list_from_name(x_cat), rotation=45)
    plt.xlabel(get_xaxis_label_from_name(x_cat))
    plt.ylabel(perf_metric)
    plt.title(plottitle)
    plt.legend()
    plt.tight_layout()

    plt.savefig(PLOT_FILES_PATH+savename, dpi=300)
    plt.close()

def main():

    #### Ratio Effects on Cycles/Operation

    ratio_plot_title = "Impact of worldload Ratio on Throughput"
    ratio_plot_savename = "rw_ratio_line_plot"

    data_arr = read_data_for_plot("rw", " Cycles Per Operation", "lock")

    plot_double_line_graph(data_arr, "rw", "lock", "Cycles/Operation",
                           ratio_plot_title, ratio_plot_savename)

    ### Effects of dataset size on throughput

    size_plot_title = "Impact of hashmap size on Throughput"
    size_plot_savename = "set_size_line_plot"

    data_arr = read_data_for_plot("size", " Cycles Per Operation", "lock")

    plot_double_line_graph(data_arr, "size", "lock", "Cycles/Operation",
                           size_plot_title, size_plot_savename)

    ### Effects of thread count size on throughput

    size_plot_title = "Impact of # of threads on Throughput"
    size_plot_savename = "thread_count_line_plot"

    data_arr = read_data_for_plot("thread", " Cycles Per Operation", "lock")

    plot_double_line_graph(data_arr, "thread", "lock", "Cycles/Operation",
                           size_plot_title, size_plot_savename)

    ## Generate Three plots of different workload ratios

    size_plot_title = "Impact of # of threads on read only workload"
    size_plot_savename = "thread_count_line_plot_readonly"

    data_arr = read_data_for_plot("thread", " Cycles Per Operation", "lock",
                                  default_overrides={"rw": "100"})

    plot_double_line_graph(data_arr, "thread", "lock", "Cycles/Operation",
                           size_plot_title, size_plot_savename)

    size_plot_title = "Impact of # of threads on write only workload"
    size_plot_savename = "thread_count_line_plot_writeonly"

    data_arr = read_data_for_plot("thread", " Cycles Per Operation", "lock",
                                  default_overrides={"rw": "0"})

    plot_double_line_graph(data_arr, "thread", "lock", "Cycles/Operation",
                           size_plot_title, size_plot_savename)

    size_plot_title = "Impact of # of threads on mixed workloads (70% reads)"
    size_plot_savename = "thread_count_line_plot_mixed"

    data_arr = read_data_for_plot("thread", " Cycles Per Operation", "lock",
                                  default_overrides={"rw": "70"})

    plot_double_line_graph(data_arr, "thread", "lock", "Cycles/Operation",
                           size_plot_title, size_plot_savename)

main()





