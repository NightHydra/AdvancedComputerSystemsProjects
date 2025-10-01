import re
import pandas as pd


def get_content_from_file(fname : str) -> str:
    with open(fname, "r") as file:
        return file.read()

def parse_loaded_latency_bandwidths(fname : str) -> list[float]:

    data_content = get_content_from_file(fname)

    pattern = r'\n\s*\d+\s+\d+\.\d+\s+(\d+\.?\d*)'
    extracted_bandwidth_values = re.findall(pattern, data_content)

    return list(map(float, extracted_bandwidth_values))

def parse_loaded_latency_latencies(fname : str) -> list[float]:

    data_content = get_content_from_file(fname)

    pattern = r'^\s*\d+\s+(\d+\.?\d*)\s+\d+\.?\d*'
    extracted_latency_values = re.findall(pattern, data_content, re.MULTILINE)


    return list(map(float, extracted_latency_values))


def parse_idle_latency_file(fname: str) -> list[float]:

    pattern = r'(\d+\.\d+)\s*ns'

    with open(fname, "r") as file:
        log_content = file.read()
    # Find all matching nanosecond values
    extracted_ns_values = re.findall(pattern, log_content)

    ns_list = []
    for value in extracted_ns_values:
        ns_list.append(float(value))
    return ns_list

def parse_intensity_run_tests_for_averages(fname: str):
    """
    Parses all Delay Time, Latency, and Bandwidth values from the log, groups them by
    Delay Time, and calculates the mean for both Latency and Bandwidth for each unique delay.

    Args:
        fname: The path to the log file (used only in the mock get_content_from_file).

    Returns:
        A tuple containing two lists: (mean_latencies, mean_bandwidths),
        where both lists are ordered by increasing Delay Time.
    """
    data_content = get_content_from_file(fname)

    # UPDATED REGEX: Captures three groups:
    # 1. Delay Time (Iteration number) as a string (e.g., '00000', '00200')
    # 2. Latency Value (The first number)
    # 3. Bandwidth Value (The second number) -> Bandwidth is now explicitly captured.
    pattern = r'^\s*(\d+)\s+(\d+\.?\d*)\s+(\d+\.?\d*)'

    # re.MULTILINE flag is essential for matching line starts (^)
    extracted_data_tuples = re.findall(pattern, data_content, re.MULTILINE)

    # 1. Convert extracted data to a list of dictionaries for DataFrame creation
    records = []
    for delay_str, latency_str, bandwidth_str in extracted_data_tuples:
        # Convert the string delay (e.g., '00000') to an integer (0)
        delay_int = int(delay_str)
        records.append({
            'Delay': delay_int,
            'Latency': float(latency_str),
            'Bandwidth': float(bandwidth_str)
        })

    # 2. Use Pandas to group and calculate the mean
    df = pd.DataFrame(records)

    # Group by the 'Delay' column and find the mean of both 'Latency' and 'Bandwidth'
    mean_data = df.groupby('Delay')[['Latency', 'Bandwidth']].mean()

    # 3. Convert the resulting Series back to two lists, ordered by the 'Delay' index
    mean_latencies = mean_data['Latency'].tolist()
    mean_bandwidths = mean_data['Bandwidth'].tolist()

    print (mean_bandwidths)

    return mean_latencies, mean_bandwidths