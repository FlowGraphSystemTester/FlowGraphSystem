#include "PrimePathGen.h"

PrimePathGen::PrimePathGen(const Graph& graph) : graph(graph) {}

void PrimePathGen::findPrimePathCandidateWithDFS(const int current,std::vector<int>& path,std::vector<std::vector<int>>& paths,std::vector<bool>& visited) const {
    //Put the current vertex as starting point for prime path
    std::stack<DFSState> dfsStack;
    dfsStack.push({ current, 0 });

    while (!dfsStack.empty())
    {
        // Take a new vertex from the stack
        auto [vertex, nextEdge] = dfsStack.top();
        dfsStack.pop();

        // First visit of the vertex.
        if (nextEdge == 0)
        {
            path.push_back(vertex);
            visited[vertex] = true;
        }

        // No more neighbors to process -> backtrack.
        if (nextEdge >= graph.adjacency_list[vertex].size())
        {
            paths.push_back(path);
            visited[vertex] = false;
            path.pop_back();
            continue;
        }

        // Take the next edge and remember the updated state.
        const int next = graph.adjacency_list[vertex][nextEdge];
        dfsStack.push({ vertex, nextEdge+1 });

        // Close a cycle back to the beginning of the path.
        if (next == path.front() && path.size() > 1)
        {
            std::vector<int> cycle = path;
            cycle.push_back(next);
            paths.push_back(std::move(cycle));
            continue;
        }

        // Do not revisit vertices in a simple path.
        if (!visited[next]) dfsStack.push({ next, 0 });
    }
}

std::vector<std::vector<int>> PrimePathGen::generatePaths() const {
    std::vector<std::vector<int>> allPaths;
    std::vector<bool> visited(graph.verticesCount, false);
    std::vector<int> path;
    path.reserve(graph.verticesCount);

    //For each edge find prime path candidates
    for (int vertex = 0; vertex < graph.verticesCount; ++vertex)
    {
        for (const int& neighbor : graph.adjacency_list[vertex])
        {
            std::ranges::fill(visited, false);
            path.clear();

            path.push_back(vertex);
            visited[vertex] = true;

            findPrimePathCandidateWithDFS(neighbor,path,allPaths,visited);
        }
    }

    std::ranges::sort(allPaths);

    std::vector<std::vector<int>> primePaths;
    primePaths.reserve(allPaths.size());

    // Filter prime path candidates by removing paths that are subpath of the candidate
    for (const auto& candidatePath : allPaths) {
        const bool isSubpathOfAnotherPath =
            std::ranges::any_of(allPaths,[&](const auto& otherPath){return candidatePath.size() < otherPath.size() && graph.is_subpath(candidatePath, otherPath); });

        if (!isSubpathOfAnotherPath) primePaths.push_back(candidatePath);
    }

    // Adding beginning and the end (again) at the end of the list so that BFS can find paths between prime paths
    primePaths.push_back({ 0 });
    primePaths.push_back({ graph.verticesCount - 1 });

    return primePaths;
}