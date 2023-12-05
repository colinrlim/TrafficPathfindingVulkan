#pragma once

#include "MapGraph.h"
#include "../lib/tinyxml2.h"

#include <iostream>
#include <string>

MapGraph::MapGraph(const char* osmFileLocation) : nodes{}, edges{} {
    // used to map node ids from file into more efficient indexes
    std::unordered_map<size_t, size_t> idToIndex{};

    // load map data from XML style file
    tinyxml2::XMLDocument doc{};
    doc.LoadFile(osmFileLocation);
    tinyxml2::XMLElement* root = doc.FirstChildElement("osm");
    
    // iterate through nodes
    for (tinyxml2::XMLElement* element = root->FirstChildElement("node"); element; element = element->NextSiblingElement("node")) {
        size_t id = std::stoll(element->Attribute("id")); // node id
        double x = atof(element->Attribute("lon")); // longitude
        double y = atof(element->Attribute("lat")); // latitude

        // create node
        Node* newNode = new Node{ idToIndex.size(), (x + 82.3535) * 150, (y - 29.6465) * -150 }; // preprocessing; offset & scale the data
        idToIndex[id] = newNode->id;
        nodes.push_back(newNode);
    }

    // initialize adjacency list
    for (size_t i = 0; i < nodes.size(); i++)
        adjacencyList[nodes[i]] = {};

    // iterate through edges ("ways")
    for (tinyxml2::XMLElement* element = root->FirstChildElement("way"); element; element = element->NextSiblingElement("way")) {
        std::vector<size_t> node_refs{};
        for (tinyxml2::XMLElement* nd = element->FirstChildElement("nd"); nd; nd = nd->NextSiblingElement("nd")) {
            size_t ref = std::stoll(nd->Attribute("ref"));
            node_refs.push_back(ref);
        }

        // check if edge is directed or undirected
        bool oneway = false;
        for (tinyxml2::XMLElement* t = element->FirstChildElement("tag"); t; t = t->NextSiblingElement("tag")) {
            if (t->Attribute("k") != "oneway") continue;
            if (t->Attribute("v") == "yes")
                oneway = true;
        }
        // a "way" can have many, many, nodes. separate "way" into multiple edges, composed of two nodes each.
        for (size_t i = 0; i < node_refs.size() - 1; i++) {
            Node* from{ nodes[idToIndex[node_refs[i]]] };
            Node* to{ nodes[idToIndex[node_refs[i + 1]]] };
            if (from == to) continue;

            // weight determined from latitude & longitude distance
            const double dist = std::sqrt((from->x - to->x) * (from->x - to->x) + (from->y - to->y) * (from->y - to->y));

            // create edge
            Edge* newEdge = new Edge{ from, to, dist };
            edges.push_back(newEdge);

            adjacencyList[from].push_back(newEdge);
            if (!oneway) { // if undirected, create second edge going opposite direction
                Edge* newEdgeOpposite = new Edge{ to, from, dist };
                edges.push_back(newEdgeOpposite);
                adjacencyList[to].push_back(newEdgeOpposite);
            }
        }
    }
}

MapGraph::~MapGraph() {
    for (size_t i = 0; i < nodes.size(); i++)
        delete nodes[i];
    for (size_t i = 0; i < edges.size(); i++)
        delete edges[i];
}