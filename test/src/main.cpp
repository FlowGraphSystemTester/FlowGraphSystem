//
// Created by M.Stanek on 19.08.2026.
//

#include <fstream>
#include <test.h>
#include <PP_to_G2.h>
#include <../include/pch.h>

#include "ConverterDAG.h"
#include "FlowNetwork.h"
#include "Graph.h"
#include "GraphUtils.h"
#include "helper.hpp"
#include "PrimePathEdgeGen.h"
#include "PrimePathGen.h"


FlowNetwork convertG3toFlowNetwork(const Graph& g3) {
    FlowNetwork newFlowNetwork(g3.adjacency_list.size());
    std::vector<bool> used(g3.adjacency_list.size(), false);
    std::set<std::pair<int,int>> edges;
    for (int i = 0; i < g3.adjacency_list.size(); ++i) {
        if (!g3.adjacency_list[i].empty()) {
            for (const int to : g3.adjacency_list[i]) {
                edges.insert({i,to});
                used[i] = true;
                used[to] = true;
            }
        }
    }

    int usedVerticesCounter = 0;
    for (int i = 0; i < used.size(); i++) {
        if (used[i]) usedVerticesCounter++;
    }

    for (const auto& [u,v] : edges) {
        newFlowNetwork.addEdge(u, v, 1, usedVerticesCounter - 1, 0);
    }

    return newFlowNetwork;
}

void printFlowGraph(FlowNetwork& network, const Graph& g3, const ConverterDAG& converter){
    std::cout << "Wypisywanie flow graph po minimalnym przeplywie" << std::endl;
    std::cout << "Graf ma " << network.getAdjacencyList().size() << " krawedzi oraz " << g3.verticesCount << " wierzcholkow" << std::endl;
    for (const auto& vertex : network.getAdjacencyList()) {
        for (const auto&[source, destination, lowerBound, upperBound, currentFlow] : vertex) {
            std::string from = converter.getLabelFromVertexID(source);
            std::string to = converter.getLabelFromVertexID(destination);

            std::cout << from << " -> " << to << "   przeplyw " << currentFlow << " w (" << lowerBound << " - " << upperBound << ")" << std::endl;
        }
    }
    std::cout << "\n";
}

void printPrimePath(const std::vector<std::vector<int>>& paths) {
    if (paths.size() > 2) {
        std::cout << "Prime paths\n";
        for (int i = 0; i < paths.size()-2; ++i) {
            std::cout << "(P"+std::to_string(i)+"):\t";
            for (int j = 0; j < paths[i].size(); ++j) {
                std::cout << paths[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

const std::string PROJECT_DIR = "..";

void draw_flow(TC_G2_to_G3 tc, int n) {
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

std::unordered_map<int,int> mappingToArticle = {
    {0,4},
    {1,5},
    {2,9},
    {3,0},
    {4,16},
    {5,20},
    {6,19},
    {7,11},
    {8,13},
    {9,1},
    {10,18},
    {11,21},
    {12,12},
    {13,7},
    {14,6},
    {15,10},
    {16,17},
    {17,3},
    {18,2},
    {19,8},
    {20,14},
    {21,15},
    // For vertex s and t
    {22, 22},
    {23, 23}
};

std::vector<std::vector<int>> reorderPathsToArticleOrder(const std::vector<std::vector<int>>& paths) {
    std::vector<std::vector<int>> pathsToArticleOrder(paths.size(), std::vector<int>());
    for (int i = 0; i < paths.size(); i++) {
        pathsToArticleOrder[mappingToArticle[i]] = paths[i];
    }
    return pathsToArticleOrder;
}

void article() {

    Graph g1 = readGraphFromStdin();
    RevIdMapT state_map = {
        {0, "s"},
        {1, "A"},
        {2, "B"},
        {3, "C"},
        {4, "D"},
        {5, "E"},
        {6, "F"},
        {7, "G"},
        {8, "H"},
        {9, "I"},
        {10, "J"},
        {11, "K"},
        {12, "t"},

    };
    auto g1_dot = adj_to_dot_named(g1.adjacency_list, state_map);
    png_from_dot(g1_dot, PROJECT_DIR + "/test/data/article/g1.png");

    PrimePathGen prime_path_gen(g1);
    auto paths = prime_path_gen.generatePaths();

    paths = reorderPathsToArticleOrder(paths);
    printPrimePath(paths);

    for (int i = 0; i < paths.size(); i++) {
        auto path_dot = path_to_dot(paths[i]);
        png_from_dot(path_dot, PROJECT_DIR + "/test/data/article/paths/p" + std::to_string(i) + ".png");
    }

    PrimePathEdgeGen prime_path_edge_gen(g1, paths.size());
    auto g2 = prime_path_edge_gen.createTransitionEdgesFromPP(paths);
    auto path_map = identify_names_PP_to_G2_map(g2.adjacency_list, paths.size());

    auto g2_dot = adj_to_dot_named(g2.adjacency_list, path_map);
    png_from_dot(g2_dot, PROJECT_DIR + "/test/data/article/g2.png");

    ConverterDAG converter_dag(g2);
    auto g3 = converter_dag.removeCycles();
    path_map = update_map_with_vertices(g3.adjacency_list, path_map);

    auto g3_dot = adj_to_dot_named(g3.adjacency_list, path_map);
    png_from_dot(g3_dot, PROJECT_DIR + "/test/data/article/g3.png");

    auto flowGraph = convertG3toFlowNetwork(g3);

    auto net_dot = net_to_dot_named(flowGraph.getAdjacencyList(), path_map);
    png_from_dot(net_dot, PROJECT_DIR + "/test/data/article/net.png");

    const int source = g2.verticesCount - 2;
    const int sink = g2.verticesCount - 1;
    int min = flowGraph.sendMinimumFlow(source,sink);

    auto flow_dot = net_to_dot_named(flowGraph.getAdjacencyList(), path_map);
    png_from_dot(flow_dot, PROJECT_DIR + "/test/data/article/flow.png");
}

int main() {
    article();
    return 0;
}
// void example() {
//     TS_G1_to_PP example_suite = from_file_ts_G1_to_PP(PROJECT_DIR + "/test/data/example.suite.json");
//     TC_G1_to_PP stage1_in = example_suite.cases[0];
//     TCR_G1_to_PP stage1_out = test_G1_to_PP(stage1_in);
//
//     GraphViz gv;
//     auto stage1_dot = gv.adj_to_dot(stage1_in.in_graph);
//     png_from_dot(stage1_dot, PROJECT_DIR + "/test/data/example/g1.png");
//
//     TC_PP_to_G2 stage2_in = {
//         .name = stage1_in.name,
//         .in_graph = stage1_in.in_graph,
//         .in_pps = stage1_out.output_pps,
//     };
//     TCR_PP_to_G2 stage2_out = test_PP_to_G2(stage2_in);
//     RevIdMapT pp_names = identify_names_PP_to_G2(stage2_out);
//     auto stage2_dot = adj_to_dot_named(stage2_out.output_graph, pp_names);
//     png_from_dot(stage2_dot, PROJECT_DIR + "/test/data/example/g2.png");
//
//
//     TC_G2_to_G3 stage3_in = {
//         .name = stage2_in.name,
//         .in_graph = stage2_out.output_graph,
//         .in_pps = stage2_in.in_pps,
//     };
//     TCR_G2_to_G3 stage3_out = test_G2_to_G3(stage3_in);
//     auto stage3_dot = gv.adj_to_dot(stage3_out.output_graph);
//     png_from_dot(stage3_dot, PROJECT_DIR + "/test/data/example/g3.png");
//
//     draw_flow(stage3_in, 0);
//
// }
//
// int main() {
//     example();
//     return 0;
// }

// int main() {
//     TS_G1_to_PP g1_to_pp = from_file_ts_G1_to_PP(PROJECT_DIR + "/test/data/G1toPP.suite.json");
//     TSR_G1_to_PP res_g1_to_pp = run_ts_G1_to_PP(g1_to_pp, true);
//     std::ofstream ofs(PROJECT_DIR + "/test/data/G1toPP.suite.out.json");
//     ofs << res_g1_to_pp.to_json_string();
//     ofs.close();
//
//     TS_PP_to_G2 pp_to_g2 = from_file_ts_PP_to_G2(PROJECT_DIR + "/test/data/PPtoG2.suite.json");
//     TSR_PP_to_G2 res_pp_to_g2 = run_ts_PP_to_G2(pp_to_g2, true);
//     std::ofstream ofs2(PROJECT_DIR + "/test/data/PPtoG2.suite.out.json");
//     ofs2 << res_pp_to_g2.to_json_string();
//     ofs2.close();
//
//     TS_G2_to_G3 g2_to_g3 = from_file_ts_G2_to_G3(PROJECT_DIR + "/test/data/G2toG3.suite.json");
//     TSR_G2_to_G3 res_g2_to_g3 = run_ts_G2_to_G3(g2_to_g3, true);
//     std::ofstream ofs3(PROJECT_DIR + "/test/data/G2toG3.suite.out.json");
//     ofs3 << res_g2_to_g3.to_json_string();
//     ofs3.close();
//
//     // test 1 imgs
//     for (int id = 0; id < res_g1_to_pp.results.size(); id++) {
//         std::cout << "Generating input image for G1 to PP case no. " << id << std::endl;
//         png_from_dot(adj_to_dot(res_g1_to_pp.results[id].test_case.in_graph), PROJECT_DIR + "/test/data/G1toPPpngs/"  + std::to_string(id) + ".in.png");
//     }
//
//     for (int id = 0; id < res_pp_to_g2.results.size(); id++) {
//         std::cout << "Generating input image for PP to G2 case no. " << id << std::endl;
//         png_from_dot(adj_to_dot(res_pp_to_g2.results[id].test_case.in_graph), PROJECT_DIR + "/test/data/PPtoG2pngs/"  + std::to_string(id) + ".in.png");
//         std::cout << "Generating output image for PP to G2 case no. " << id << std::endl;
//         png_from_dot(adj_to_dot(res_pp_to_g2.results[id].output_graph), PROJECT_DIR + "/test/data/PPtoG2pngs/"  + std::to_string(id) + ".out.png");
//     }
//
//     for (int id = 0; id < res_g2_to_g3.results.size(); id++) {
//         std::cout << "Generating input image for G2 to G3 case no. " << id << std::endl;
//         png_from_dot(adj_to_dot(res_g2_to_g3.results[id].test_case.in_graph), PROJECT_DIR + "/test/data/G2toG3pngs/"  + std::to_string(id) + ".in.png");
//         std::cout << "Generating output image for G2 to G3 case no. " << id << std::endl;
//         png_from_dot(adj_to_dot(res_g2_to_g3.results[id].output_graph), PROJECT_DIR + "/test/data/G2toG3pngs/"  + std::to_string(id) + ".out.png");
//     }
//
//     // std::vector<int> failedIDs;
//     // for (int i = 0; i < res_cycle_red.results.size(); i++) {
//     //     if (!res_cycle_red.results[i].success) failedIDs.push_back(i);
//     // }
//     //
//     // for (auto id : failedIDs) std::cout << id << " " << std::endl;
//
//     return 0;
// }
