//
// Created by Kacper Smykla on 26.08.2026.
//
#include "GraphUtils.h"
#include "PrimePathGen.h"
#include "PrimePathEdgeGen.h"
#include "FlowGraphUtils.h"
#include "ConverterDAG.h"

#include "PP_to_G2.h"
#include "Visualization.h"

constexpr std::string PROJECT_DIR = "..";

inline std::unordered_map<int,int> mappingToArticle = {
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

inline std::vector<std::vector<int>> reorderPathsToArticleOrder(const std::vector<std::vector<int>>& paths) {
    std::vector<std::vector<int>> pathsToArticleOrder(paths.size(), std::vector<int>());
    for (int i = 0; i < paths.size(); i++) {
        pathsToArticleOrder[mappingToArticle[i]] = paths[i];
    }
    return pathsToArticleOrder;
}

void articleMainLoop() {
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
    std::cout << "Paths (after G1): " << (paths.size() > 2 ? paths.size() - 2 : 0) << std::endl;

    for (int i = 0; i < paths.size(); i++) {
        auto path_dot = path_to_dot(paths[i]);
        png_from_dot(path_dot, PROJECT_DIR + "/test/data/article/paths/p" + std::to_string(i) + ".png");
    }

    PrimePathEdgeGen prime_path_edge_gen(g1, paths.size());
    auto g2 = prime_path_edge_gen.createTransitionEdgesFromPP(paths);
    auto path_map = identify_names_PP_to_G2_map(g2.adjacency_list, paths.size());

    auto g2_dot = adj_to_dot_named(g2.adjacency_list, path_map);
    png_from_dot(g2_dot, PROJECT_DIR + "/test/data/article/g2.png");

    const int source = g2.verticesCount - 2;
    const int sink = g2.verticesCount - 1;

    std::cout << "\nG2 Graph:" << std::endl;
    printGraph2(g2, source, sink);

    ConverterDAG converter_dag(g2);
    auto g3 = converter_dag.removeCycles();
    path_map = update_map_with_vertices(g3.adjacency_list, path_map);

    auto g3_dot = adj_to_dot_named(g3.adjacency_list, path_map);
    png_from_dot(g3_dot, PROJECT_DIR + "/test/data/article/g3.png");

    std::cout << "\nG3 Graph:" << std::endl;
    printGraph2(g3,source,sink);

    auto flowGraph = convertG3toFlowNetwork(g3);

    auto net_dot = net_to_dot_named(flowGraph.getAdjacencyList(), path_map);
    png_from_dot(net_dot, PROJECT_DIR + "/test/data/article/net.png");

    int min = flowGraph.sendMinimumFlow(source,sink);

    auto flow_dot = net_to_dot_named(flowGraph.getAdjacencyList(), path_map);
    png_from_dot(flow_dot, PROJECT_DIR + "/test/data/article/flow.png");
}

int main() {
    articleMainLoop();
    return 0;
}