#include "ConverterDAG.h"

ConverterDAG::ConverterDAG(const Graph& graph): graph(graph), cycleCounter(0){
    newCycleVertices.resize(graph.verticesCount);

    const int maxVertices = 2 * graph.verticesCount + 1;

    dfs_visited.resize(maxVertices, false);
    dfs_position.resize(maxVertices, -1);
    dfs_path.reserve(maxVertices);
    dfs_stack.reserve(maxVertices);
    in_cycle.resize(maxVertices, false);

    incomingEdges.resize(maxVertices);
    outcomingEdges.resize(maxVertices);
}

//To avoid allocating in the hot path - only clear/fill (reuse) temporary arrays which are allocated only one time in the constructor
void ConverterDAG::clearDFSData() {
    std::fill_n(dfs_visited.begin(),graph.verticesCount,0);
    std::fill_n(dfs_position.begin(),graph.verticesCount,-1);

    dfs_path.clear();
    dfs_stack.clear();
}

// Find the first cycle in the graph (in the order in which adjacency list is stored)
std::vector<int> ConverterDAG::findFirstCycleInGraph()
{
    clearDFSData();
    std::vector<int> cycle;

    for (int startVertex = 0; startVertex < graph.verticesCount; ++startVertex)
    {
        if (dfs_visited[startVertex]) continue;
        if (isCycleDFS(startVertex,cycle)) return cycle;
    }
    return {};
}

void ConverterDAG::addStartVertex(int startVertex) {
    dfs_visited[startVertex] = true;
    dfs_position[startVertex] = dfs_path.size();
    dfs_path.push_back(startVertex);
    dfs_stack.push_back({ startVertex, 0 });
}

bool ConverterDAG::isCycleDFS(const int& start, std::vector<int>& cycle){
    addStartVertex(start);
    while (!dfs_stack.empty())
    {
        const auto&[vertex, next_edge] = dfs_stack.back();

        // If all neighbors have been visited backtrack to previous vertex
        if (next_edge >= graph.adjacency_list[vertex].size())
        {
            dfs_position[vertex] = -1;
            dfs_path.pop_back();
            dfs_stack.pop_back();
            continue;
        }

        const int neighbor = graph.adjacency_list[vertex][next_edge];
        dfs_stack.pop_back();
        dfs_stack.push_back({ vertex, next_edge+1 });

        // If a neighboring vertex belongs to the search path that means that the cycle was found
        if (dfs_position[neighbor] != -1)
        {
            cycle.assign(dfs_path.begin() + dfs_position[neighbor],dfs_path.end());
            return true;
        }

        // Otherwise if this neighboring vertex was not visited, make it so and add it to the stack
        if (!dfs_visited[neighbor])
        {
            dfs_visited[neighbor] = true;
            dfs_position[neighbor] = dfs_path.size();
            dfs_path.push_back(neighbor);
            dfs_stack.push_back({ neighbor, 0 });
        }
    }
    return false;
}

int ConverterDAG::addNewCycleVertex(const std::vector<int>& cycle) {
    const int newVertex = graph.verticesCount++;
    graph.adjacency_list.resize(graph.verticesCount);
    graph.vertex_types.push_back('v');
    newCycleVertices.push_back(cycle);
    cycleCounter++;
    return newVertex;
}

void ConverterDAG::initializeInCycleCache(const std::vector<int>& cycle) {
    std::fill(in_cycle.begin(),in_cycle.begin() + graph.verticesCount,0);

    for (const int& vertex : cycle)
    {
        in_cycle[vertex] = true;
    }
}

void ConverterDAG::redirectEdges(const std::vector<int>& cycle, int newVertex) {
    incomingEdges[newVertex].clear();
    outcomingEdges[newVertex].clear();

    for (const int cycle_vertex : cycle)
    {
        // Redirect incoming edges for cycle vertices into the new vertex
        for (int vertex = 0; vertex < newVertex;++vertex)
        {
            if (auto it = std::ranges::find(graph.adjacency_list[vertex], cycle_vertex); it != graph.adjacency_list[vertex].end())
            {
                graph.adjacency_list[vertex].erase(it);
                //graph.adjacency_list[vertex].push_back(newVertex);
                if (!std::ranges::contains(graph.adjacency_list[vertex],newVertex)) graph.adjacency_list[vertex].push_back(newVertex);
            }
        }

        // Redirect outcoming edges from cycle vertices into new vertex
        for (const int neighbor : graph.adjacency_list[cycle_vertex])
        {
            if (neighbor == newVertex || in_cycle[neighbor]) continue;
            outcomingEdges[newVertex].push_back({ cycle_vertex, neighbor });
            //if (!std::ranges::contains(adjacency_list[newVertex], neighbor)) adjacency_list[newVertex].push_back(neighbour);
            if (!std::ranges::contains(graph.adjacency_list[newVertex],neighbor)) graph.adjacency_list[newVertex].push_back(neighbor);
        }
    }
}

// Remove edges connected to vertices in the middle of the cycle (not at the beginning nor at the end) after adding new vertex
void ConverterDAG::removeCycleEdges(const int newVertex) {
    for (int vertex = 0;vertex < newVertex;++vertex)
    {
        auto& neighbours = graph.adjacency_list[vertex];
        std::erase_if(neighbours, [this](const int neighbour) { return in_cycle[neighbour]; } );
    }
}

Graph ConverterDAG::removeCycles(){
    while (true){
        //We should avoid resizing/allocating in the hot path
        assert(graph.verticesCount <= dfs_visited.size());

        const std::vector<int> firstCycle = findFirstCycleInGraph();
        if (firstCycle.empty()) break;

        const int newVertex = addNewCycleVertex(firstCycle);
        initializeInCycleCache(firstCycle);

        redirectEdges(firstCycle, newVertex);
        removeCycleEdges(newVertex);

        // Remaining cycle vertices stay but without edges connecting to them
        for (const int cycle_vertex : firstCycle)
        {
            graph.adjacency_list[cycle_vertex].clear();
        }
    }
    return graph;
}

std::vector<std::vector<int>> ConverterDAG::getNewCycleVertices() const{
    return newCycleVertices;
}

std::string ConverterDAG::getLabelFromVertexID(const int vertexID) const {
    const int g2Size = graph.verticesCount-cycleCounter-2;
    std::string label = "";

    if (vertexID < g2Size) label += "p" + std::to_string(vertexID);
    else if (vertexID == g2Size) label += "s";
    else if (vertexID == g2Size+1) label += "t";
    else label += "v" + std::to_string(vertexID-g2Size-2);

    return label;
}

void ConverterDAG::printCycleVertices() const {
    std::cout << "v components (cycle vertices): " << std::endl;

    for (int i = 0; i < newCycleVertices.size(); i++) {
        std::cout << getLabelFromVertexID(i) << ": ";

        for (int j = 0 ; j < newCycleVertices[i].size(); ++j) {
            std::cout << getLabelFromVertexID(newCycleVertices[i][j]) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}