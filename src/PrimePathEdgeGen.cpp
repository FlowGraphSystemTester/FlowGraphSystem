#include "PrimePathEdgeGen.h"

PrimePathEdgeGen::PrimePathEdgeGen(const Graph& graph, int ppCount) : g1(graph), g2(Graph(ppCount)) {
    initializeVertexTypes();
}

void PrimePathEdgeGen::initializeVertexTypes() {
    g2.vertex_types.resize(g2.verticesCount, 'p'); //prime path
    g2.vertex_types[g2.verticesCount - 2] = 's';     //source
    g2.vertex_types[g2.verticesCount - 1] = 't';     //sink
}

std::vector<int> PrimePathEdgeGen::bfsFindShortestPath(int startVertex, int endVertex) {
    if (startVertex < 0 || endVertex < 0 || startVertex >= g1.get_adjacency_list().size() || endVertex >= g1.get_adjacency_list().size()) return {}; //assert

    // Return cached result if available.
    const auto cacheKey = std::make_pair(startVertex, endVertex);
    if (bfsCache.contains(cacheKey)) return bfsCache.at(cacheKey);

    std::vector<bool> visited(g1.get_adjacency_list().size(), false);
    std::vector<int> predecessor(g1.get_adjacency_list().size(), -1);
    std::queue<int> queue;

    visited[startVertex] = true;
    queue.push(startVertex);

    while (!queue.empty())
    {
        const int currentVertex = queue.front();
        queue.pop();

        for (const int neighbour : g1.get_adjacency_list()[currentVertex])
        {
            if (visited[neighbour]) continue;

            visited[neighbour] = true;
            predecessor[neighbour] = currentVertex;
            queue.push(neighbour);

            if (neighbour == endVertex)
            {
                std::vector<int> path;
                //Find path by iterating backwards
                for (int vertex = endVertex; vertex != -1; vertex = predecessor[vertex])
                {
                    path.push_back(vertex);
                }
                // And then reverse it
                std::ranges::reverse(path);
                // Cache the answer
                bfsCache[cacheKey] = path;
                return path;
            }
        }
    }

    return {};
}

std::vector<int> PrimePathEdgeGen::mergePaths(const std::vector<int>& firstPath,const std::vector<int>& secondPath) {
    if (firstPath.empty() || secondPath.empty()) return {};

    std::vector<int> mergedPath;
    // The end of the first path is the beginning of the second path.
    if (firstPath.back() == secondPath.front())
    {
        mergedPath.reserve(firstPath.size() + secondPath.size() - 1);
        mergedPath.insert(mergedPath.end(),firstPath.begin(),firstPath.end());
        mergedPath.insert(mergedPath.end(),secondPath.begin() + 1,secondPath.end());
        return mergedPath;
    }

    // Find an overlapping part (suffix) of path_i and prefix of path_j.
    size_t overlapLength = 0;
    for (size_t i = 0; i < firstPath.size(); ++i)
    {
        bool isMatch = true;
        const size_t remainingPathLength = firstPath.size() - i;
        const size_t comparisonLength = std::min(remainingPathLength, secondPath.size());

        for (size_t j = 0; j < comparisonLength; ++j)
        {
            if (firstPath[i + j] != secondPath[j])
            {
                isMatch = false;
                break;
            }
        }

        if (isMatch)
        {
            overlapLength = remainingPathLength;
            break;
        }
    }

    // Join paths using their common part.
    if (overlapLength > 0)
    {
        mergedPath.reserve(firstPath.size() + secondPath.size() - overlapLength);
        mergedPath.insert(mergedPath.end(),firstPath.begin(),firstPath.end() - overlapLength);
        mergedPath.insert(mergedPath.end(),secondPath.begin(),secondPath.end());
        return mergedPath;
    }

    // If the paths do not overlap, connect them using the shortest path.
    const std::vector<int> shortestPath = bfsFindShortestPath(firstPath.back(), secondPath.front());
    if (shortestPath.empty()) return {};

    mergedPath.reserve(firstPath.size() + shortestPath.size() + secondPath.size() - 2);
    mergedPath.insert(mergedPath.end(),firstPath.begin(),firstPath.end());

    // Skip the first and last vertices because they are already present in path_i and path_j
    if (shortestPath.size() > 2) mergedPath.insert(mergedPath.end(),shortestPath.begin() + 1,shortestPath.end() - 1);

    mergedPath.insert(mergedPath.end(),secondPath.begin(),secondPath.end());
    return mergedPath;
}

Graph PrimePathEdgeGen::createTransitionEdgesFromPP(const std::vector<std::vector<int>>& paths) {
    const int sourceVertexIndex = g2.verticesCount - 2;
    const int sinkVertexIndex = g2.verticesCount - 1;

    for (int from = 0; from < sinkVertexIndex; ++from){
        for (int to = 0; to < g2.verticesCount; ++to){
            // Do not create self-loops or edges pointing to the source.
            if (from == to || to == sourceVertexIndex) continue;

            const auto& firstPath = paths[from];
            const auto& secondPath = paths[to];

            const std::vector<int> mergedPath = mergePaths(firstPath, secondPath);
            if (mergedPath.empty()) continue;

            // Check whether the resulting path contains another path as a subpath. If so, the edge is not valid.
            bool containsAnotherPrimePath = false;
            for (int anotherPath = 0; anotherPath < sourceVertexIndex; ++anotherPath)
            {
                if (anotherPath == from || anotherPath == to) continue;
                if (g2.is_subpath(paths[anotherPath], mergedPath))
                {
                    containsAnotherPrimePath = true;
                    break;
                }
            }

            if (!containsAnotherPrimePath)
            {
                g2.addEdge(from, to);
            }
        }
    }
    return g2;
}