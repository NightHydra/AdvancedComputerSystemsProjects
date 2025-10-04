import matplotlib.pyplot as plt
import math
import numpy as np

def save_md_table_to_file(column_headers: dict[str, dict[str, str]], row_headers: dict[str, str],  savename: str):
    """

    :param column_headers: Each of the columns name, to what that columns data is
    :param row_headers: Actual Name matching to what its called in the data dictionaries
    :param savename: Filename to save the table as
    """

    # First Row
    table = ""
    table += "| |"
    for maj_cat in column_headers.keys():
        table += maj_cat + "|"
    table += "\n"

    # Second Row
    table += ("|" + "---|" * (len(column_headers.keys())+1)) + "\n"



    # Go through every row
    for row_name, row_key in row_headers.items():

        table += "|" + row_name + "|"

        for col_data in column_headers.values():

            # Get the mean of the column and print it
            table += f"{col_data[row_key]:.2f}"+"|"
        table += "\n"

    with open(savename, "w") as file:
        file.write(table)

def plot_single_line_graph(
        x_axis: list[float],
        line1_data: list[float],
        x_axis_title: str,
        y_axis_title: str,
        chartname: str
):
    """
    Create a single line graph.

    Parameters
    ----------
    x_axis : list[float]
        X-axis values.
    line1_data : list[float]
        Y-axis values for the single line.
    x_axis_title : str
        Label for the x-axis.
    y_axis_title : str
        Label for the y-axis.
    chartname : str
        Title of the chart.

    Returns
    -------
    fig : matplotlib.figure.Figure
    axes : list[matplotlib.axes.Axes]
        Returns (fig, [ax]) so it can be used with combine_plots.
    """
    fig, ax = plt.subplots(figsize=(6, 4))

    ax.plot(x_axis, line1_data, label="Line 1", color="blue", marker="o")
    ax.set_xlabel(x_axis_title)
    ax.set_ylabel(y_axis_title)
    ax.set_title(chartname)
    ax.grid(True, linestyle="--", alpha=0.6)
    ax.legend()

    return fig, [ax]

def plot_double_line_graph(
        x_axis: list[float],
        line1_data: list[float],
        line2_data: list[float],
        x_axis_title: str,
        line1_axis_title: str,
        line2_axis_title: str,
        chartname: str
):
    fig, ax1 = plt.subplots(figsize=(5, 4))

    # First line (left y-axis)
    color1 = "tab:blue"
    ax1.set_xlabel(x_axis_title)
    ax1.set_ylabel(line1_axis_title, color=color1)
    ax1.plot(x_axis, line1_data, color=color1, marker="o", label=line1_axis_title)
    ax1.tick_params(axis="y", labelcolor=color1)
    ax1.grid(True, linestyle="--", alpha=0.6)

    # Second line (right y-axis)
    ax2 = ax1.twinx()
    color2 = "tab:red"
    ax2.set_ylabel(line2_axis_title, color=color2)
    ax2.plot(x_axis, line2_data, color=color2, marker="s", label=line2_axis_title)
    ax2.tick_params(axis="y", labelcolor=color2)

    # ✅ Set axis title (not figure title, so it carries over in combined grid)
    ax1.set_title(chartname)

    return fig, [ax1, ax2]



def combine_plots(
        plots: list[tuple[plt.Figure, tuple[plt.Axes, ...]]],
        savename: str,
        suptitle: str = "Combined Plots",
        rows: int = None,
        cols: int = None,
        single_plot_figsize: tuple[float, float] = (5, 4)
) -> plt.Figure:
    """
    Combines a list of existing Matplotlib Figure objects into a single new figure
    arranged in a grid, and saves the result to a PNG file.

    This version handles standard plots (lines and bars/patches) by explicitly
    copying the underlying plot elements.

    Args:
        plots (List[Tuple[Figure, Tuple[Axes, ...]]]): A list where each element is a tuple
            containing the original Figure and a tuple of ALL its associated Axes objects.
        savename (str): The filename (e.g., 'combined_results.png') to save the output.
        suptitle (str): The main title for the entire combined figure.
        rows (int, optional): The number of rows for the grid. Defaults to auto-square layout.
        cols (int, optional): The number of columns for the grid. Defaults to auto-square layout.
        single_plot_figsize (Tuple[float, float]): The base size (width, height) used for
                                                   calculating the total figure size.

    Returns:
        Figure: The newly created combined Matplotlib Figure object.
    """
    n = len(plots)
    if n == 0:
        raise ValueError("The list of plots cannot be empty!")

    # 1. Determine the Grid Layout
    if rows is None and cols is None:
        # Auto-compute square-ish layout
        rows = math.ceil(math.sqrt(n))
        cols = math.ceil(n / rows)
    elif rows is None:
        cols = int(cols)
        rows = math.ceil(n / cols)
    elif cols is None:
        rows = int(rows)
        cols = math.ceil(n / rows)

    # Calculate total figure size based on the number of plots and base size
    total_figsize = (single_plot_figsize[0] * cols, single_plot_figsize[1] * rows)

    # 2. Create the new combined figure and subplots
    combined_fig, axes = plt.subplots(rows, cols, figsize=total_figsize)

    # Ensure 'axes' is a flat list/array for easy iteration
    axes_flat = axes.flatten() if n > 1 else [axes]

    # 3. Iterate through source figures and copy content
    for i, (source_fig, source_axes_tuple) in enumerate(plots):

        # source_axes_tuple is the tuple of axes (ax1, ax2, ...) from the original plot
        # target_ax_main is the primary axis of the new subplot
        target_ax_main = axes_flat[i]

        source_axes = source_axes_tuple

        # Prepare Target Axes to match the source structure
        target_axes = [target_ax_main]
        # Create twin axes in the target subplot for every extra axis in the source
        for _ in range(1, len(source_axes)):
            target_axes.append(target_ax_main.twinx())

        all_lines = []
        all_labels = []

        for j, (source_ax, target_ax) in enumerate(zip(source_axes, target_axes)):

            # --- Copy Plot Content (Lines, Markers, Colors) ---

            # 1. Copy Lines (from ax.plot)
            for line in source_ax.get_lines():
                target_ax.plot(
                    line.get_xdata(),
                    line.get_ydata(),
                    label=line.get_label(),
                    color=line.get_color(),
                    marker=line.get_marker(),
                    linestyle=line.get_linestyle(),
                    linewidth=line.get_linewidth(),
                    zorder=line.get_zorder()
                )

            # 2. Copy Bar/Rectangle Patches (from ax.bar, ax.hist)
            for patch in source_ax.patches:
                # Matplotlib bar() plots rectangles. We extract geometry and properties.
                x = patch.get_x()
                y = patch.get_y()
                width = patch.get_width()
                height = patch.get_height()

                # Replicate the bar using its properties. The x position for bar is its center.
                target_ax.bar(
                    x + width / 2, # New x is the center point
                    height,
                    width=width,
                    bottom=y,
                    color=patch.get_facecolor(),
                    edgecolor=patch.get_edgecolor(),
                    linewidth=patch.get_linewidth(),
                    alpha=patch.get_alpha(),
                    zorder=patch.get_zorder()
                )

            # --- Copy Labels and Title ---
            if j == 0: # Primary Axis
                target_ax.set_title(source_ax.get_title())
                target_ax.set_xlabel(source_ax.get_xlabel())
                target_ax.set_ylabel(source_ax.get_ylabel())

                # --- Copy Axis Limits and Ticks (Crucial for visual consistency) ---
                target_ax.set_xlim(source_ax.get_xlim())
                target_ax.set_ylim(source_ax.get_ylim())

                # Copy categorical x-tick labels if they exist (common in bar charts)
                target_ax.set_xticks(source_ax.get_xticks())
                target_ax.set_xticklabels([t.get_text() for t in source_ax.get_xticklabels()])

                # Add a grid for clarity
                target_ax.grid(True, linestyle='--', alpha=0.7)

            else: # Twin Axis
                # Only set the Y-label for the twin axis
                line_color = target_ax.get_lines()[0].get_color() if target_ax.get_lines() else 'black'
                target_ax.set_ylabel(source_ax.get_ylabel(), color=line_color)

                # Copy Y-limits and Ticks for twin axis
                target_ax.set_ylim(source_ax.get_ylim())
                target_ax.set_yticks(source_ax.get_yticks())


            # Collect lines and labels for consolidated legend
            # NOTE: Bar plots don't always add handles/labels this way.
            handles, labels = target_ax.get_legend_handles_labels()
            all_lines.extend(handles)
            all_labels.extend(labels)

        # --- Consolidate Legends on the MAIN target axis ---
        # Get the original legend contents from the source figure's primary axis
        source_handles, source_labels = source_axes[0].get_legend_handles_labels()

        # If the source had a legend, use its contents directly.
        if source_handles:
            # Re-create a unified legend
            target_ax_main.legend(source_handles, source_labels, loc='best')
        elif all_lines:
            # Fallback to consolidation if source didn't define a unified legend
            target_ax_main.legend(all_lines, all_labels, loc='best')


        # Close the source figure to free memory
        plt.close(source_fig)

    # 4. Hide unused subplots if grid size > n
    for j in range(n, len(axes_flat)):
        axes_flat[j].set_visible(False)

    # 5. Apply Super Title and Final Layout adjustments
    combined_fig.suptitle(suptitle, fontsize=14, fontweight='bold')
    plt.tight_layout(rect=[0, 0, 1, 0.95]) # Adjust layout to make room for suptitle

    # 6. Save the combined figure
    try:
        combined_fig.savefig(savename)
        print(f"Successfully saved combined plot to '{savename}'")
    except Exception as e:
        print(f"Error saving file: {e}")

    return combined_fig

def plot_single_bar_graph(barnames_to_data: dict[str, list[float]],
                          xaxis_name: str,
                          yaxis_name: str,
                          plot_title: str):
    """
    Create a bar plot with error bars (mean ± std dev).

    Parameters
    ----------
    barnames_to_data : dict[str, list[float]]
        Key = bar category, Value = [mean, std]
    xaxis_name : str
        Label for x-axis
    yaxis_name : str
        Label for y-axis
    plot_title : str
        Title of the plot

    Returns
    -------
    fig, (ax1, None)
        Figure and primary axis (to match combine_plots interface)
    """
    fig, ax = plt.subplots(figsize=(5, 4))

    categories = list(barnames_to_data.keys())
    means = [barnames_to_data[k][0] for k in categories]
    stds = [barnames_to_data[k][1] for k in categories]

    x = np.arange(len(categories))

    ax.bar(x, means, yerr=stds, capsize=5, alpha=0.8, color="tab:blue")

    ax.set_xticks(x)
    ax.set_xticklabels(categories)
    ax.set_xlabel(xaxis_name)
    ax.set_ylabel(yaxis_name)
    ax.set_title(plot_title)
    ax.grid(axis="y", linestyle="--", alpha=0.6)

    return fig, [ax]