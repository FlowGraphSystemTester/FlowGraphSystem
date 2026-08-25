//
// Created by M.Stanek on 19.08.2026.
//

#include <fstream>

#include "test.h"
#include <iostream>
#include <ostream>
#include <sstream>


TC_G1_to_PP uj_from_json_tc(std::string json_string) {
    using namespace boost;

    auto json_v = json::parse(json_string);
    auto json_obj = json_v.as_object();
    TC_G1_to_PP tc {
        .name = value_to<std::string>(json_obj.at("name")),
        .in_graph = value_to<std::vector<std::vector<int>>>(json_obj.at("input_graph")), //in_graph
        .expected_pps = value_to<std::vector<std::vector<int>>>(json_obj.at("expected_graph")), //expected_pps
    };

    return tc;
}

TS_G1_to_PP uj_from_json_ts(std::string json_string, int first_n) {
    using namespace boost;

    auto json_v = json::parse(json_string);
    auto json_obj = json_v.as_object();

    TS_G1_to_PP ts {
        .name = value_to<std::string>(json_obj.at("name")),
        .cases = {}
    };

    std::cout << std::format("Loading {}", ts.name) << std::endl;
    std::cout << std::format("Loading {} cases.", first_n) << std::endl;

    json::array cases = json_obj.at("cases").as_array();
    size_t counter {0};
    size_t total_cases { cases.size() };
    for (auto c : cases) {
        std::cout << std::format("Loading case no. {}/{}", counter++, total_cases) << std::endl;
        std::string cs = json::serialize(c);
        ts.cases.push_back(uj_from_json_tc(cs));
        if (counter == first_n && first_n != -1) {break;}
    }

    return ts;
}

TS_G1_to_PP uj_from_file_ts(std::string json_path, int first_n) {
    std::ifstream ifs(json_path);
    std::stringstream ss;
    ss << ifs.rdbuf();
    TS_G1_to_PP ts = uj_from_json_ts(ss.str(), first_n);

    return ts;
}

void suite_from_results_to_file(std::string input_file, std::string output_file) {
    TS_G1_to_PP ts1 = from_file_ts_G1_to_PP(input_file);
    // TS_G1_to_G2 ts1 = from_file_ts_G1_to_G2("C:/Users/M.Stanek/CLionProjects/pokrycie-testami/test/data/uj-30.json", -1);
    std::cout << "Loaded cases exited..." << std::endl;
    TSR_G1_to_PP tsr1 = run_ts_G1_to_PP(ts1, true, "");

    std::cout << "Creating new test suite" << std::endl;
    TS_G1_to_PP new_ts { .name ="uj"};
    for (auto r : tsr1.results) {
        TC_G1_to_PP c { .name = r.test_case.name };

        c.in_graph = r.test_case.in_graph;
        c.expected_pps = r.output_pps;
        new_ts.cases.push_back(c);
    }

    std::ofstream ofs(output_file);
    ofs << new_ts.to_json_string();
}