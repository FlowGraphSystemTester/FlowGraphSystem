#pragma once

#include "Graph.h"
#include "FlowNetwork.h"

#define DISPLAY_MONIT

inline Graph readGraphFromStdin() {
#ifdef DISPLAY_MONIT
    std::cout << "Enter the graph data in the following format (metadata line + incidence line for each node - example shows 3 edges):\n";
    std::cout << "number_of_vertices number_of_edges\n";
    std::cout << "v1 v2\n";
    std::cout << "v2 v3\n";
    std::cout << "v3 v1\n\n\n";
#endif

    int vertCount;
    int edgeCount;

    std::cin >> vertCount >> edgeCount;
    Graph newGraph(vertCount);

    for (int i = 0; i < edgeCount; ++i) {
        int from;
        int to;

        std::cin >> from >> to;
        newGraph.addEdge(from, to);
    }
    return newGraph;
}

inline Graph generateFullGraph(const int K) {
    Graph graph(K);
    for (int i = 0; i < K; ++i) {
        for (int j = 0; j < K; ++j) {
            if (i != j) {
                graph.addEdge(i, j);
            }
        }
    }
    return graph;
}

inline std::string prettyPrintVertex(const int vert, const int source,const int sink) {
    if (vert < source) {
        return "p"+std::to_string(vert);
    } else if (vert == source) {
        return "s";
    } else if (vert == sink) {
        return "t";
    } else {
        return "v"+ std::to_string(vert-sink-1);
    }
}

inline void printGraph2(const Graph& graph, const int source, const int sink) {
    for (int i = 0; i < graph.adjacency_list.size(); ++i) {
        std::cout << prettyPrintVertex(i, source, sink) << ": ";
        for (const auto& neighbor : graph.adjacency_list[i]) {
            std::cout << prettyPrintVertex(neighbor, source, sink) << " ";
        }
        std::cout << std::endl;
    }
}

inline std::string saveGraphToString(const Graph& graph) {
    std::ostringstream graph_file("");
    graph_file << graph.verticesCount << " " << graph.number_of_edges << std::endl;

    for (int i = 0; i < graph.adjacency_list.size(); ++i) {
        for (int j = 0; j < graph.adjacency_list[i].size(); ++j) {
            graph_file << i << " " << graph.adjacency_list[i][j] << std::endl;
        }
    }
    return graph_file.str();
}

std::string savePrimePathsToString(const std::vector<std::vector<int>>& paths) {
    std::ostringstream graph_file("");
    graph_file << paths.size() << std::endl;

    for (int i = 0; i < paths.size(); ++i) {
        graph_file << i << "\t";
        for (int j = 0; j < paths[i].size(); ++j) {
            graph_file << paths[i][j] << " ";
        }
        graph_file << std::endl;
    }
    return graph_file.str();
}

inline void printPrimePath(const std::vector<std::vector<int>>& paths) {
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

