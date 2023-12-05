#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "MapGraph.h"

struct PathfindingSolution {
	std::vector<Edge*> checked;
	std::vector<Edge*> path;
	long long beginTimestamp;
	long long endTimestamp;
};

namespace pathfinding {
	PathfindingSolution dijkstra(MapGraph& graph, Node* from, Node* to);
	PathfindingSolution bellmanford(MapGraph& graph, Node* from, Node* to);
}