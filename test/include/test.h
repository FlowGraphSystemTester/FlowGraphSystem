//
// Created by M.Stanek on 10.08.2026.
//

#ifndef KONWERSJAGRAFOW_TEST2_H
#define KONWERSJAGRAFOW_TEST2_H

#include <map>

#include "FlowBoundedEdge.h"
#include "PP_to_G2.h"
#include "boost/json.hpp"

namespace json = boost::json;

using PP = std::vector<int>;
using AdjacencyListT = std::vector<std::vector<int>>;
using NetAdjacencyListT = std::vector<std::vector<FlowBoundedEdge>>;
using RevIdMapT = std::map<int, std::string>;

struct TC_G1_to_PP {
    std::string     name        { };
    AdjacencyListT  in_graph    { };
    std::vector<PP> expected_pps{ };

    std::string     to_json_string();
    json::object    to_json_object();

};

struct TCR_G1_to_PP {
    TC_G1_to_PP     test_case;
    std::vector<PP> output_pps          { };
    std::vector<PP> missing_pps         { };
    std::vector<PP> unexpected_pps      { };
    std::chrono::milliseconds duration  { };
    bool            success         { false };

    std::string to_json_string();
    json::object to_json_object();
};

struct TC_G2_to_G3 {
    std::string     name            { };
    AdjacencyListT  in_graph        { };
    std::vector<PP> in_pps          { };
    AdjacencyListT  expected_graph  { };

    std::string to_json_string();
    json::object to_json_object();
};

struct TCR_G2_to_G3 {
    TC_G2_to_G3     test_case;
    AdjacencyListT  output_graph  { };
    AdjacencyListT  diff_graph  { };
    std::chrono::milliseconds duration  { };
    bool            success     { false };

    std::string     to_json_string();
    json::object    to_json_object();
};

struct TS_G1_to_PP {
    std::string     name            { };
    std::vector<TC_G1_to_PP> cases  { };

    std::string     to_json_string();
    json::object    to_json_object();
};

struct TSR_G1_to_PP {
    TS_G1_to_PP                 test_suite;
    std::vector<TCR_G1_to_PP>   results  { };
    std::chrono::milliseconds   duration { };
    bool                        success  { false };

    std::string to_json_string();
    void        to_file(std::string output_path);
};

struct TS_G2_to_G3 {
    std::string     name            { };
    std::vector<TC_G2_to_G3> cases  { };
    std::string     to_json_string();
    json::object    to_json_object();
};

struct TSR_G2_to_G3 {
    TS_G2_to_G3 &               test_suite;
    std::vector<TCR_G2_to_G3>   results  { };
    std::chrono::milliseconds   duration { };
    bool                        success  { false };
    std::string to_json_string();
    void        to_file(std::string output_path);
};


TCR_G1_to_PP test_G1_to_PP(TC_G1_to_PP &test_case);
TCR_G2_to_G3 test_G2_to_G3(TC_G2_to_G3 &test_case);

TC_G1_to_PP from_json_tc_G1_to_PP(std::string json_string);
TC_G2_to_G3 from_json_tc_G2_to_G3(std::string json_string);

TC_G1_to_PP from_file_tc_G1_to_PP(std::string json_path);
TC_G2_to_G3 from_file_tc_G2_to_G3(std::string json_path);

TS_G1_to_PP from_json_ts_G1_to_PP(std::string json_string, int first_n);
TS_G2_to_G3 from_json_ts_G2_to_G3(std::string json_string);

TS_G1_to_PP from_file_ts_G1_to_PP(std::string json_path, int first_n = -1);
TS_G2_to_G3 from_file_ts_G2_to_G3(std::string json_path);

TSR_G1_to_PP run_ts_G1_to_PP(TS_G1_to_PP &test_suite, bool log = false, std::string log_dir = "./log", int first_n = -1, int skip_larger_than = -1);
TSR_G2_to_G3 run_ts_G2_to_G3(TS_G2_to_G3 &test_suite, bool log = false, std::string log_dir = "./log", int first_n = -1, int skip_larger_than = -1);


std::string adj_to_dot(AdjacencyListT &adj_list);
std::string net_to_dot(NetAdjacencyListT &adj_list);
std::string net_to_dot_named(NetAdjacencyListT &adj_list, RevIdMapT path_map);
std::string path_to_dot(PP &path);

RevIdMapT identify_names_alg2(TCR_G2_to_G3 &tcr);
RevIdMapT identify_names_PP_to_G2(TCR_PP_to_G2 &tcr);
RevIdMapT identify_names_PP_to_G2_map(AdjacencyListT &adj, int path_num);
RevIdMapT update_map_with_vertices(AdjacencyListT &adj, RevIdMapT rev_id_map);

std::string adj_to_dot_named(AdjacencyListT &adj_list, RevIdMapT rev_id_map);

void svg_from_dot(std::string dot_str, std::string output_path);
void png_from_dot(std::string dot_str, std::string output_path);


std::string result_to_json_tc_G1_to_PP(TCR_G1_to_PP &case_result);
std::string result_to_json_ts_G1_to_PP(TSR_G1_to_PP &suite_result);


#endif //KONWERSJAGRAFOW_TEST2_H
