//
// Created by M.Stanek on 21.08.2026.
//

#include "PP_to_G2.h"
#include "Visualization.h"

#include <iostream>

#include "Graph.h"
#include "PrimePathEdgeGen.h"


TCR_PP_to_G2 test_PP_to_G2(TC_PP_to_G2 &test_case) {
    TCR_PP_to_G2 tcr {
        .test_case = test_case,
        .output_graph = {},
        .diff_graph = {},
        .success = false
    };

    std::cout << "Initialising graph" << std::endl;
    Graph g1(test_case.in_graph.size());


    for (int node = 0; node < test_case.in_graph.size(); node++) {
        for (auto neighbor : test_case.in_graph[node]) {
            g1.addEdge(node, neighbor);
        }
    }

    std::cout << "Initialising PrimePathEdgeGen..." << std::endl;

    auto begin = std::chrono::high_resolution_clock::now();

    PrimePathEdgeGen prime_path_edge_gen(g1, test_case.in_pps.size());
    Graph g2 = prime_path_edge_gen.createTransitionEdgesFromPP(test_case.in_pps);

    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    tcr.duration = time;

    tcr.output_graph = g2.get_adjacency_list();

    if (tcr.output_graph == test_case.expected_graph) {
        tcr.success = true;
    }
    else {
        tcr.success = false;
    }

    return tcr;
}

std::string TS_PP_to_G2::to_json_string() {
    return json::serialize(this->to_json_object());
};

json::object TCR_PP_to_G2::to_json_object() {
    json::object res_obj;
    res_obj["test_case"] = test_case.to_json_object();

    json::array output_graph_array {};
    for (auto n : output_graph) {
        json::array neighbors {};
        for (auto neighbor : n) {
            neighbors.push_back(neighbor);
        }
        output_graph_array.push_back(neighbors);
    }

    res_obj["output_graph"] = output_graph_array;

    json::array diff_graph_array {};
    for (auto n : diff_graph) {
        json::array neighbors {};
        for (auto neighbor : n) {
            neighbors.push_back(neighbor);
        }
        diff_graph_array.push_back(neighbors);
    }

    res_obj["diff_graph"] = diff_graph_array;

    res_obj["duration"] = duration.count();
    res_obj["success"] = success;

    return res_obj;
}


std::string TCR_PP_to_G2::to_json_string() {
    return json::serialize(this->to_json_object());
}


std::string TSR_PP_to_G2::to_json_string() {
    json::object res_obj {};

    json::object ts_obj {};
    ts_obj["name"] = test_suite.name;
    json::array test_suite_array {};
    for (auto tc : test_suite.cases) {
        test_suite_array.push_back(tc.to_json_object());
    }

    res_obj["test_suite"] = ts_obj;

    json::array results_array {};
    for (auto r : results) {
        results_array.push_back(r.to_json_object());
    }

    res_obj["results"] = results_array;

    res_obj["duration"] = duration.count();
    res_obj["success"] = success;

    return json::serialize(res_obj);
}

json::object TS_PP_to_G2::to_json_object() {
    json::object res_obj {};

    res_obj["name"] = name;

    json::array cases_array {};
    std::cout << "Making objects from cases" << std::endl;
    int counter = 0;
    for (auto c : cases) {
        cases_array.push_back(c.to_json_object());
        if (counter++ % 500 == 0) {
            std::cout << counter << std::endl;
        }
    }
    res_obj["cases"] = cases_array;

    return res_obj;
}

json::object TC_PP_to_G2::to_json_object() {
    json::object res_obj;
    res_obj["name"] = name;

    json::array in_graph_array {};
    for (auto v : in_graph) {
        json::array neighbors {};
        for (auto n : v) {
            neighbors.push_back(n);
        }
        in_graph_array.push_back(neighbors);
    }

    res_obj["in_graph"] = in_graph_array;

    json::array in_pps_array {};
    for (auto p : in_pps) {
        json::array path {};
        for (auto v : p) {
            path.push_back(v);
        }
        in_pps_array.push_back(path);
    }

    res_obj["in_pps"] = in_pps_array;

    json::array expected_graph_array {};
    for (auto v : expected_graph) {
        json::array neighbors {};
        for (auto n : v) {
            neighbors.push_back(n);
        }
        expected_graph_array.push_back(neighbors);
    }
    res_obj["expected_graph"] = expected_graph_array;

    return res_obj;
}

TC_PP_to_G2 from_json_tc_PP_to_G2(std::string json_string) {
    using namespace boost;

    auto json_v = json::parse(json_string);
    auto json_obj = json_v.as_object();
    TC_PP_to_G2 tc {
        .name = value_to<std::string>(json_obj.at("name")),
        .in_graph = value_to<std::vector<std::vector<int>>>(json_obj.at("in_graph")),
        .in_pps = value_to<std::vector<std::vector<int>>>(json_obj.at("in_pps")),
        .expected_graph = value_to<std::vector<std::vector<int>>>(json_obj.at("expected_graph"))
    };

    return tc;
}


TS_PP_to_G2 from_json_ts_PP_to_G2(std::string json_string) {
    using namespace boost;

    auto json_v = json::parse(json_string);
    auto json_obj = json_v.as_object();

    TS_PP_to_G2 ts {
        .name = value_to<std::string>(json_obj.at("name")),
        .cases = {}
    };

    std::cout << std::format("Loading {}", ts.name) << std::endl;
    std::cout << std::format("Loading {} cases.", ts.cases.size()) << std::endl;


    json::array cases = json_obj.at("cases").as_array();
    size_t counter {0};
    size_t total_cases { cases.size() };
    for (auto c : cases) {
        std::cout << std::format("Loading case no. {}/{}", counter++, total_cases) << std::endl;
        std::string cs = json::serialize(c);
        ts.cases.push_back(from_json_tc_PP_to_G2(cs));
    }

    return ts;
}

TS_PP_to_G2 from_file_ts_PP_to_G2(std::string json_path) {
    std::ifstream ifs(json_path);
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string s = ss.str();
    TS_PP_to_G2 ts = from_json_ts_PP_to_G2(ss.str());

    return ts;
}

TSR_PP_to_G2 run_ts_PP_to_G2(TS_PP_to_G2 & test_suite, bool log, std::string log_dir, int first_n, int skip_larger_than) {
    using namespace boost;
    std::cout << "Running test suite..." << std::endl;
    TSR_PP_to_G2 ts {
        .test_suite = test_suite,
        .results = {},
        .duration {},
        .success = true
    };



    int tc_i = 0;
    for (auto c : test_suite.cases) {
        //std::cout << "Running case: " << c.name << std::endl;
        if (c.in_graph.size() <= skip_larger_than || skip_larger_than < 0) {
            TCR_PP_to_G2 res = test_PP_to_G2(c);
            if (!res.success) ts.success = false;
            ts.results.push_back(res);
            auto formatted = std::format("[{}/{}] {} time: {} name: {}...", tc_i, test_suite.cases.size() - 1, res.success ? "PASS" : "FAIL"  , res.duration.count(), c.name.substr(0, 10));
            std::cout << formatted << std::endl;
            ts.duration += res.duration;
        }
        else {
            std::cout << "Skipped case. Graph too large." << std::endl;
        }
        if (tc_i++ == first_n) {break;}
    }

    return ts;
}