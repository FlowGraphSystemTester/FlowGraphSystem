#pragma once
#include "Graph.h"

class ConverterDAG
{
public:
    ConverterDAG(const Graph& graph);

    std::vector<int> findFirstCycleInGraph();
    bool isCycleDFS(const int& start, std::vector<int>& cycle);

    [[nodiscard]] Graph removeCycles();
    [[nodiscard]] std::vector<std::vector<int>> getNewCycleVertices() const;
    [[nodiscard]] std::string getLabelFromVertexID(int vertexID) const;
    [[nodiscard]] const std::vector<std::vector<std::pair<int, int>>> &getIncomingEdges() const;
    [[nodiscard]] const std::vector<std::vector<std::pair<int, int>>> &getOutcomingEdges() const;

    void printCycleVertices() const;

private:
    inline void clearDFSData();
    inline void addStartVertex(int startVertex);
    inline int addNewCycleVertex(const std::vector<int> &cycle);
    inline void initializeInCycleCache(const std::vector<int> &cycle);
    inline void redirectEdges(const std::vector<int> &cycle, int newVertex);
    inline void removeCycleEdges(int newVertex);

    std::vector<std::vector<int>> newCycleVertices;
    std::vector<std::vector<std::pair<int,int>>> incomingEdges;
    std::vector<std::vector<std::pair<int,int>>> outcomingEdges;

    std::vector<bool> dfs_visited;
    std::vector<int> dfs_position;
    std::vector<int> dfs_path;
    std::vector<DFSState> dfs_stack;
    std::vector<bool> in_cycle;

    Graph graph;
    int cycleCounter;
};