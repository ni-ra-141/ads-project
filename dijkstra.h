// Person 2: Dijkstra baseline. Terminal analysis only, it is not drawn in the visualization.
#pragma once
#include <vector>
#include "astar.h"
#include "graph.h"
using namespace std;

// Same interface as runAstar, but without a heuristic. Fills cost, path, expanded, pushed and
// the per-leg vectors of r. Returns false (and sets r.failedLeg) if a leg has no path.
bool runDijkstra(const Graph& g, const vector<int>& route, Result& r);
