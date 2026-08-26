//
// Created by Kacper Smykla on 26.08.2026.
//
#pragma once
#include "Visualization.h"

TCR_G2_to_G3 test_G2_to_G3(TC_G2_to_G3 &test_case);
TC_G2_to_G3 from_json_tc_G2_to_G3(std::string json_string);
TC_G2_to_G3 from_file_tc_G2_to_G3(std::string json_path);
TS_G2_to_G3 from_json_ts_G2_to_G3(std::string json_string);
TS_G2_to_G3 from_file_ts_G2_to_G3(std::string json_path);
TSR_G2_to_G3 run_ts_G2_to_G3(TS_G2_to_G3 &test_suite, bool log = false, std::string log_dir = "./log", int first_n = -1, int skip_larger_than = -1);
