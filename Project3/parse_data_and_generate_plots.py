import pandas as pd
import data_retrieval_file as dr
import plotting_helper as ph
from plotting_helper import combine_plots
import matplotlib.pyplot as plt

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

    for access in ["random", "sequential"]:
        block_sweep_json = dr.open_json(DATAPATH+f"block_size_sweep_{access}.json")
        extracted_data = dr.extract_performance_data_from_json(block_sweep_json)

        block_sizes = dr.get_blocksizes_from_json_data(block_sweep_json)

        iops_read_data = [extracted_data[j]["read"]["iops_mean"] for j in extracted_data.keys()]
        iops_write_data = [extracted_data[j]["write"]["iops_mean"] for j in extracted_data.keys()]

        bw_read_data = [extracted_data[j]["read"]["bw_mean"] for j in extracted_data.keys()]
        bw_write_data = [extracted_data[j]["write"]["bw_mean"] for j in extracted_data.keys()]

        plotout_read = ph.plot_double_line_graph(block_sizes.values(), iops_read_data, bw_read_data,
                                            "Block size (kb)",
                                            "IOPS", "Bandwidth (MB/s)",
                                            "Reads Data")

        plotout_write = ph.plot_double_line_graph(block_sizes.values(), iops_write_data, bw_write_data,
                                                 "Block size (kb)",
                                                 "IOPS", "Bandwidth (MB/s)",
                                                 "Writes Data")
        savename = PLOTPATH+f"block_size_sweep_{access}.png"
        combine_plots([plotout_read, plotout_write], savename, f"{access.capitalize()} Access Block Size Sweep",
                      1, 2)

    # Read Write Ratios Data

    read_write_json = dr.open_json(DATAPATH+"read_write_sweep.json")
    extracted_data = dr.extract_performance_data_from_json(read_write_json)


    # 100% Read Data
    read_lat = [extracted_data["100 R"]["read"]["lat_mean"], extracted_data["100 R"]["read"]["lat_var"]]
    read_bw = [extracted_data["100 R"]["read"]["bw_mean"], extracted_data["100 R"]["read"]["bw_var"]]

    # 100% Write Data
    write_lat = [extracted_data["100 W"]["write"]["lat_mean"], extracted_data["100 W"]["write"]["lat_var"]]
    write_bw = [extracted_data["100 W"]["write"]["bw_mean"], extracted_data["100 W"]["write"]["bw_var"]]

    # 70/30 Read Write Ratio
    read_write_70_30_lat_read = [extracted_data["70R 30W"]["read"]["lat_mean"], extracted_data["70R 30W"]["read"]["lat_var"]]
    read_write_70_30_lat_write = [extracted_data["70R 30W"]["write"]["lat_mean"], extracted_data["70R 30W"]["write"]["lat_var"]]
    read_write_70_30_bw_read = [extracted_data["70R 30W"]["read"]["bw_mean"], extracted_data["70R 30W"]["read"]["bw_var"]]
    read_write_70_30_bw_write = [extracted_data["70R 30W"]["write"]["bw_mean"], extracted_data["70R 30W"]["write"]["bw_var"]]

    # Now take the combined mean and variances
    read_write_70_30_lat = [read_write_70_30_lat_read[0]*.7 + 0.3*read_write_70_30_lat_write[0],
                            dr.get_weighted_var(70/30, read_write_70_30_lat_read[1], read_write_70_30_lat_write[1])]
    read_write_70_30_bw = [read_write_70_30_bw_read[0]*0.7 + 0.3*read_write_70_30_bw_write[0],
                            dr.get_weighted_var(70/30, read_write_70_30_bw_read[1],read_write_70_30_bw_write[1])]

    # 50/50 Read Write Ratio
    read_write_50_50_lat_read = [extracted_data["50R 50W"]["read"]["lat_mean"], extracted_data["50R 50W"]["read"]["lat_var"]]
    read_write_50_50_lat_write = [extracted_data["50R 50W"]["write"]["lat_mean"], extracted_data["50R 50W"]["write"]["lat_var"]]
    read_write_50_50_bw_read = [extracted_data["50R 50W"]["read"]["bw_mean"], extracted_data["50R 50W"]["read"]["bw_var"]]
    read_write_50_50_bw_write = [extracted_data["50R 50W"]["write"]["bw_mean"], extracted_data["50R 50W"]["write"]["bw_var"]]

    # Now take the combined mean and variances
    read_write_50_50_lat = [read_write_50_50_lat_read[0]*.5 + read_write_50_50_lat_write[0]*.5,
                            dr.get_weighted_var(1, read_write_50_50_lat_read[1], read_write_50_50_lat_write[1])]
    read_write_50_50_bw = [read_write_50_50_bw_read[0]*.5 + 0.5*read_write_50_50_bw_write[0],
                           dr.get_weighted_var(1, read_write_50_50_bw_read[1],read_write_50_50_bw_write[1])]

    lat_data = {"100:0" : read_lat, "70:30" : read_write_70_30_lat,
                "50:50" : read_write_50_50_lat, "0:100" :write_lat}
    bw_data = {"100:0" : read_bw, "70:30" : read_write_70_30_bw,
               "50:50" : read_write_50_50_bw, "0:100" :write_bw}

    lat_plot = ph.plot_single_bar_graph(lat_data, "Read/Write Ratio", "Latency (us)",
                                        "Latency Measurements for various Read/Write Ratios")
    bw_plot = ph.plot_single_bar_graph(bw_data, "Read/Write Ratio", "Bandwidth (MB/s)",
                                        plot_title="Bandwidth Measurements for various Read/Write Ratios")

    rw_ratio_savename_lat = PLOTPATH+"read_write_ratio_plots_latency.png"
    rw_ratio_savename_bw = PLOTPATH+"read_write_ratio_plots_bandwidth.png"

    lat_plot[0].savefig(rw_ratio_savename_lat)
    bw_plot[0].savefig(rw_ratio_savename_bw)

    #combine_plots([lat_plot, bw_plot], rw_ratio_savename,
    #              "How Read/Write Ratios impact memory latency and bandwidth", 1, 2)

    # Queue Depth Plots
    parallelism_json = dr.open_json(DATAPATH+"queue_depth.json")
    extracted_data = dr.extract_performance_data_from_json(parallelism_json)

    queue_sizes = [1, 2, 4, 8, 16, 32]

    read_latencies = [extracted_data["Q"+str(s)]["read"]["lat_mean"] for s in queue_sizes]
    write_latencies = [extracted_data["Q"+str(s)]["write"]["lat_mean"] for s in queue_sizes]

    read_bws = [extracted_data["Q"+str(s)]["read"]["bw_mean"] for s in queue_sizes]
    write_bws = [extracted_data["Q"+str(s)]["write"]["bw_mean"] for s in queue_sizes]


    # Generate plots of read / write littles law curves
    read_curve = ph.plot_single_line_graph(read_latencies, read_bws, "Latency (us)", "Throughput (MB/s)",
                                           "Read Plot")
    write_curve = ph.plot_single_line_graph(write_latencies, write_bws, "Latency (us)", "Throughput (MB/s)",
                                           "Write Plot")

    savename = PLOTPATH+"littles_law_curves.png"

    combine_plots([read_curve, write_curve], savename, "Throughput vs Latency Curves when varying queue depth",
                  1, 2)

    # Plot the tail latency table

    queue_sizes = [1,2,4,8,16,32]

    column_data_read = {
        "50th Percentile" : {"Q"+str(qsize) : extracted_data["Q"+str(qsize)]["read"]["lat_p50"] for qsize in queue_sizes},
        "95th Percentile" : {"Q"+str(qsize) : extracted_data["Q"+str(qsize)]["read"]["lat_p95"] for qsize in queue_sizes},
        "99th Percentile" : {"Q"+str(qsize) : extracted_data["Q"+str(qsize)]["read"]["lat_p99"] for qsize in queue_sizes},
        "99.9th Percentile" : {"Q"+str(qsize) : extracted_data["Q"+str(qsize)]["read"]["lat_p99.9"] for qsize in queue_sizes}
    }
    row_data_read = {f"Queue Depth = {qd}" : f'Q{qd}' for qd in queue_sizes}
    percentile_savename_read = PLOTPATH+"tail_alignment_table_read.txt"

    ph.save_md_table_to_file(column_data_read, row_data_read, percentile_savename_read)

    column_data_write = {
        "50th Percentile" : {"Q"+str(qsize) : extracted_data["Q"+str(qsize)]["write"]["lat_p50"] for qsize in queue_sizes},
        "95th Percentile" : {"Q"+str(qsize) : extracted_data["Q"+str(qsize)]["write"]["lat_p95"] for qsize in queue_sizes},
        "99th Percentile" : {"Q"+str(qsize) : extracted_data["Q"+str(qsize)]["write"]["lat_p99"] for qsize in queue_sizes},
        "99.9th Percentile" : {"Q"+str(qsize) : extracted_data["Q"+str(qsize)]["write"]["lat_p99.9"] for qsize in queue_sizes}
    }
    row_data_write = {f"Queue Depth = {qd}" : f'Q{qd}' for qd in queue_sizes}
    percentile_savename_write = PLOTPATH+"tail_alignment_table_write.txt"

    ph.save_md_table_to_file(column_data_write, row_data_read, percentile_savename_write)



main()
