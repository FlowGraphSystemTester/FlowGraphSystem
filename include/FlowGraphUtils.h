//
// Created by Kacper Smykla on 26.08.2026.
//
#pragma once
#include "ConverterDAG.h"
#include "FlowNetwork.h"
#include "Graph.h"

inline FlowNetwork convertG3toFlowNetwork(const Graph& g3) {
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

inline void printFlowGraph(FlowNetwork& network, const Graph& g3, const ConverterDAG& converter){
    std::cout << "Printing flow network after min flow" << std::endl;
    std::cout << "Graph has " << network.getAdjacencyList().size() << " vertices" << std::endl;
    for (const auto& vertex : network.getAdjacencyList()) {
        for (const auto&[source, destination, lowerBound, upperBound, currentFlow] : vertex) {
            std::string from = converter.getLabelFromVertexID(source);
            std::string to = converter.getLabelFromVertexID(destination);

            std::cout << from << " -> " << to << "   flow " << currentFlow << " in bounds (" << lowerBound << " - " << upperBound << ")" << std::endl;
        }
    }
    std::cout << "\n";
}

