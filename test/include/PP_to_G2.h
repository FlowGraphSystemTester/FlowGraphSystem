//
// Created by M.Stanek on 21.08.2026.
//

#ifndef GRAPHCYCLEREDUCER_PP_TO_G2_H
#define GRAPHCYCLEREDUCER_PP_TO_G2_H

#include <vector>
#include <boost/json.hpp>
#include <map>

using namespace boost;

using PP = std::vector<int>;
using AdjacencyListT = std::vector<std::vector<int>>;
using RevIdMapT = std::map<int, std::string>;

struct TC_PP_to_G2 {
    std::string     name            { };
    AdjacencyListT  in_graph        { };
    std::vector<PP> in_pps          { };
    AdjacencyListT  expected_graph  { };

    std::string to_json_string();
    json::object to_json_object();
};

struct TCR_PP_to_G2 {
    TC_PP_to_G2     test_case { };
    AdjacencyListT  output_graph  { };
    AdjacencyListT  diff_graph  { };
    std::chrono::milliseconds duration  { };
    bool            success     { false };

    std::string     to_json_string();
    json::object    to_json_object();
};

struct TS_PP_to_G2 {
    std::string     name            { };
    std::vector<TC_PP_to_G2> cases  { };
    std::string     to_json_string();
    json::object    to_json_object();
};

struct TSR_PP_to_G2 {
    TS_PP_to_G2 &               test_suite;
    std::vector<TCR_PP_to_G2>   results  { };
    std::chrono::milliseconds   duration { };
    bool                        success  { false };
    std::string to_json_string();
    void        to_file(std::string output_path);
};

TCR_PP_to_G2 test_PP_to_G2(TC_PP_to_G2 &test_case);

TC_PP_to_G2 from_json_tc_PP_to_G2(std::string json_string);

TC_PP_to_G2 from_file_tc_PP_to_G2(std::string json_path);

TS_PP_to_G2 from_json_ts_PP_to_G2(std::string json_string);

TS_PP_to_G2 from_file_ts_PP_to_G2(std::string json_path);

TSR_PP_to_G2 run_ts_PP_to_G2(TS_PP_to_G2 &test_suite, bool log = false, std::string log_dir = "./log", int first_n = -1, int skip_larger_than = -1);
#endif //GRAPHCYCLEREDUCER_PP_TO_G2_H
