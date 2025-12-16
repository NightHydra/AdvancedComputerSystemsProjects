import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import copy as cp
import plotting_code as pc

DATA_FILES_BASE_PATH = "data_analysis/raw_data"
PLOT_FILES_PATH = "data_analysis/plots/"

def fetch_data_from_file(fname, metrics):

    datacol = pd.read_csv(fname, delimiter=",").astype(float)[metrics]

    return [datacol.median(), datacol.std()]

def fetch_perf_metric_for_zeroio():

    zero_copy_means = []
    zero_copy_stds = []

    copy_means = []
    copy_stds = []

    for s in ["small", "medium", "large", "extra_large"]:
        zc_datapoint = fetch_data_from_file(DATA_FILES_BASE_PATH + f'/zero_copy/{s}_output.csv', 'Zero-Copy Cycles')
        nc_datapoint = fetch_data_from_file(DATA_FILES_BASE_PATH + f'/zero_copy/{s}_output.csv', 'Normal-Copy Cyles')

        zero_copy_means.append(zc_datapoint[0])
        zero_copy_stds.append(zc_datapoint[1])

        copy_means.append(nc_datapoint[0])
        copy_stds.append(nc_datapoint[1])

    return [[copy_means, zero_copy_means], [copy_stds, zero_copy_stds]]

def main():
    ## ZERO COPY GRAPHS
    zc_data = fetch_perf_metric_for_zeroio()

    pc.plot_double_bar_graph(zc_data[0], zc_data[1], ["100 kb", "1 Mb", "10 Mb", "100 Mb"],
                             ["Normal", "Zero-Copy"], "File Size for Comparison",
                             "log_10(Cycles Taken)", "Speedup From Using Zero-Copy IO", "zero_copy/zero_copy_perf.png",
                             logscale=True)

main()

