#pragma once

#include <unordered_map>
#include <vector>

struct Node {
    size_t id;
    double x;
    double y;
};

struct Edge {
    Node* from;
    Node* to;
    double weight;
};

struct MapGraph {
    std::vector<Node*> nodes;
    std::vector<Edge*> edges;
    std::unordered_map<Node*, std::vector<Edge*>> adjacencyList;

    MapGraph(const char* osmFileLocation);

    ~MapGraph();
};