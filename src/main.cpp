#include "PrimePathGen.h"
#include "PrimePathEdgeGen.h"
#include "ConverterDAG.h"
#include "FlowNetwork.h"
#include "GraphUtils.h"

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
    std::cout << "Printing flow graph after min flow" << std::endl;
    std::cout << "Graph has " << network.getAdjacencyList().size() << " edges and " << g3.verticesCount << " vertices" << std::endl;
    for (const auto& vertex : network.getAdjacencyList()) {
        for (const auto&[source, destination, lowerBound, upperBound, currentFlow] : vertex) {
            std::string from = converter.getLabelFromVertexID(source);
            std::string to = converter.getLabelFromVertexID(destination);

            std::cout << from << " -> " << to << "   current flow " << currentFlow << " in [" << lowerBound << " - " << upperBound << "]" << std::endl;
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

std::vector<std::vector<int>> generateTestCasesFromStateMachineGraph(const Graph& inputGraph) {
    const Graph& graph = inputGraph;

    // 1. Generate prime paths from the input graph that is UML state machine graph with non-orthogonal states
    PrimePathGen primePathGen(graph);
    std::vector<std::vector<int>> primePaths = primePathGen.generatePaths();

    // 2. Generate graph G2 from prime paths using them as vertices so that edges are all possible changes between vertices
    PrimePathEdgeGen primeEdgeGen(graph, primePaths.size());
    Graph g2 = primeEdgeGen.createTransitionEdgesFromPP(primePaths);
    int source; int sink;
    source = graph.verticesCount - 2;
    sink = graph.verticesCount - 1;

    // 3. Generate graph G3 by removing all the cycles from the graph G2.
    ConverterDAG converterDAG(graph);
    Graph g3 = converterDAG.removeCycles();

    // 4. Convert graph G3 into Flow Graph (G4) so that minimum flow can be calculated
    FlowNetwork flowGraph = convertG3toFlowNetwork(graph);
    int minFlow = flowGraph.sendMinimumFlow(source,sink);

    // 5. Find minimal test paths using minimal flows and flow graph (G4)
    // TODO: Implement expanding test paths from G4 and minimal flow
    // expand cycle vertices, connect disconnected neighbors in paths, remove repeated cycles, connect complete path (list of vertices) from prime path

    return {};
}

int main() {
    auto begin = std::chrono::high_resolution_clock::now();

    Graph g1 = readGraphFromStdin();
    //auto g1 = generateBigGraph(6);

    PrimePathGen primePathGen(g1);
    std::vector<std::vector<int>> paths = primePathGen.generatePaths();

    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.0f;

    //std::sort(paths.begin(), paths.end());

    printPrimePath(paths);

    std::cout << "Paths (after G1): " << (paths.size() > 2 ? paths.size() - 2 : 0) << std::endl;
    std::cout << "G1 Time: " << time << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    PrimePathEdgeGen prime_path_edge_gen(g1, paths.size());
    Graph g2 = prime_path_edge_gen.createTransitionEdgesFromPP(paths);
    end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.0f;

    const int source = g2.verticesCount - 2;
    const int sink = g2.verticesCount - 1;

    std::cout << "\nG2 Graph:" << std::endl;
    printGraph2(g2, source, sink);
    std::cout << "G2 Time: " << time << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    ConverterDAG converter_dag(g2);
    Graph g3 = converter_dag.removeCycles();
    end = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000.0f;

    std::cout << "\nG3 Graph:" << std::endl;
    printGraph2(g3,source,sink);
    std::cout << "G3 Time: " << time << std::endl << std::endl;

    converter_dag.printCycleVertices();

    //Flow graph conversion from DAG
    auto flowGraph = convertG3toFlowNetwork(g3);

    int min = flowGraph.sendMinimumFlow(source,sink);

    printFlowGraph(flowGraph, g3, converter_dag);

    return 0;
}
