// Person 1: graph data structure and .graph file loading.
//
// .graph format (undirected):
//   N M
//   id x y        (N lines, ids are 0..N-1, each used once)
//   u v weight    (M lines)
#pragma once
#include <string>
#include <vector>
using namespace std;

struct Edge { 
    int to; 
    double w; 
};

struct EdgeRec { 
    int u, v; 
    double w; 
};

struct Graph {
    int n = 0;
    vector<double> x, y;                // node coordinates
    vector<vector<Edge>> adj;      // adjacency list
    vector<EdgeRec> edges;              // each undirected edge once (for drawing)
    double scale = 1.0;                      // heuristic scale (< 1 if some edge is shorter than
                                             // the straight-line distance, keeps A* optimal)
};

// Returns false if the file is missing or invalid (bad numbers, ids out of range,
// duplicate node ids, negative weights).
bool loadGraph(const string& file, Graph& g);
