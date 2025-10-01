import re
import pandas as pd
import output_parsing_tools as pt

DATAPATH = "data_analysis/raw_data"

def parse_idle_latencies():
    df = pd.DataFrame()
    df["L1"] = pt.parse_idle_latency_file(DATAPATH+"/zero_queue_l1.txt")
    df["L2"] = pt.parse_idle_latency_file(DATAPATH+"/zero_queue_l2.txt")
    df["LLC"] = pt.parse_idle_latency_file(DATAPATH+"/zero_queue_llc.txt")
    df["DRAM"] = pt.parse_idle_latency_file(DATAPATH+"/zero_queue_dram.txt")

    return df

def parse_strided_data() -> tuple[pd.DataFrame, pd.DataFrame]:
    df_bw = pd.DataFrame()
    df_lat = pd.DataFrame()

    stride_sizes = ["64", "256", "1024"]

    for ssize in stride_sizes:
        df_bw[ssize] = pt.parse_loaded_latency_bandwidths(DATAPATH + "/strided"+ssize+"b.txt")
        df_bw[ssize+"ra"] = pt.parse_loaded_latency_bandwidths(DATAPATH + "/strided"+ssize+"b_ra.txt")

        df_lat[ssize] = pt.parse_loaded_latency_latencies(DATAPATH + "/strided"+ssize+"b.txt")
        df_lat[ssize+"ra"] = pt.parse_loaded_latency_latencies(DATAPATH + "/strided"+ssize+"b_ra.txt")
    return df_bw, df_lat

def parse_rw_ratio_data() ->pd.DataFrame:
    df = pd.DataFrame()

    read_write_ratios = {"100% Read" : "r1", "3:1 Read/Write" : "r3w1",
                         "2:1 Read/Write" : "r2w1", "1:1 Read/Write" : "r1w1"}

    for (k, v) in read_write_ratios.items():
        df[k] = pt.parse_loaded_latency_bandwidths(DATAPATH+"/"+v+".txt")

    return df

def parse_full_loaded_sweep() -> pd.DataFrame:

    mean_lats, mean_bws = pt.parse_intensity_run_tests_for_averages(DATAPATH+"/intensity_data.txt")

    df = pd.DataFrame()

    df["Latency"] = mean_lats
    df["Bandwidth"] = mean_bws

    return df.sort_values(by="Latency", ascending=True)


def parse_memsize_and_latency_for_sizesweep() -> pd.DataFrame():

    file_contents = pt.get_content_from_file(DATAPATH+"/memsweep.txt")

    pattern = r'(?:Using buffer size of )(\d+\.?\d*)MiB/thread.*?MB/sec\n=+\n\s*\d+\s+(\d+\.\d+)\s+(\d+\.?\d*)'

    extracted_data = re.findall(pattern, file_contents, re.DOTALL)

    kb_size_list = []
    bw_size_list = []
    for mib_size, latency, bandwidth in extracted_data:
        kb_size_list.append(float(mib_size)*1000)
        bw_size_list.append(float(bandwidth))

    df = pd.DataFrame()
    df["Memory Size (kb)"] = kb_size_list
    df["Bandwidth (MB / s)"] = bw_size_list

    return df