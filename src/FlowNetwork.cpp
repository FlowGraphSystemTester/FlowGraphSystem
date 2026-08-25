//
// Created by dcend on 30.03.2025.
//
#include "FlowNetwork.h"

FlowNetwork::FlowNetwork(int numberOfNodes): adjacencyList(numberOfNodes) {
  this->numberOfNodes = numberOfNodes;
  adjacencyList.resize(numberOfNodes);
}

void FlowNetwork::addEdge(int from, int to, int lowerBound, int upperBound, int currentFlow) {
  adjacencyList[from].push_back({from, to, lowerBound, upperBound, currentFlow});
}

int FlowNetwork::sendMinimumFlow(int source, int sink) {
  establishFeasibleFlow(source, sink);
  sendMaximumFlowWithoutEstablishingFlow(sink, source);
  return getFlowOutgoingFrom(source);
}

void FlowNetwork::assignCurrentFlow(FlowBoundedEdge &flowBoundedEdge, ResidualEdge &residualEdge) {
  flowBoundedEdge.currentFlow = flowBoundedEdge.upperBound - residualEdge.residualValue + flowBoundedEdge.
                                lowerBound;
}

void FlowNetwork::assignFlowToBoundedEdge(FlowBoundedEdge &flowBoundedEdge,
                                          std::vector<std::vector<ResidualEdge> > &residualAdjacencyList) {
  for (int k = 0; k < residualAdjacencyList[flowBoundedEdge.source].size(); k++) {
    ResidualEdge &residualEdge = residualAdjacencyList[flowBoundedEdge.source][k];
    if (flowBoundedEdge.destination == residualEdge.destination) {
      ResidualEdge &backwardEdge = residualAdjacencyList[residualEdge.destination][residualEdge.reverseEdgeId];
      flowBoundedEdge.currentFlow = backwardEdge.residualValue + flowBoundedEdge.lowerBound;
      break;
    }
  }
}

void FlowNetwork::assignFlowFromResidualGraph(ResidualGraph &residualGraph) {
  std::vector<std::vector<ResidualEdge> > &residualAdjacencyList = residualGraph.getAdjacencyList();
  for (int i = 0; i < adjacencyList.size(); i++) {
    for (int j = 0; j < adjacencyList[i].size(); j++) {
      FlowBoundedEdge &flowBoundedEdge = adjacencyList[i][j];
      assignFlowToBoundedEdge(flowBoundedEdge, residualAdjacencyList);
    }
  }
}

int FlowNetwork::sendMaximumFlow(int source, int sink) {
  establishFeasibleFlow(source, sink);
  return sendMaximumFlowWithoutEstablishingFlow(source, sink);
}

void FlowNetwork::calculateBalances(FlowNetwork &copy, std::vector<int> &balances) {
  balances = std::vector<int>(copy.numberOfNodes, 0);

  for (int i = 0; i < copy.numberOfNodes; i++) {
    for (FlowBoundedEdge &edge: copy.adjacencyList[i]) {
      balances[edge.source] -= edge.lowerBound;
      balances[edge.destination] += edge.lowerBound;
    }
  }
}

void FlowNetwork::transformToZeroFlowAndLowerBound(FlowNetwork &copy) {
  for (int i = 0; i < copy.numberOfNodes; i++) {
    for (int j = 0; j < copy.adjacencyList[i].size(); j++) {
      FlowBoundedEdge &e = copy.adjacencyList[i][j];
      e.upperBound = e.upperBound - e.lowerBound;
      e.lowerBound = 0;
      e.currentFlow = 0;
    }
  }
}

void FlowNetwork::connectEdgesToSAndT(FlowNetwork &copy, std::vector<int> &balances, int S, int T) {
  for (int i = 0; i < balances.size(); i++) {
    if (balances[i] > 0) {
      copy.addEdge(S, i, 0, balances[i], 0);
    } else if (balances[i] < 0) {
      copy.addEdge(i, T, 0, -balances[i], 0);
    }
  }
}

void FlowNetwork::prepareCopy(int source, int sink, FlowNetwork &copy) {
  for (int i = 0; i < numberOfNodes; i++) {
    for (int j = 0; j < adjacencyList[i].size(); j++) {
      FlowBoundedEdge &e = adjacencyList[i][j];
      copy.addEdge(e.source, e.destination, e.lowerBound, e.upperBound, e.currentFlow);
    }
  }
  copy.addEdge(sink, source, 0,INT_MAX, 0);
}

void FlowNetwork::transformFromCopyToOriginal(FlowNetwork &copy) {
  for (int i = 0; i < adjacencyList.size(); i++) {
    for (int j = 0; j < adjacencyList[i].size(); j++) {
      FlowBoundedEdge &orginalEdge = adjacencyList[i][j];
      FlowBoundedEdge &copiedEdge = copy.adjacencyList[i][j];
      orginalEdge.currentFlow = copiedEdge.currentFlow + orginalEdge.lowerBound;
    }
  }
}

int FlowNetwork::establishFeasibleFlow(int source, int sink) {
  FlowNetwork copy = FlowNetwork(numberOfNodes + 2);
  prepareCopy(source, sink, copy);
  std::vector<int> balances;
  calculateBalances(copy, balances);
  transformToZeroFlowAndLowerBound(copy);
  int S = numberOfNodes;
  int T = numberOfNodes + 1;
  connectEdgesToSAndT(copy, balances, S, T);
  int feasibleFlow = copy.sendMaximumFlowWithoutEstablishingFlow(S, T);
  transformFromCopyToOriginal(copy);
  return feasibleFlow;
}

FlowNetwork FlowNetwork::createFromFile(std::string fileName) {
  std::fstream file(fileName);
  std::string line;

  if (file.is_open()) {
    std::getline(file, line);
    int numberOfVertices = std::stoi(line);
    FlowNetwork newFlowNetwork = FlowNetwork(numberOfVertices);
    std::getline(file, line); //ignore source
    std::getline(file, line); //ignore sink

    while (std::getline(file, line)) {
      std::stringstream ss(line);
      std::string token;
      std::vector<std::string> tokens;

      while (std::getline(ss, token, ' ')) {
        tokens.push_back(token);
      }

      int from = std::stoi(tokens[0]);
      int to = std::stoi(tokens[1]);
      int lowerBound = std::stoi(tokens[2]);
      int currentFlow = std::stoi(tokens[3]);
      int upperBound = std::stoi(tokens[4]);

      newFlowNetwork.addEdge(from, to, lowerBound, upperBound, currentFlow);
    }
    file.close();
    return newFlowNetwork;
  }
  return FlowNetwork(0);
}

void FlowNetwork::saveToFile(std::string fileName, int sourceVertex, int sinkVertex) {
  std::ofstream file(fileName);

  if (file.is_open()) {
    file << numberOfNodes << std::endl;
    file << sourceVertex << std::endl;
    file << sinkVertex << std::endl;
    for (int i = 0; i < adjacencyList.size(); ++i) {
      for (int j = 0; j < adjacencyList[i].size(); ++j) {
        FlowBoundedEdge &e = adjacencyList[i][j];
        file << e.source << " " << e.destination << " " << e.lowerBound << " " << e.currentFlow << " " << e.
            upperBound << std::endl;
      }
    }
  }
}


std::vector<std::vector<FlowBoundedEdge>>& FlowNetwork::getAdjacencyList(){
  return adjacencyList;
}

int FlowNetwork::getFlowOutgoingFrom(int node) {
  int flow = 0;
  for (FlowBoundedEdge &edge: adjacencyList[node]) {
    flow += edge.currentFlow;
  }
  return flow;
}

int FlowNetwork::sendMaximumFlowWithoutEstablishingFlow(int source, int sink) {
  ResidualGraph residualGraph = toResidualGraph();
  int currentFlow = getFlowOutgoingFrom(source);
  int maxFlow = residualGraph.findMaxFlow(source, sink);
  assignFlowFromResidualGraph(residualGraph);
  return maxFlow + currentFlow;
}

ResidualGraph FlowNetwork::toResidualGraph() {
  ResidualGraph residualGraph(numberOfNodes);
  for (int i = 0; i < numberOfNodes; i++) {
    for (int j = 0; j < adjacencyList[i].size(); j++) {
      FlowBoundedEdge &e = adjacencyList[i][j];
      residualGraph.addResidualEdgePair(e.source, e.destination, e.upperBound - e.currentFlow,
                                        e.currentFlow - e.lowerBound);
    }
  }
  return residualGraph;
}
