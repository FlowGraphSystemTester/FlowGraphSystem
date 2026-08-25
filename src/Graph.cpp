#include "Graph.h"

Graph::Graph(int verticesCount) : verticesCount(verticesCount), number_of_edges(0), adjacency_list(verticesCount), vertex_types() {}

const std::vector<char>& Graph::get_vertex_types() const {
    return vertex_types;
}

void Graph::addEdge(int sourceVertex, int destinationVertex) {
    adjacency_list[sourceVertex].push_back(destinationVertex);
    number_of_edges++;
}

const std::vector<std::vector<int>>& Graph::get_adjacency_list() const {
    return adjacency_list;
}

int Graph::get_number_of_vertices() const {
    return verticesCount;
}

bool Graph::hasDirectEdge(int from, int to) const {
    if (from < 0 || from >= verticesCount) return false;
    return std::ranges::find(adjacency_list[from], to) != adjacency_list[from].end();
}

bool Graph::is_subpath(const std::vector<int>& first, const std::vector<int>& second) const {
    if (first.empty() || second.empty()) return false;
    if (first.size() > second.size()) return false;

    for (size_t i = 0; i <= second.size() - first.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < first.size(); ++j) {
            if (second[i + j] != first[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return true;
        }
    }

    return false;
}
