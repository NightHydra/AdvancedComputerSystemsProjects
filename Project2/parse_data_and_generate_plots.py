import re
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

import output_parsing_tools as pt
import test_parsing_helpers as testparse

PLOTPATH = "data_analysis/plots/"

def plot_line_graph(df : pd.DataFrame, plot_title : str, savename : str,
                    log_scale: bool = False):

    # Infer X and Y column names (using the first two columns)
    x_col = df.columns[0]
    y_col = df.columns[1]

    # NOTE: Since we assume the data is all floats, we remove the
    # to_numeric and dropna calls for efficiency.

    # 1. Create the plot figure
    plt.figure(figsize=(10, 6))

    # 2. Generate the line plot
    df.plot(
        x=x_col,
        y=y_col,
        kind='line',
        marker='o',
        title=plot_title,
        xlabel=x_col,
        ylabel=y_col,
        ax=plt.gca() # Use the current figure axes
    )

    if log_scale:
        plt.xscale('log', base=2)
        plt.yscale('log', base=2)

    plt.grid(True, linestyle='--', alpha=0.6)


    plt.savefig(savename)

def plot_bar_graphs(df: pd.DataFrame, plot_title: str, savename: str, x_label: str,
                    y_label : str):
    """
    Plots a bar graph where each bar represents the mean of a single column
    in the input DataFrame, including error bars based on the standard deviation.

    Args:
        df: A pandas DataFrame containing only numeric (float) data. The mean
            of each column will be plotted as a separate bar.
        plot_title: The title for the plot.
        savename: Optional filename to save the plot (e.g., 'my_bar_plot.png').
                  If None, the plot is displayed.
        y_label: The title for the Y-axis. Defaults to 'Mean Value'.
    """

    # ASSUMPTION: The input DataFrame 'df' is all floats, so we use it directly.
    if df.empty:
        print("Error: DataFrame is empty.")
        return

    mean_series = df.mean() # Calculate Mean across all columns
    std_series = df.std() # Calculate Standard Deviation for error bars
    num_bars = len(mean_series)

    # 2. Set up colors for visualization (automatically scales to number of bars)
    colors = plt.cm.viridis(np.linspace(0, 1, num_bars))

    # 3. Create the plot figure
    plt.figure(figsize=(max(8, num_bars * 1.5), 6))

    # 4. Generate the bar plot, now including the yerr parameter
    plt.bar(
        mean_series.index,   # X values (Column Names)
        mean_series.values,  # Y values (Mean of each column)
        yerr=std_series.values, # Error bars based on Standard Deviation
        capsize=5,           # Add caps to the error bars
        color=colors,        # Scaled colors for all bars
        edgecolor='black'
    )

    # 5. Set labels and title
    plt.title(plot_title, fontsize=16, fontweight='bold')
    plt.xlabel(x_label, fontsize=12)
    plt.ylabel(y_label, fontsize=12)

    # Add a horizontal grid line for better reading of values
    plt.grid(axis='y', linestyle='--', alpha=0.6)
    plt.tight_layout() # Adjust plot to fit all elements

    plt.savefig(savename, bbox_inches='tight')


def save_md_table_to_file(df : pd.DataFrame(), minor_category_titles, major_category_titles,
                          savename: str):
    """

    :param df: The dataframe to evaluate
    :param minor_category_titles: The titles for all the categories separated by distance 1 columns
    :param major_category_titles: The titles for all the categories that arent together
    """

    # First Row
    table = ""
    table += "| |"
    for maj_cat in major_category_titles:
        table += maj_cat + "|"
    table += "\n"

    # Second Row
    table += ("|" + "---|" * (len(major_category_titles)+1)) + "\n"

    # Every Other Row
    column_names = df.columns.tolist()

    num_minor_cats = len(minor_category_titles)


    for minor_cat_index in range(0, num_minor_cats):

        table += "|" + minor_category_titles[minor_cat_index] + "|"

        for major_cat_index in range(0, len(major_category_titles)):

            # Get the mean of the column and print it
            table += f"{df[column_names[major_cat_index * num_minor_cats + minor_cat_index]].mean() : .2f}|"
        table += "\n"

    with open(savename, "w") as file:
        file.write(table)


def plot_double_bar_graphs(df: pd.DataFrame, plot_title: str, columns_per_group: int, savename: str, y_label: str, category_names: list[str] = None, group_names: list[str] = None):
    """
    Plots a single grouped bar chart where columns are grouped based on their
    positional index modulo 'columns_per_group'.

    - The X-axis (Category) is determined by the column index // columns_per_group.
    - The Legend (Group) is determined by the column index % columns_per_group.

    Args:
        df: A pandas DataFrame containing only numeric (float) data.
        plot_title: The title for the plot.
        columns_per_group: The number of columns that form a logical group (or run)
                           and will be clustered together on the plot for each category.
        savename: Optional filename to save the plot (e.g., 'my_bar_plot.png').
                  If None, the plot is displayed.
        y_label: The title for the Y-axis. Defaults to 'Mean Value'.
        category_names: Optional list of names for the X-axis categories. Must match
                        the number of categories (total_cols / columns_per_group).
        group_names: Optional list of names for the legend groups. Must match
                     columns_per_group.
    """

    if df.empty:
        print("Error: DataFrame is empty.")
        return

    total_cols = df.shape[1]
    num_categories = total_cols // columns_per_group

    if columns_per_group <= 0 or total_cols % columns_per_group != 0:
        print(f"Error: 'columns_per_group' ({columns_per_group}) must be a positive integer and must evenly divide the total number of columns ({total_cols}).")
        return

    if category_names and len(category_names) != num_categories:
        print(f"Error: Provided category_names list has {len(category_names)} names, but {num_categories} categories are required.")
        return

    if group_names and len(group_names) != columns_per_group:
        print(f"Error: Provided group_names list has {len(group_names)} names, but {columns_per_group} groups are required.")
        return


    # 1. Calculate Mean and Standard Deviation
    mean_series = df.mean()
    std_series = df.std()

    # 2. Create a temporary DataFrame for plotting and add the grouping columns
    plot_data = pd.DataFrame({
        'Mean': mean_series.values,
        'Std': std_series.values,
        'Combined': mean_series.index
    })

    # 3. Define Category (X-axis) and Group (Legend) based on column index
    column_index = np.arange(total_cols)
    plot_data['Category_Index'] = column_index // columns_per_group
    plot_data['Group_Index'] = column_index % columns_per_group

    # Category (X-Axis, index // N): Should be the Run/Category name
    if category_names:
        plot_data['Category'] = plot_data['Category_Index'].apply(lambda i: category_names[i])
    else:
        # Fallback: Get the name from the *first* column in that category index group
        def get_category_name(idx):
            col_name = mean_series.index[idx * columns_per_group]
            # Assumes Category is the suffix (e.g., 'Run1' in 'Metric_Run1')
            return col_name.split('_', 1)[-1] if '_' in col_name and len(col_name.split('_')) > 1 else f"C{idx}"

        plot_data['Category'] = plot_data['Category_Index'].apply(get_category_name)


    # Group (Legend, index % N): Should be the Metric/Group name
    if group_names:
        plot_data['Group'] = plot_data['Group_Index'].apply(lambda i: group_names[i])
    else:
        # Fallback: Get the name from the column index that corresponds to the group index
        def get_group_name(idx):
            col_name = mean_series.index[idx]
            # Assumes Group is the prefix (e.g., 'Latency' in 'Latency_Run1')
            return col_name.split('_')[0] if '_' in col_name else f"G{idx}"

        plot_data['Group'] = plot_data['Group_Index'].apply(get_group_name)


    # 4. Prepare data for the grouped bar plot (pivot table)
    # Pivot the data to get 'Category' as index, 'Group' as columns, and 'Mean' as values
    mean_pivot = plot_data.pivot_table(index='Category', columns='Group', values='Mean')
    std_pivot = plot_data.pivot_table(index='Category', columns='Group', values='Std')

    mean_pivot = mean_pivot.reindex(category_names)
    std_pivot = std_pivot.reindex(category_names)

    # 5. Set up the plotting style
    plt.style.use('default')
    fig, ax = plt.subplots(figsize=(10, 6))

    # Use pandas plotting functionality (built on matplotlib) for grouped bars
    ax = mean_pivot.plot(
        kind='bar',
        yerr=std_pivot,
        capsize=5,
        ax=ax,
        edgecolor='black', # Add black edge for better bar separation
        width=0.6 # Adjusted width to make bars closer for better grouping visualization
    )

    # 6. Set labels and title
    ax.set_title(plot_title, fontsize=16, fontweight='bold')
    ax.set_xlabel('Category', fontsize=12)
    ax.set_ylabel(y_label, fontsize=12)

    # Ensure X-axis labels are readable
    ax.tick_params(axis='x', rotation=0)

    # Add a legend title and place outside to prevent overlap
    ax.legend(title='Group', bbox_to_anchor=(1.05, 1), loc='upper left')

    # Add a horizontal grid line for better reading of values
    ax.yaxis.grid(True, linestyle='--', alpha=0.6)

    plt.tight_layout(rect=[0, 0, 0.9, 1]) # Adjust layout to make room for legend

    # 7. Save or display the plot
    plt.savefig(savename, bbox_inches='tight')






def main():
    part1_frame = testparse.parse_idle_latencies()

    (part2_bwframe, part2_latframe) = testparse.parse_strided_data()


    part3_frame = testparse.parse_rw_ratio_data()
    part4_frame = testparse.parse_full_loaded_sweep()
    part5_frame = testparse.parse_memsize_and_latency_for_sizesweep()

    ## ZERO QUEUE BASELINE PLOT
    save_md_table_to_file(part1_frame, ["Memory Locality"],
                          ["L1", "L2", "LLC", "DRAM"], PLOTPATH+"zq_baseline_matrix.txt")

    ## Stride / Access Pattern Matrix
    save_md_table_to_file(part2_bwframe, ["Sequential", "Random Access"],
                          ["64 bit stride", "256 bit stride", "1024 bit stride"],
                          PLOTPATH+"stride_vs_bw.txt")

    plot_double_bar_graphs(part2_bwframe, "Bandwidth when varying stride and access pattern",
                           2, PLOTPATH+"stride_bw_plot", "Bandwidth (MB/s)",
                           ["64 bit stride", "256 bit stride", "1024 bit stride"],
                           ["Sequential", "Random Access"])

    plot_double_bar_graphs(part2_latframe, "Latency when varying stride and access pattern",
                           2, PLOTPATH+"stride_lat_plot", "Latency (ns)",
                           ["64 bit stride", "256 bit stride", "1024 bit stride"],
                           ["Sequential", "Random Access"])

    ## READ/WRITE RATIO PLOT
    plot_bar_graphs(part3_frame, "RW Ratio Tests", PLOTPATH+"rw_sweep_plot.png",
                    "Read/Write Ratio", "Bandwidth (MB/S)")

    ## INTENSITY SWEEP PLOT
    plot_line_graph(part4_frame, "Intensity Sweep", PLOTPATH + "latency_vs_bandwidth.png")

    ## WORKING SIZE SWEEP PLOT
    plot_line_graph(part5_frame, "Bandwidth Sweep over Working Set Size",
                    PLOTPATH + "working_set_bws.png", log_scale=True)



main()
