//
// Created by M.Stanek on 20.08.2026.
//
#include "pch.h"
#include <boost/json.hpp>

using namespace boost;

using AdjMapT = std::map<std::string, std::vector<std::string>>;
using IdMapT = std::map<std::string, int>;
using RevIdMapT = std::map<int, std::string>;

AdjMapT make_adj_map(json::object in_graph_obj) {
    AdjMapT result_map {};

    for (auto node_pair: in_graph_obj) {
        result_map[node_pair.key_c_str()] = value_to<std::vector<std::string>>(node_pair.value());
    }

    return result_map;
}

AdjMapT add_s_t(AdjMapT adj_map, std::string entry, std::string exit) {
    auto result_map = adj_map;
    result_map["s"] = {entry};
    result_map["t"] = {};
    result_map[exit].push_back("t");

    return result_map;
};

IdMapT get_id_map(AdjMapT adj_map) {
    IdMapT result_map {};

    result_map["s"] = 0;
    auto largest = 0;
    for (auto node : adj_map) {
        if (node.first != "s" && node.first != "t") {
            std::string node_name = node.first;
            std::erase(node_name, 'B');
            auto id = std::stoi(node_name) + 1;

            result_map[node.first] = id;

            if ( id > largest ) {
                largest = id;
            }
        }
    }
    for (auto node : result_map) {
        if (node.first != "s" && node.first != "t") {
            result_map[node.first] = largest + 1 - node.second;
        }
    }
    result_map["t"] = largest + 1;

    return result_map;
}

AdjacencyListT get_adj_int(AdjMapT adj_map, IdMapT id_map) {
    AdjacencyListT result_adj {};

    for (auto node : id_map) {
        result_adj.push_back({});
    }

    for (auto node : adj_map) {
        for (auto neighbor : node.second) {
            result_adj[id_map[node.first]].push_back(id_map[neighbor]);
        }
    }

    return result_adj;
}

RevIdMapT get_rev_id_map(IdMapT id_map) {
    RevIdMapT result_map {};

    for (auto node : id_map) {
        result_map[node.second] = node.first;
    }

    return result_map;
}

AdjacencyListT get_adj_list(json::object in_graph, std::string entry, std::string exit) {
    auto adj_map = make_adj_map(in_graph);
    adj_map = add_s_t(adj_map, entry, exit);
    auto id_map = get_id_map(adj_map);
    auto adj_list = get_adj_int(adj_map, id_map);

    return adj_list;
}

TC_G1_to_PP in_from_uj_TC_G1_to_G2(json::object cfg) {
    TC_G1_to_PP tc {
        .name = cfg["name"].as_string().c_str(),
        .in_graph = get_adj_list(
            cfg["graph"].as_object(),
               cfg["ENTRY"].as_array()[0].as_string().c_str(),
                cfg["EXIT"].as_array()[0].as_string().c_str()
                ),
        .expected_pps = {}
    };

    return tc;
}

TS_G1_to_PP in_from_uj_TS_G1_to_G2(json::value cfgs, int first_n) {
    TS_G1_to_PP uj = {
        .name = "UJ",
        .cases = {}
    };

    auto cfg_array = cfgs.as_array();
    auto num_of_cfgs = cfg_array.size();
    auto i = 0;
    for (auto cfg : cfg_array) {
        if (i == first_n) {
            break;
        }
        if (i % 300 == 0) {
            std::cout << "Parsing case " << i << "/" << num_of_cfgs << std::endl;
        }

        uj.cases.push_back(in_from_uj_TC_G1_to_G2(cfg.as_object()));
        ++i;
    }

    return uj;
}

TC_G1_to_PP gen_test_uj_TC_G1_to_G2(TC_G1_to_PP test_case) {
    TC_G1_to_PP tc {
        .name = test_case.name,
        .in_graph = test_case.in_graph,
        .expected_pps = {}
    };

    std::cout << "Initialising graph." << std::endl;
    Graph1 g1(tc.in_graph.size());

    for (int node = 0; node < test_case.in_graph.size(); node++) {
         for (auto neighbor : test_case.in_graph[node]) {
             g1.add_edge(node, neighbor);
         }
    }

    auto begin = std::chrono::high_resolution_clock::now();

    tc.expected_pps = g1.generate_paths();

    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    std::cout << "no. of PPs: " << tc.expected_pps.size() << std::endl;
    std::cout << "time: " << time.count() << " ms" << std::endl;

    return tc;
}


TS_G1_to_PP gen_from_uj_TS_G1_to_G2(TS_G1_to_PP test_suite, bool log, std::string log_dir, int first_n, int skip_larger_than) {
    using namespace boost;
    TS_G1_to_PP tc_result {
        .name = test_suite.name,
        .cases = {}
    };

    std::cout << "Generating test suite..." << std::endl;

    for (int i = 0; i < test_suite.cases.size(); i++) {
        std::cout << "Generating case no. " << i << " name: " << test_suite.cases[i].name << std::endl;
        std::cout << "Graph size: " << test_suite.cases[i].in_graph.size() << std::endl;
        if (test_suite.cases[i].in_graph.size() <= skip_larger_than || skip_larger_than < 0) {
            tc_result.cases.push_back(gen_test_uj_TC_G1_to_G2(test_suite.cases[i]));
        }
        else {
            std::cout << "Skipped case. Graph larger than " << skip_larger_than << std::endl;
        }
    }
    std::cout << "Got " << tc_result.cases.size() << " cases out of " << test_suite.cases.size() << std::endl;
    return tc_result;
}

