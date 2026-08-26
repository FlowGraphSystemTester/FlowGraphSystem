//
// Created by M.Stanek on 19.08.2026.
//
#pragma once

#include "visualization.h"
#include "G1_to_PP.h"

#include "PrimePathEdgeGen.h"
#include "ConverterDAG.h"
#include "FlowGraphUtils.h"
#include "FlowNetwork.h"

inline void suite_from_results_to_file(std::string input_file, std::string output_file) {
    TS_G1_to_PP ts1 = from_file_ts_G1_to_PP(input_file);
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

constexpr std::string PROJECT_DIR = "..";

inline void draw_flow(TC_G2_to_G3 tc, int n) {
    Graph g1(tc.in_graph.size());

    for (int node = 0; node < tc.in_graph.size(); node++) {
        for (auto neighbor : tc.in_graph[node]) {
            g1.addEdge(node, neighbor);
        }
    }

    std::cout << "Generating flow for case: " + std::to_string(n) << std::endl;

    PrimePathEdgeGen prime_path_edge_gen(g1, tc.in_pps.size());
    Graph g2 = prime_path_edge_gen.createTransitionEdgesFromPP(tc.in_pps);
    ConverterDAG converter_dag(g2);
    Graph g3 = converter_dag.removeCycles();
    auto flowGraph = convertG3toFlowNetwork(g3);
    const int source = g2.verticesCount -2;
    const int sink = g2.verticesCount -1;
    int min = flowGraph.sendMinimumFlow(source,sink);
    std::cout << "\nMinimum flow: " << min << std::endl << std::endl;

    std::cout << "Generating image for case: " + std::to_string(n) << std::endl;

    auto dot_str = net_to_dot(flowGraph.getAdjacencyList());
    png_from_dot(dot_str, PROJECT_DIR + "/test/data/example/" + std::to_string(n) + ".png");
}