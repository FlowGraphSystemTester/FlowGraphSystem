//
// Created by Kacper Smykla on 26.08.2026.
//
#pragma once
#include "Visualization.h"

TCR_G1_to_PP test_G1_to_PP(TC_G1_to_PP &test_case);
TC_G1_to_PP from_json_tc_G1_to_PP(std::string json_string);
TC_G1_to_PP from_file_tc_G1_to_PP(std::string json_path);
TS_G1_to_PP from_json_ts_G1_to_PP(std::string json_string, int first_n);
TS_G1_to_PP from_file_ts_G1_to_PP(std::string json_path, int first_n = -1);
TSR_G1_to_PP run_ts_G1_to_PP(TS_G1_to_PP &test_suite, bool log = false, std::string log_dir = "./log", int first_n = -1, int skip_larger_than = -1);

std::string result_to_json_tc_G1_to_PP(TCR_G1_to_PP &case_result);
std::string result_to_json_ts_G1_to_PP(TSR_G1_to_PP &suite_result);