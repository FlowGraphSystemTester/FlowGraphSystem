#pragma once
#include "Graph.h"

class PrimePathGen{
public:
    explicit PrimePathGen(const Graph& graph);

    [[nodiscard]] std::vector<std::vector<int>> generatePaths() const;
    void findPrimePathCandidateWithDFS(int current,std::vector<int>& path,std::vector<std::vector<int>>& paths,std::vector<bool>& visited) const;

private:
    Graph graph;
};

