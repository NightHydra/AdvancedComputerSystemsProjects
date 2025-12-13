$echo off

echo Starting vtune tests
set "current_dir=%CD%"
vtune.lnk -collect memory-access -r %current_dir%\vtune_log_results\WS_1000 -- %current_dir%\cmake-build-release\WALK_SIZE_1000.exe
vtune.lnk -report hw-events -result-dir %current_dir%\vtune_log_results\WS_1000 -report-output %current_dir%\vtune_results\walk_size_1000 -format csv -csv-delimiter comma

vtune.lnk -collect memory-access -r %current_dir%\vtune_log_results\WS_16000 -- %current_dir%\cmake-build-release\WALK_SIZE_16000.exe
vtune.lnk -report hw-events -result-dir %current_dir%\vtune_log_results\WS_16000 -report-output %current_dir%\vtune_results\walk_size_16000 -format csv -csv-delimiter comma

vtune.lnk -collect memory-access -r %current_dir%\vtune_log_results\WS_2000000 -- %current_dir%\cmake-build-release\WALK_SIZE_2000000.exe
vtune.lnk -report hw-events -result-dir %current_dir%\vtune_log_results\WS_2000000 -report-output %current_dir%\vtune_results\walk_size_2000000 -format csv -csv-delimiter comma

vtune.lnk -collect memory-access -r %current_dir%\vtune_log_results\WS_2560000 -- %current_dir%\cmake-build-release\WALK_SIZE_2560000.exe
vtune.lnk -report hw-events -result-dir %current_dir%\vtune_log_results\WS_2560000 -report-output %current_dir%\vtune_results\walk_size_2560000 -format csv -csv-delimiter comma

vtune.lnk -collect memory-access -r %current_dir%\vtune_log_results\WS_10000000 -- %current_dir%\cmake-build-release\WALK_SIZE_10000000.exe
vtune.lnk -report hw-events -result-dir %current_dir%\vtune_log_results\WS_10000000 -report-output %current_dir%\vtune_results\walk_size_10000000 -format csv -csv-delimiter comma

vtune.lnk -collect memory-access -r %current_dir%\vtune_log_results\WS_50000000 -- %current_dir%\cmake-build-release\WALK_SIZE_50000000.exe
vtune.lnk -report hw-events -result-dir %current_dir%\vtune_log_results\WS_50000000 -report-output %current_dir%\vtune_results\walk_size_50000000 -format csv -csv-delimiter comma