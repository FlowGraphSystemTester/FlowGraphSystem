#pragma once
#include "pch.h"

class Graph {
//private:
public:
    int verticesCount;
    int number_of_edges;
    std::vector<std::vector<int>> adjacency_list;
    std::vector<char> vertex_types;

public:
    explicit Graph(int number_of_vertices);

    const std::vector<char>& get_vertex_types() const;
    void addEdge(int sourceVertex, int destinationVertex);
    const std::vector<std::vector<int>>& get_adjacency_list() const;
    int get_number_of_vertices() const;
    bool hasDirectEdge(int from, int to) const;
    bool is_subpath(const std::vector<int>& first, const std::vector<int>& second) const;
};
