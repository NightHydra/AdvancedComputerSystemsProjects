import pandas as pd
import data_retrieval_file as dr
import plotting_helper as ph


DATAPATH = "raw_data/data_analysis/"
PLOTPATH = "raw_data/plots/"





def main():


    # Zero queue table
    zq_json = dr.open_json(DATAPATH+"zero_queue_baseline_data.json")
    extracted_data = dr.extract_performance_data_from_json(zq_json)



    column_data = {"4Ki Random Reads" : extracted_data["Zero Queue 4kb"]["read"],
                   "4Ki Random Writes" : extracted_data["Zero Queue 4kb"]["write"],
                   "128Ki Sequential Reads" : extracted_data["Zero Queue 128 Seq"]["read"],
                   "128Ki Sequential Writes" : extracted_data["Zero Queue 128 Seq"]["write"]}
    row_data = {"Average Latency(us)" : "lat_mean",
                "Latency variance(us)" : "lat_var",
                "95th Percentile Latency(us)": "lat_p95",
                "99th Percentile Latency(us)": "lat_p99"}
    zq_table_savepath = PLOTPATH+"zero_queue_table.txt"

    ph.save_md_table_to_file(column_data, row_data, zq_table_savepath)

    # Block Pattern Sweep



main()







