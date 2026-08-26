//
// Created by Kacper Smykla on 26.08.2026.
//
#include "G1_to_PP.h"
#include "PrimePathGen.h"

TCR_G1_to_PP test_G1_to_PP(TC_G1_to_PP &test_case) {
    TCR_G1_to_PP tcr {
        .test_case = test_case,
        .missing_pps = {},
        .unexpected_pps = {},
        .success = false
    };

    std::cout << "Initialising graph." << std::endl;
    Graph g1(test_case.in_graph.size());


    for (int node = 0; node < test_case.in_graph.size(); node++) {
        for (auto neighbor : test_case.in_graph[node]) {
            g1.addEdge(node, neighbor);
        }
    }


    auto begin = std::chrono::high_resolution_clock::now();

    PrimePathGen primePathGen(g1);
    auto paths = primePathGen.generatePaths();

    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    tcr.duration = time;
    tcr.output_pps = paths;

    for (auto path : tcr.output_pps) {
        if (std::find(tcr.test_case.expected_pps.begin(), tcr.test_case.expected_pps.end(), path) == tcr.test_case.expected_pps.end()) {
            tcr.unexpected_pps.push_back(path);
        }
    }

    for (auto epath : tcr.test_case.expected_pps) {
        if (std::find(tcr.output_pps.begin(), tcr.output_pps.end(), epath) == tcr.output_pps.end()) {
            tcr.missing_pps.push_back(epath);
        }
    }

    if (tcr.unexpected_pps.size() > 0 || tcr.missing_pps.size() > 0) {
        tcr.success = false;
    }
    else {
        tcr.success = true;
    }

    return tcr;
}

TC_G1_to_PP from_json_tc_G1_to_PP(std::string json_string) {
    using namespace boost;

    auto json_v = json::parse(json_string);
    auto json_obj = json_v.as_object();
    TC_G1_to_PP tc {
        .name = value_to<std::string>(json_obj.at("name")),
        .in_graph = value_to<std::vector<std::vector<int>>>(json_obj.at("in_graph")), //in_graph
        .expected_pps = value_to<std::vector<std::vector<int>>>(json_obj.at("expected_pps")), //expected_pps
    };

    return tc;
}

TC_G1_to_PP from_file_tc_G1_to_PP(std::string json_path) {
    std::ifstream ifs(json_path);
    std::stringstream ss;
    ss << ifs.rdbuf();
    TC_G1_to_PP tc = from_json_tc_G1_to_PP(ss.str());

    return tc;
}

TC_G1_to_PP from_json_value_tc_G1_to_G2(json::value json_v) {
    using namespace boost;

    auto json_obj = json_v.as_object();
    TC_G1_to_PP tc {
        .name = value_to<std::string>(json_obj.at("name")),
        .in_graph = value_to<std::vector<std::vector<int>>>(json_obj.at("in_graph")), //in_graph
        .expected_pps = value_to<std::vector<std::vector<int>>>(json_obj.at("expected_pps")), //expected_pps
    };

    return tc;
}

TS_G1_to_PP from_json_ts_G1_to_PP(std::string json_string, int first_n) {
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
        ts.cases.push_back(from_json_value_tc_G1_to_G2(c));
        if (counter == first_n && first_n != -1) {break;}
    }
    std::cout << "Returning cases..." << std::endl;
    return ts;
}

TS_G1_to_PP from_file_ts_G1_to_PP(std::string json_path, int first_n) {
    std::ifstream ifs(json_path);
    std::stringstream ss;
    ss << ifs.rdbuf();
    TS_G1_to_PP ts = from_json_ts_G1_to_PP(ss.str(), first_n);
    std::cout<<"Returning test suite..." << std::endl;

    return ts;
}

TSR_G1_to_PP run_ts_G1_to_PP(TS_G1_to_PP & test_suite, bool log, std::string log_dir, int first_n, int skip_larger_than) {
    using namespace boost;
    std::cout << "Running test suite..." << std::endl;
    TSR_G1_to_PP ts {
        .test_suite = test_suite,
        .results = {},
        .duration {},
        .success = true
    };



    int tc_i = 0;
    for (auto c : test_suite.cases) {
        //std::cout << "Running case: " << c.name << std::endl;
        if (c.in_graph.size() <= skip_larger_than || skip_larger_than < 0) {
            TCR_G1_to_PP res = test_G1_to_PP(c);
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

json::object TC_G1_to_PP::to_json_object() {
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

    json::array expected_pps_array {};
    for (auto p : expected_pps) {
        json::array path {};
        for (auto v : p) {
            path.push_back(v);
        }
        expected_pps_array.push_back(path);
    }

    res_obj["expected_pps"] = expected_pps_array;
    return res_obj;
}


std::string TC_G1_to_PP::to_json_string() {
    return json::serialize(this->to_json_object());
}

json::object TCR_G1_to_PP::to_json_object() {
    json::object res_obj;
    res_obj["test_case"] = json::parse(test_case.to_json_string());

    json::array output_pps_array {};
    for (auto p : output_pps) {
        json::array path {};
        for (auto v : p) {
            path.push_back(v);
        }
        output_pps_array.push_back(path);
    }

    res_obj["output_pps"] = output_pps_array;

    json::array missing_pps_array {};
    for (auto p : missing_pps) {
        json::array path {};
        for (auto v : p) {
            path.push_back(v);
        }
        missing_pps_array.push_back(path);
    }

    res_obj["missing_pps"] = missing_pps_array;

    json::array unexpected_pps_array {};
    for (auto p : unexpected_pps) {
        json::array path {};
        for (auto v : p) {
            path.push_back(v);
        }
        unexpected_pps_array.push_back(path);
    }

    res_obj["unexpected_pps"] = unexpected_pps_array;

    res_obj["duration"] = duration.count();
    res_obj["success"] = success;

    return res_obj;
}


std::string TCR_G1_to_PP::to_json_string() {
    return json::serialize(this->to_json_object());
}

json::object TS_G1_to_PP::to_json_object() {
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

std::string TS_G1_to_PP::to_json_string() {
    return json::serialize(this->to_json_object());
}

std::string TSR_G1_to_PP::to_json_string() {
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

void TSR_G1_to_PP::to_file(std::string output_path) {
    std::ofstream ofs(output_path);
    ofs << to_json_string();
}

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