@echo off

set data_header=terse_version_3;fio_version;jobname;groupid;error;read_kb;read_bandwidth_kb;read_iops;read_runtime_ms;read_slat_min_us;read_slat_max_us;read_slat_mean_us;read_slat_dev_us;read_clat_min_us;read_clat_max_us;read_clat_mean_us;read_clat_dev_us;read_clat_pct01;read_clat_pct02;read_clat_pct03;read_clat_pct04;read_clat_pct05;read_clat_pct06;read_clat_pct07;read_clat_pct08;read_clat_pct09;read_clat_pct10;read_clat_pct11;read_clat_pct12;read_clat_pct13;read_clat_pct14;read_clat_pct15;read_clat_pct16;read_clat_pct17;read_clat_pct18;read_clat_pct19;read_clat_pct20;read_tlat_min_us;read_lat_max_us;read_lat_mean_us;read_lat_dev_us;read_bw_min_kb;read_bw_max_kb;read_bw_agg_pct;read_bw_mean_kb;read_bw_dev_kb;write_kb;write_bandwidth_kb;write_iops;write_runtime_ms;write_slat_min_us;write_slat_max_us;write_slat_mean_us;write_slat_dev_us;write_clat_min_us;write_clat_max_us;write_clat_mean_us;write_clat_dev_us;write_clat_pct01;write_clat_pct02;write_clat_pct03;write_clat_pct04;write_clat_pct05;write_clat_pct06;write_clat_pct07;write_clat_pct08;write_clat_pct09;write_clat_pct10;write_clat_pct11;write_clat_pct12;write_clat_pct13;write_clat_pct14;write_clat_pct15;write_clat_pct16;write_clat_pct17;write_clat_pct18;write_clat_pct19;write_clat_pct20;write_tlat_min_us;write_lat_max_us;write_lat_mean_us;write_lat_dev_us;write_bw_min_kb;write_bw_max_kb;write_bw_agg_pct;write_bw_mean_kb;write_bw_dev_kb;cpu_user;cpu_sys;cpu_csw;cpu_mjf;cpu_minf;iodepth_1;iodepth_2;iodepth_4;iodepth_8;iodepth_16;iodepth_32;iodepth_64;lat_2us;lat_4us;lat_10us;lat_20us;lat_50us;lat_100us;lat_250us;lat_500us;lat_750us;lat_1000us;lat_2ms;lat_4ms;lat_10ms;lat_20ms;lat_50ms;lat_100ms;lat_250ms;lat_500ms;lat_750ms;lat_1000ms;lat_2000ms;lat_over_2000ms;disk_name;disk_read_iops;disk_write_iops;disk_read_merges;disk_write_merges;disk_read_ticks;write_ticks;disk_queue_time;disk_util

echo -- Zero Queue Baseline Data Collection --

REM echo %data_header% > raw_data/data_analysis/zero_queue_baseline_data.txt
fio.exe --output-format=json zq_baseline_jobs.fio > raw_data/data_analysis/zero_queue_baseline_data.json

echo -- Block Size Sweep Data Collection --

REM echo %data_header% > raw_data/data_analysis/block_size_sweep_random.txt
fio.exe --output-format=json block_size_sweep_random.fio > raw_data/data_analysis/block_size_sweep_random.json

REM echo %data_header% > raw_data/data_analysis/block_size_sweep_sequential.txt
fio.exe --output-format=json block_size_sweep_sequential.fio > raw_data/data_analysis/block_size_sweep_sequential.json


echo -- Read/Write Sweep Data Collection --

REM echo %data_header% > raw_data/data_analysis/read_write_sweep.txt
fio.exe --output-format=json read_write_sweep.fio > raw_data/data_analysis/read_write_sweep.json

echo -- Queue Depth Data Collection --

REM echo %data_header% > raw_data/data_analysis/queue_depth.txt
fio.exe --output-format=json queue_depth.fio > raw_data/data_analysis/queue_depth.json

echo -- Tail Latency Data Collection --

REM echo %data_header% > raw_data/data_analysis/tail_latency.txt
fio.exe --output-format=json tail_latency.fio > raw_data/data_analysis/tail_latency.json