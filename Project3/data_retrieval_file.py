import json as json
import math as math

def get_weighted_var(n1n2ratio : float, v1 : float, v2: float):

    vsq = math.pow(v1, 2)
    v2sq = math.pow(v2, 2)

    varsq = (n1n2ratio*(vsq+v2sq))/(n1n2ratio+1)

    return math.sqrt(varsq)

def open_json(filename: str):
    with open(filename, "r") as f:
        return json.load(f)

def extract_performance_data_from_json(json_dict) -> dict[str, dict[str, dict[str, float]]]:

    all_data = json_dict

    ret_data = dict()

    job_data=all_data["jobs"]

    for job in job_data:
        ret_data[job["jobname"]] = dict()

        ret_job_data = ret_data[job["jobname"]]


        ret_job_data["read"] = dict()
        ret_job_data["write"] = dict()
        ret_job_data["general"] = dict()

        ret_rdata = ret_job_data["read"]
        ret_wdata = ret_job_data["write"]


        if job["read"]["io_bytes"] != 0:
            readdict = job["read"]

            ret_rdata["lat_mean"] = readdict["clat_ns"]["mean"]/1000
            ret_rdata["lat_var"] = readdict["clat_ns"]["stddev"]/1000
            ret_rdata["lat_p50"] = readdict["clat_ns"]["percentile"]["50.000000"]/1000
            ret_rdata["lat_p95"] = readdict["clat_ns"]["percentile"]["95.000000"]/1000
            ret_rdata["lat_p99"] = readdict["clat_ns"]["percentile"]["99.000000"]/1000
            ret_rdata["lat_p99.9"] = readdict["clat_ns"]["percentile"]["99.900000"]/1000
            ret_rdata["iops_mean"] = readdict["iops_mean"]
            ret_rdata["iops_var"] = readdict["iops_stddev"]
            ret_rdata["bw_mean"] = readdict["bw_mean"]/1000
            ret_rdata["bw_var"] = readdict["bw_dev"]/1000

        if job["write"]["io_bytes"] != 0:
            writedict = job["write"]

            # Convert to us for latency
            ret_wdata["lat_mean"] = writedict["clat_ns"]["mean"]/1000
            ret_wdata["lat_var"] = writedict["clat_ns"]["stddev"]/1000
            ret_wdata["lat_p50"] = writedict["clat_ns"]["percentile"]["50.000000"]/1000
            ret_wdata["lat_p95"] = writedict["clat_ns"]["percentile"]["95.000000"]/1000
            ret_wdata["lat_p99"] = writedict["clat_ns"]["percentile"]["99.000000"]/1000
            ret_wdata["lat_p99.9"] = writedict["clat_ns"]["percentile"]["99.900000"]/1000
            ret_wdata["iops_mean"] = writedict["iops_mean"]
            ret_wdata["iops_var"] = writedict["iops_stddev"]
            # BW in kb/s so make in MB/s
            ret_wdata["bw_mean"] = writedict["bw_mean"]/1000
            ret_wdata["bw_var"] = writedict["bw_dev"]/1000

    return ret_data

def get_blocksizes_from_json_data(json_dict) -> dict[str, int]:

    sizes_list = {}

    for job in json_dict["jobs"]:
        size_of_bs = job["job options"]["bs"]

        integral_size = size_of_bs[:-1]
        if size_of_bs[-1] == "m":
            integral_size *= 1000

        sizes_list[job["jobname"]] = integral_size
    return sizes_list




