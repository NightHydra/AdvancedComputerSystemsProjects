import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import copy as cp

print ("STARTING")

DATA_FILES_BASE_PATH = "data_analysis/raw_data"
PLOT_FILES_PATH = "data_analysis/plots/"

vec_options = ["NO_VECTORIZE", "VECTORIZE"]
float_options = ["float", "double"]
memory_level_options = ["L1", "L2", "LLC", "DRAM"]
stride_len = ["1", "2", "4", "8"]
align_options = ["NO_ALIGN", "ALIGN_ARRAYS"]
kernel_options = ["STREAM", "REDUCE", "MULTIPLY"]

option_names = {"vec" : vec_options, "float" : float_options, "mem" : memory_level_options,
                "stride" : stride_len, "align" : align_options, "kern" : kernel_options}

option_axis_names = {"vec" : "Vectorization Options", "float" : "Floating point type bitwidth",
                     "mem" : "Memory Level", "stride": "Stride Length", "align" : "Alignment Option"}

control_options = {"vec" : "NO_VECTORIZE", "float" : "double", "mem" : "L1", "stride" : "1", "align" : "NO_ALIGN",
                   "kernel" : "MULTIPLY"}

def get_option_list_from_name(name : str):
    return option_names[name]
def get_xaxis_label_from_name(name: str):
    return option_axis_names[name]

def fetch_data_from_file(options, metrics):
    file_to_read = "data_analysis/raw_data/{}_{}_{}_{}_{}/{}.csv".format(*options.values())

    print (file_to_read)

    datacol = pd.read_csv(file_to_read, delimiter=",").astype(float)[metrics]

    return [datacol.median(), datacol.std()]


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

            current_means.append(filedata[0])
            current_stds.append(filedata[1])
        full_means.append(current_means)
        full_stds.append(current_stds)
    return [full_means, full_stds]

def plot_double_bar_graph(means, stds, prim_x : str, prim_y: str, second_x : str, plot_title : str, savename : str):

    """Bar graph with standard deviation error bars.

    The structure supports multiple groups (defined by 'second_x') clustered
    for each primary category (defined by 'prim_x').

    Args:
        means (list[list[float]]): Nested list where outer index is the secondary
            group (second_x), and inner list holds mean values for each primary category (prim_x).
        stds (list[list[float]]): Nested list for standard deviations (error bars),
            matching the structure of 'means'.
        prim_x (list[str]): Labels for the primary categories on the X-axis (e.g., ['Product A', 'Product B']).
        prim_y (str): Label for the Y-axis (e.g., 'Performance Score').
        second_x (list[str]): Labels for the secondary groups (used in the legend, e.g., ['Model 1', 'Model 2']).
    """

        # --- 1. Sanity Checks and Parameter Setup ---
    if not prim_x or not second_x:
        print("Error: Primary or secondary labels are missing.")
        return

    n_prim_categories = len(means[0])
    n_groups = len(means)

    # Calculate bar width to ensure bars fit nicely within the category space (0.8 is a good total width)
    bar_width = 0.8 / n_prim_categories

    # Set the central positions of the primary category ticks on the X-axis
    ind = np.arange(n_prim_categories)
    print (ind)

    # --- 2. Create Plot and Axes ---
    fig, ax = plt.subplots(figsize=(10, 6))

    # --- 3. Plotting Logic for Grouped Bars ---
    for i in range(n_groups):
        # Calculate the offset for the current group of bars (i-th secondary group)
        # This is the crucial step: it shifts the bars so they are clustered around the tick mark 'ind'.
        # Example for 3 groups (n_groups=3):
        # i=0 (first group): offset = ind + (0 - 1) * bar_width -> ind - bar_width
        # i=1 (middle group): offset = ind + (1 - 1) * bar_width -> ind
        # i=2 (last group): offset = ind + (2 - 1) * bar_width -> ind + bar_width
        offset = ind + (i - (n_prim_categories-1) / 2) * bar_width

        # Plot the bars for the current secondary group
        ax.bar(
            offset,
            means[i],
            bar_width,
            yerr=stds[i],
            capsize=5,  # Size of the error bar caps
            label=get_option_list_from_name(second_x)[i] # Label for the legend
        )

    # --- 4. Customize Plot ---

    # Set the Y-axis label
    ax.set_ylabel(prim_y, fontsize=14)

    # Set the X-axis labels
    ax.set_xlabel(get_xaxis_label_from_name(prim_x), fontsize=14)

    # Set the X-axis tick positions to the center of the bar cluster (ind)
    ax.set_xticks(ind)
    # Set the X-axis tick labels
    ax.set_xticklabels(get_option_list_from_name(prim_x), fontsize=12, rotation=0)

    # Add a legend to identify the secondary groups
    ax.legend(title="Group", fontsize=10, loc='upper right')

    # Add a title
    ax.set_title(plot_title, fontsize=16, pad=15)

    # Add a horizontal grid for better readability
    ax.grid(axis='y', linestyle='--', alpha=0.6)

    # Ensure the X-axis limits include some padding
    ax.set_xlim(ind[0] - 0.5, ind[-1] + 0.5)

    # Display the plot
    plt.savefig(PLOT_FILES_PATH+savename)

def plot_line_graph(means, stds, x_cat: str, perf_metric: str, plottitle: str, savename: str):
    """
    Plots a line graph with error bars for the given means and stds.

    Parameters:
        means (list or np.ndarray): Mean values for each category.
        stds (list or np.ndarray): Standard deviations corresponding to each mean.
        x_cats (list of str): Categories for the x-axis.
        perf_metric (str): Label for the y-axis (performance metric).
        plottitle (str): Title of the plot.
        savename (str): File name to save the plot (e.g., 'plot.png').
    """
    x = np.arange(len(means))  # numeric positions for x-axis

    plt.figure(figsize=(8, 5))
    plt.errorbar(x, means, yerr=stds, fmt='-o', capsize=5, label=perf_metric)

    plt.xticks(x, get_option_list_from_name(x_cat), rotation=45)
    plt.xlabel(get_xaxis_label_from_name(x_cat))
    plt.ylabel(perf_metric)
    plt.title(plottitle)
    plt.legend()
    plt.tight_layout()

    plt.savefig(savename, dpi=300)
    plt.close()

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



def fetch_data_and_generate_plot(primary_x, primary_y, secondary_x,
                                 plot_title, savename, default_overrides : dict[str, str] = None):

    means, stds = read_data_for_plot(primary_x, primary_y, secondary_x, default_overrides)
    plot_double_bar_graph(means, stds, primary_x, primary_y, secondary_x, plot_title, savename)

def main():

    ## PART 1 Plots
    part1_gflop_title = "Scalar Baseline vs Autovectorized : {} kernel"
    part1_gflop_savename = "BaselineVsVectorized{}"

    for kern in kernel_options:
        fetch_data_and_generate_plot("mem", "GFLOPS", "vec",
                                    part1_gflop_title.format(kern), part1_gflop_savename.format(kern),
                                     {"kernel" : kern})

    speedup_plot_title = "Scalar vs Baseline Speedup for all kernels"
    speedup_plot_savename = "Part1SpeedUp"

    speedup_by_kerntype_arr = []
    for kern in kernel_options:
        means, _ = read_data_for_plot("mem", "GFLOPS", "vec",
                                      {"kernel" : kern})
        speedup_means = list(map(lambda x: x[1] / x[0], zip(means[0], means[1])))
        speedup_by_kerntype_arr.append(speedup_means)

    plot_double_line_graph(speedup_by_kerntype_arr, "mem", "kern",
                           "Vectorized vs Non Vectorized Speedup",
                           speedup_plot_title, speedup_plot_savename)

    ## PART 2 PLOTS
    part2_gflop_title = "Locality Sweep Peformance in GFLOPS"
    part2_gflop_savename = "LocalitySweepGFLOPS"

    part2_cpe_title = "Locality Sweep Peformance in CPE"
    part2_cpe_savename = "LocalitySweepCPE"

    fetch_data_and_generate_plot("mem", "GFLOPS", "vec",
                                 part2_gflop_title, part2_gflop_savename,
                                 {"kernel" : "STREAM"})

    fetch_data_and_generate_plot("mem", " CPE", "vec",
                                 part2_cpe_title, part2_cpe_savename,
                                 {"kernel" : "STREAM"})

    ## PART 3 PLOTS
    part3_title = "Alignment vs Tail Handling Across Sizes"
    part3_savename = "Part3Graph"

    fetch_data_and_generate_plot("mem", "GFLOPS", "align",
                                 part3_title, part3_savename)

    ## PART 4 PLOTS
    part4_title = "Stride vs Gather patterns performance effects"
    part4_savename = "Part4Plot"

    fetch_data_and_generate_plot("stride", "GFLOPS", "vec",
                                 part4_title, part4_savename)

    ## PART 5 PLOTS

    part5_title = "Float vs Double effects on SIMD for GFLOPS"
    part5_savename = "Part5Plot"

    fetch_data_and_generate_plot("float", "GFLOPS", "vec",
                                 part5_title, part5_savename)

    #### FLOATING POINT SPEEDUP PLOT

    speedup_plot_title = "Float32 vs Float64 Speedup for all kernels"
    speedup_plot_savename = "Float32Float64SpeedUp"

    speedup_by_kerntype_arr = []
    for ftype in float_options:
        means, _ = read_data_for_plot("mem", "GFLOPS", "vec",
                                  {"float" : ftype})
        speedup_means = list(map(lambda x: x[1] / x[0], zip(means[0], means[1])))
        speedup_by_kerntype_arr.append(speedup_means)

    plot_double_line_graph(speedup_by_kerntype_arr, "mem", "float",
                           "SIMD Speedup Float32 vs Float64",
                           speedup_plot_title, speedup_plot_savename)

main()

