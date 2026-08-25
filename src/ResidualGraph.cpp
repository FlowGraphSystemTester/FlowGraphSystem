//
// Created by dcend on 29.03.2025.
//

#include "ResidualGraph.h"

bool ResidualGraph::setLevelsByBFS(int source, int sink) {
  std::fill(levelOfVertex.begin(), levelOfVertex.end(), UNASSIGNED_LEVEL);
  std::queue<int> vertex_queue;
  vertex_queue.push(source);
  levelOfVertex[source] = 0;

  while (!vertex_queue.empty()) {
    int current_vertex = vertex_queue.front();
    vertex_queue.pop();
    for (ResidualEdge &edge: adjacencyList[current_vertex]) {
      if (levelOfVertex[edge.destination] == UNASSIGNED_LEVEL && edge.residualValue > 0) {
        levelOfVertex[edge.destination] = levelOfVertex[current_vertex] + 1;
        vertex_queue.push(edge.destination);
      }
    }
  }
  return levelOfVertex[sink] != UNASSIGNED_LEVEL;
}

int ResidualGraph::sendFlowByDFS(int currentVertex, int sink, int flow) {
  if (currentVertex == sink) {
    return flow;
  }

  for (; edgePointer[currentVertex] < adjacencyList[currentVertex].size(); edgePointer[currentVertex]++) {
    ResidualEdge &edge = adjacencyList[currentVertex][edgePointer[currentVertex]];
    if (levelOfVertex[edge.destination] == levelOfVertex[currentVertex] + 1 && edge.residualValue > 0) {
      int available_flow = std::min(flow, edge.residualValue);
      int sent_flow = sendFlowByDFS(edge.destination, sink, available_flow);

      if (sent_flow > 0) {
        edge.residualValue -= sent_flow;
        adjacencyList[edge.destination][edge.reverseEdgeId].residualValue += sent_flow;
        return sent_flow;
      }
    }
  }
  return 0;
}

int ResidualGraph::dinicMaxFlow(int source, int sink) {
  int total_flow = 0;
  while (setLevelsByBFS(source, sink)) {
    std::fill(edgePointer.begin(), edgePointer.end(), 0);
    total_flow += sendFlowByDFS(source, sink, INT_MAX);
  }
  return total_flow;
}

ResidualGraph::ResidualGraph(int numberOfVertices) {
  this->numberOfVertices = numberOfVertices;
  adjacencyList.resize(numberOfVertices);
  levelOfVertex.resize(numberOfVertices);
  edgePointer.resize(numberOfVertices);
}

int ResidualGraph::findMaxFlow(int source, int sink) {
  return dinicMaxFlow(source, sink);
}

void ResidualGraph::addResidualEdgePair(const int v1, const int v2, const int forwardResidualValue,
                                        int backwardResidualValue) {
  adjacencyList[v1].push_back({v2, forwardResidualValue, (int) adjacencyList[v2].size()});
  adjacencyList[v2].push_back({v1, backwardResidualValue, (int) adjacencyList[v1].size() - 1});
}

std::vector<std::vector<ResidualEdge> > &ResidualGraph::getAdjacencyList() {
  return adjacencyList;
}
