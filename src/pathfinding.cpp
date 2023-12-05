#include "pathfinding.h"

#include <queue>
#include <chrono>
#include <unordered_set>
#include <iostream>
#include <set>

PathfindingSolution pathfinding::dijkstra(MapGraph& graph, Node* from, Node* to) {
    const auto beginTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    std::vector<Edge*> predecessor(graph.nodes.size(), nullptr);
    std::vector<double> distance(graph.nodes.size(), DBL_MAX);
    distance[from->id] = 0.0;

    std::set<std::pair<double, Node*>> todo;
    for (auto& node : graph.nodes) {
        todo.insert({ distance[node->id], node });
    }

    std::vector<Edge*> checked{};
    std::unordered_set<Edge*> checkedSet{};
    checked.reserve(graph.edges.size());
    checkedSet.reserve(graph.edges.size());

    while (!todo.empty()) {
        Node* v = todo.begin()->second;
        todo.erase(todo.begin());

        for (auto& edge : graph.adjacencyList[v]) {
            Node* toNode = edge->to;
            double newDist = distance[v->id] + edge->weight;

            if (newDist < distance[toNode->id]) {
                todo.erase({ distance[toNode->id], toNode });
                distance[toNode->id] = newDist;
                predecessor[toNode->id] = edge;
                todo.insert({ newDist, toNode });
            }

            if (checkedSet.find(edge) == checkedSet.end()) {
                checked.push_back(edge);
                checkedSet.insert(edge);
            }
        }
    }

    const auto endTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    if (predecessor[to->id] == nullptr) return { checked, {}, beginTimestamp, endTimestamp };

    std::vector<Edge*> path;
    for (Node* current = to; current != from; current = predecessor[current->id]->from) {
        path.push_back(predecessor[current->id]);
    }

    std::reverse(path.begin(), path.end());

    return { checked, path, beginTimestamp, endTimestamp };
}

PathfindingSolution pathfinding::bellmanford(MapGraph& graph, Node* from, Node* to) {
    const auto beginTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    std::vector<Edge*> predecessor(graph.nodes.size(), nullptr);
    std::vector<double> distance(graph.nodes.size(), DBL_MAX);
    distance[from->id] = 0.0;

    std::vector<Edge*> checked{};
    std::unordered_set<Edge*> checkedSet{};
    checked.reserve(graph.edges.size());
    checkedSet.reserve(graph.edges.size());

    bool updated;
    for (size_t i = 0; i < graph.nodes.size() - 1; ++i) {
        updated = false;
        for (auto& edge : graph.edges) {
            Node* u = edge->from;
            Node* v = edge->to;
            double weight = edge->weight;

            if (distance[u->id] != DBL_MAX && distance[u->id] + weight < distance[v->id]) {
                distance[v->id] = distance[u->id] + weight;
                predecessor[v->id] = edge;
                updated = true;
            }

            if (checkedSet.find(edge) == checkedSet.end()) {
                checked.push_back(edge);
                checkedSet.insert(edge);
            }
        }
        if (!updated) break;
    }

    for (auto& edge : graph.edges) {
        Node* u = edge->from;
        Node* v = edge->to;
        double weight = edge->weight;

        if (distance[u->id] != DBL_MAX && distance[u->id] + weight < distance[v->id]) {
            return { checked, {}, beginTimestamp, std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() };
        }
    }

    const auto endTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    if (predecessor[to->id] == nullptr) return { checked, {}, beginTimestamp, endTimestamp };

    std::vector<Edge*> path;
    for (Node* current = to; current != from && predecessor[current->id] != nullptr; current = predecessor[current->id]->from) {
        path.push_back(predecessor[current->id]);
    }

    if (path.empty() && from != to) return { checked, {}, beginTimestamp, endTimestamp };

    std::reverse(path.begin(), path.end());

    return { checked, path, beginTimestamp, endTimestamp };
}
