import json as json

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

        if "read" in job:
            readdict = job["read"]

            ret_rdata["lat_mean"] = readdict["clat_ns"]["mean"]/1000
            ret_rdata["lat_var"] = readdict["clat_ns"]["stddev"]/1000
            ret_rdata["lat_p50"] = readdict["clat_ns"]["percentile"]["50.000000"]/1000
            ret_rdata["lat_p95"] = readdict["clat_ns"]["percentile"]["95.000000"]/1000
            ret_rdata["lat_p99"] = readdict["clat_ns"]["percentile"]["99.000000"]/1000
            ret_rdata["lat_p99"] = readdict["clat_ns"]["percentile"]["99.000000"]/1000
            ret_rdata["iops_mean"] = readdict["iops_mean"]
            ret_rdata["iops_var"] = readdict["iops_stddev"]
            ret_rdata["bw_mean"] = readdict["bw_mean"]
            ret_rdata["bw_var"] = readdict["bw_dev"]

        if "write" in job:
            writedict = job["write"]

            ret_wdata["lat_mean"] = writedict["clat_ns"]["mean"]/1000
            ret_wdata["lat_var"] = writedict["clat_ns"]["stddev"]/1000
            ret_wdata["lat_p50"] = writedict["clat_ns"]["percentile"]["50.000000"]/1000
            ret_wdata["lat_p95"] = writedict["clat_ns"]["percentile"]["95.000000"]/1000
            ret_wdata["lat_p99"] = writedict["clat_ns"]["percentile"]["99.000000"]/1000
            ret_wdata["lat_p99"] = writedict["clat_ns"]["percentile"]["99.000000"]/1000
            ret_wdata["iops_mean"] = writedict["iops_mean"]
            ret_wdata["iops_var"] = writedict["iops_stddev"]
            ret_wdata["bw_mean"] = writedict["bw_mean"]
            ret_wdata["bw_var"] = writedict["bw_dev"]

    return ret_data

def get_blocksizes_from_json_data(json_dict) -> dict[str, int]:

    sizes_list = {}

    for job in json_dict["jobs"]:
        sizes_list[job["jobname"]] = int(job["job options"]["bs"])
    return sizes_list




