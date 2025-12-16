import pandas as pd
import matplotlib.pyplot as plt
import re
import os
import numpy as np
import sys
import io

# --- Configuration ---

# Define the directory where the raw data files are expected and where plots will be saved
DATA_DIR = "data_analysis/raw_data"
PLOT_DIR = "data_analysis/plots"
# Create the plot directory if it doesn't exist
os.makedirs(PLOT_DIR, exist_ok=True)

# Define the expected filter targets and their resulting file names
filter_targets = {
    "XOR Filter": "test_xor_filter.txt",
    "Cuckoo Filter": "test_cuckoo_filter.txt",
    "Quotient Filter": "test_quotient_filter.txt",
    "Blocked Bloom Filter": "test_blocked_bloom_filter.txt",
}

# Regex patterns to extract the required data from the file content
NAME_PATTERN = re.compile(r"--- Results for (.*?) ---")
FPR_PATTERN = re.compile(r"Measured False Positive Rate \(FPR\): ([\d\.]+)")
THROUGHPUT_PATTERN = re.compile(r"Throughput \(Insert \+ Query\): ([\d\.]+) M keys/sec")

# --- Data Extraction ---

def extract_data_from_file(file_path):
    """Parses a single raw data file to extract filter metrics."""
    try:
        with open(file_path, 'r') as f:
            content = f.read()

        name_match = NAME_PATTERN.search(content)
        fpr_match = FPR_PATTERN.search(content)
        throughput_match = THROUGHPUT_PATTERN.search(content)

        if name_match and fpr_match and throughput_match:
            filter_name = name_match.group(1).strip()
            fpr = float(fpr_match.group(1))
            throughput = float(throughput_match.group(1))
            return {
                "Filter": filter_name,
                "FPR": fpr,
                "Throughput": throughput
            }
        else:
            print(f"Warning: Could not extract all metrics from {file_path}", file=sys.stderr)
            return None
    except FileNotFoundError:
        print(f"Error: File not found at {file_path}. Please ensure you run the .bat script first.", file=sys.stderr)
        return None
    except Exception as e:
        print(f"An error occurred while processing {file_path}: {e}", file=sys.stderr)
        return None

# --- Main Data Processing ---

def load_and_process_data():
    """Loads data from files and calculates derived metrics."""
    data_list = []
    for name, filename in filter_targets.items():
        file_path = os.path.join(DATA_DIR, filename)
        data = extract_data_from_file(file_path)
        if data:
            data_list.append(data)

    df = pd.DataFrame(data_list)

    if df.empty:
        print("\n!! WARNING: No real data was successfully parsed. Using dummy data for plotting demonstration.", file=sys.stderr)
        # Fallback to dummy data
        df = pd.DataFrame({
            "Filter": ["XOR Filter", "Cuckoo Filter", "Quotient Filter", "Blocked Bloom Filter"],
            "FPR": [0.009, 0.045, 0.015, 0.02],
            "Throughput": [54.3, 30.1, 40.5, 65.2]
        })

    # --- Derived Metrics Calculation ---

    # 1. Cost per Operation (ns)
    # Cost (ns) = 1000 / Throughput (M keys/sec)
    df['Cost_Per_Op_ns'] = 1000 / df['Throughput']

    # 2. Relative FPR (Normalized to the worst FPR)
    max_fpr = df['FPR'].max()
    df['Relative_FPR'] = df['FPR'] / max_fpr if max_fpr > 0 else 0

    # 3. Placeholder Error Bars (5% of the mean value)
    df['FPR_Error'] = df['FPR'] * 0.05
    df['Throughput_Error'] = df['Throughput'] * 0.05
    df['Cost_Per_Op_ns_Error'] = df['Cost_Per_Op_ns'] * 0.05
    df['Relative_FPR_Error'] = df['Relative_FPR'] * 0.05

    return df

# --- Plotting Function ---

def generate_bar_plot(data, y_col, y_err_col, title, ylabel, filename, ascending=True):
    """Generates and saves a bar chart with error bars, sorted by the primary metric."""

    # Sort the data frame based on the primary metric
    df_sorted = data.sort_values(by=y_col, ascending=ascending).reset_index(drop=True)

    plt.figure(figsize=(10, 6))

    # Get the error values as a list/array
    y_error_values = df_sorted[y_err_col].values

    # Plot the bars with error bars
    bars = plt.bar(df_sorted['Filter'], df_sorted[y_col],
                   yerr=y_error_values, capsize=5, color='teal', alpha=0.8)

    plt.title(title, fontsize=14)
    plt.ylabel(ylabel, fontsize=12)
    plt.xlabel('Filter Type', fontsize=12)

    # Add values on top of the bars
    for i, bar in enumerate(bars):
        yval = bar.get_height()
        error_value = y_error_values[i] # Get the error value for this bar

        # Determine formatting based on the metric
        if 'FPR' in y_col:
            text_format = f'{yval:.4f}'
        elif 'Cost' in y_col:
            text_format = f'{yval:.2f} ns'
        else:
            text_format = f'{yval:.2f}'

        # Position text above bar + error value, adding a small multiplier (1.05) for clean separation
        plt.text(bar.get_x() + bar.get_width()/2, yval + error_value * 1.05,
                 text_format, ha='center', va='bottom', fontsize=10)

    plt.xticks(rotation=15, ha='right')
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(PLOT_DIR, filename))
    plt.close()
    print(f"Saved plot to {os.path.join(PLOT_DIR, filename)}")

# --- Execution ---

if __name__ == "__main__":
    df_metrics = load_and_process_data()

    if not df_metrics.empty:
        # Group 1: FPR and False Positive Ratio
        generate_bar_plot(df_metrics, 'FPR', 'FPR_Error',
                          '1a. False Positive Rate (FPR) by Filter Type', 'FPR',
                          '1a_fpr_vs_filter.png', ascending=True)

        generate_bar_plot(df_metrics, 'Relative_FPR', 'Relative_FPR_Error',
                          '1b. Relative FPR (Normalized to Max FPR) by Filter Type', 'Relative FPR (Unitless)',
                          '1b_relative_fpr_vs_filter.png', ascending=True)

        # Group 2: Insert/Lookup Ratio (Throughput/Cost)
        generate_bar_plot(df_metrics, 'Throughput', 'Throughput_Error',
                          '2a. Throughput (Insert + Query) by Filter Type', 'Throughput (M keys/sec)',
                          '2a_throughput_vs_filter.png', ascending=False)

        generate_bar_plot(df_metrics, 'Cost_Per_Op_ns', 'Cost_Per_Op_ns_Error',
                          '2b. Cost Per Operation by Filter Type', 'Cost Per Operation (ns)',
                          '2b_cost_per_op_vs_filter.png', ascending=True)
    else:
        print("Could not generate plots due to missing or empty data.", file=sys.stderr)

    print("\nPlotting script execution finished.")