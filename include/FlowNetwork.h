#pragma once
#include "pch.h"

#include "FlowBoundedEdge.h"
#include "ResidualGraph.h"

class FlowNetwork {
public:
    explicit FlowNetwork(int numberOfNodes);

    void addEdge(int from, int to, int lowerBound, int upperBound, int currentFlow);
    int sendMinimumFlow(int source, int sink);
    int sendMaximumFlow(int source, int sink);
    int establishFeasibleFlow(int source, int sink);
    static FlowNetwork createFromFile(std::string fileName);
    void saveToFile(std::string fileName, int sourceVertex, int sinkVertex);
    std::vector<std::vector<FlowBoundedEdge>>& getAdjacencyList();

private:
    void inline assignCurrentFlow(FlowBoundedEdge &flowBoundedEdge, ResidualEdge &residualEdge);
    void inline assignFlowToBoundedEdge(FlowBoundedEdge &flowBoundedEdge,
    std::vector<std::vector<ResidualEdge>>& residualAdjacencyList);
    void inline assignFlowFromResidualGraph(ResidualGraph &residualGraph);
    void inline calculateBalances(FlowNetwork &copy, std::vector<int> &balances);
    void inline transformToZeroFlowAndLowerBound(FlowNetwork &copy);
    void inline connectEdgesToSAndT(FlowNetwork &copy, std::vector<int> &balances, int S, int T);
    void inline prepareCopy(int source, int sink, FlowNetwork &copy);
    void inline transformFromCopyToOriginal(FlowNetwork &copy);
    int getFlowOutgoingFrom(int node);
    int sendMaximumFlowWithoutEstablishingFlow(int source, int sink);
    ResidualGraph inline toResidualGraph();

    int numberOfNodes;
    std::vector<std::vector<FlowBoundedEdge>> adjacencyList;
};

