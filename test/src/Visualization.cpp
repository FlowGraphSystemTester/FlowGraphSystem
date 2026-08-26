//
// Created by M.Stanek on 17.08.2026.
//
#include "Visualization.h"
#include "boost/json/src.hpp"
#include <graphviz/gvc.h>

std::string adj_to_dot(AdjacencyListT& adj_list) {
    std::string dot {"digraph G { "};
    int v_id = 0;
    for (auto v : adj_list) {
        for (auto n : v) {
            dot += std::format("{0}->{1}; ", v_id, n);
        }
        ++v_id;
    }
    return dot + "}";
}

std::string net_to_dot(NetAdjacencyListT &adj_list) {
    std::string dot {"digraph G { "};
    for (auto v : adj_list) {
        for (auto n : v) {
            dot += std::format("{0}->{1} [label=\"[{2},{3},{4}]\"]; ",
                n.source, n.destination, n.lowerBound, n.currentFlow, n.upperBound);
        }
    }
    return dot + "}";
}

std::string net_to_dot_named(NetAdjacencyListT &adj_list, RevIdMapT path_map) {
    std::string dot {"digraph G { "};
    for (auto v : adj_list) {
        for (auto n : v) {
            auto source_name = path_map[n.source];
            auto destination_name = path_map[n.destination];

            dot += std::format("{0}->{1} [label=\"[{2},{3},{4}]\"]; ",
                source_name.empty() ? std::to_string(n.source) : source_name,
                destination_name.empty() ? std::to_string(n.destination) : destination_name
                , n.lowerBound, n.currentFlow, n.upperBound);
        }
    }
    return dot + "}";
}

RevIdMapT identify_names_alg2(TCR_G2_to_G3 &tcr) {
    RevIdMapT result;

    auto path_num = tcr.test_case.in_pps.size();
    for (int v = 0; v < path_num - 2; ++v) {
        result[v] = "p" + std::to_string(v);
    }

    result[path_num-2] = "s";
    result[path_num-1] = "t";

    for (int v = path_num; v < tcr.output_graph.size(); ++v) {
        result[v] = "\"(";
        for (int n = 0; n < tcr.output_graph[v].size(); ++n) {
            result[v].append(std::to_string(tcr.output_graph[v][n]));
            if (n < tcr.output_graph[v].size() - 1) {
                result[v].append(" ");
            }
        }
        result[v].append(")\"");
    }

    return result;
}

RevIdMapT identify_names_PP_to_G2(TCR_PP_to_G2 &tcr) {
    RevIdMapT result;

    auto path_num = tcr.test_case.in_pps.size();
    for (int v = 0; v < path_num - 2; ++v) {
        result[v] = "p" + std::to_string(v);
    }

    result[path_num-2] = "s";
    result[path_num-1] = "t";

    for (int v = path_num; v < tcr.output_graph.size(); ++v) {
        result[v] = "\"(";
        for (int n = 0; n < tcr.output_graph[v].size(); ++n) {
            result[v].append(std::to_string(tcr.output_graph[v][n]));
            if (n < tcr.output_graph[v].size() - 1) {
                result[v].append(" ");
            }
        }
        result[v].append(")\"");
    }

    return result;
}

RevIdMapT identify_names_PP_to_G2_map(AdjacencyListT &adj, int path_num) {
    RevIdMapT result;

    for (int v = 0; v < path_num - 2; ++v) {
        result[v] = "p" + std::to_string(v);
    }

    result[path_num-2] = "s";
    result[path_num-1] = "t";

    for (int v = path_num; v < adj.size(); ++v) {
        result[v] = "\"(";
        for (int n = 0; n < adj[v].size(); ++n) {
            result[v].append(std::to_string(adj[v][n]));
            if (n < adj[v].size() - 1) {
                result[v].append(" ");
            }
        }
        result[v].append(")\"");
    }

    return result;
}

RevIdMapT update_map_with_vertices(AdjacencyListT &adj, RevIdMapT rev_id_map) {
    auto new_rev_id_map = rev_id_map;

    int vert_id = 0;
    for (int v = 0; v < adj.size(); ++v) {
        if (!new_rev_id_map.contains(v)) {
            new_rev_id_map[v] = "v" + std::to_string(vert_id++);
        }
    }

    return new_rev_id_map;
}

std::string adj_to_dot_named(AdjacencyListT &adj_list, RevIdMapT rev_id_map) {
    std::string dot {"digraph G { "};
    int v_id = 0;
    for (auto v : adj_list) {
        std::string source_name = rev_id_map[v_id];
        for (auto n : v) {
            std::string dest_name = rev_id_map[n];
            dot += std::format("{0}->{1}; ", source_name.empty() ? std::to_string(v_id) : source_name
                , dest_name.empty() ? std::to_string(n) : dest_name);
        }
        ++v_id;
    }
    return dot + "}";
}

std::string path_to_dot(PP &path) {
    std::string dot {"digraph G { "};
    for (int v = 0; v < path.size() - 1; v++) {
        dot += std::format("{0}->{1}; ", path[v], path[v+1]);
    }
    return dot + "}";
};

void svg_from_dot(std::string dot_str, std::string output_path) {
    GVC_t* gvc = gvContext();

    Agraph_t* g = agmemread(dot_str.c_str());
    if (!g) {
        std::cerr << "Failed to parse dot string" << std::endl;
        gvFreeContext(gvc);
        return;
    }

    gvLayout(gvc, g, "dot");
    gvRenderFilename(gvc, g, "svg", output_path.c_str());
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
}

void png_from_dot(std::string dot_str, std::string output_path) {
    GVC_t* gvc = gvContext();

    Agraph_t* g = agmemread(dot_str.c_str());
    if (!g) {
        std::cerr << "Failed to parse dot string" << std::endl;
        gvFreeContext(gvc);
        return;
    }

    gvLayout(gvc, g, "dot");
    gvRenderFilename(gvc, g, "png", output_path.c_str());
    gvFreeLayout(gvc, g);
    agclose(g);
    gvFreeContext(gvc);
}