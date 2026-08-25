#pragma once
#include "Graph.h"

class PrimePathEdgeGen {
public:
    explicit PrimePathEdgeGen(const Graph& g1, int ppCount);

    std::vector<int> bfsFindShortestPath(int startVertex, int endVertex);
    std::vector<int> mergePaths(const std::vector<int>& firstPath, const std::vector<int>& secondPath);
    [[nodiscard]] Graph createTransitionEdgesFromPP(const std::vector<std::vector<int>>& paths);
private:
    void initializeVertexTypes();

    std::unordered_map<std::pair<int, int>, std::vector<int>, pairHash> bfsCache;
    Graph g1;
    Graph g2;
};
