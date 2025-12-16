import matplotlib.pyplot as plt
import numpy as np

PLOT_FILES_PATH = "data_analysis/plots/"

def plot_double_bar_graph(means, stds, primx_titles : list[str], second_primx_titles : list[str], x_axis_title : str,
                          y_title, plot_title : str, savename : str, logscale = False):

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

    n_prim_categories = len(means[0])
    n_groups = len(means)

    # Calculate bar width to ensure bars fit nicely within the category space (0.8 is a good total width)
    bar_width = 1.6 / (n_prim_categories*n_groups)

    # Set the central positions of the primary category ticks on the X-axis
    ind = np.arange(n_prim_categories)

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
        offset = ind + (i - (n_groups - 1) / 2) * bar_width

        # Plot the bars for the current secondary group
        ax.bar(
            offset,
            means[i],
            bar_width,
            yerr=stds[i],
            capsize=5,  # Size of the error bar caps
            label=second_primx_titles[i] # Label for the legend
        )

    # --- 4. Customize Plot ---

    # Set the Y-axis label
    ax.set_ylabel(y_title, fontsize=14)

    if logscale == True:
        ax.set_yscale('log')

    # Set the X-axis labels
    ax.set_xlabel(x_axis_title, fontsize=14)

    # Set the X-axis tick positions to the center of the bar cluster (ind)
    ax.set_xticks(ind)
    # Set the X-axis tick labels
    ax.set_xticklabels(primx_titles, fontsize=12, rotation=0)

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