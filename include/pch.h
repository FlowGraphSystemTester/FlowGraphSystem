#pragma once

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <cassert>
#include <utility>

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <unordered_map>

// This struct is needed to avoid creating two different stacks for traversing edges
struct DFSState {
    int vertex;
    int nextEdge;
};

//This is needed so that std::pair can be used as key in std::unordered_map
struct pairHash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        return h1 ^ h2; //combine with XOR
    }
};