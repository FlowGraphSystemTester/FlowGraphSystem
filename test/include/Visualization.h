//
// Created by M.Stanek on 10.08.2026.
//
#pragma once

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

