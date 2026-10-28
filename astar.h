// Person 2: A* search (with step recording for the visualization).
#pragma once
#include <vector>
#include "graph.h"

struct QItem { int node; double f, g; };  // f = g + h

// One recorded step of the search (used only by the HTML visualization).
struct Step {
    int leg, cur;                  // cur = node popped (-1 for the initial state of a leg)
    std::vector<QItem> queue;      // priority queue after this step (lowest f first)
    std::vector<QItem> updates;    // entries pushed or updated in this step
    std::vector<int> closed;       // visited nodes in this leg
    std::vector<int> path;         // path found (only on the step that pops the target)
};

// Result of a search over a whole route (start -> waypoints -> end). Also used by Dijkstra.
struct Result {
    std::vector<Step> steps;           // empty unless recorded
    std::vector<int> path;             // full node path
    std::vector<double> legCost;       // cost of each leg
    std::vector<int> legExpanded;      // nodes expanded in each leg
    double cost = 0, micros = 0;       // total cost; average search time (set by main)
    int expanded = 0, pushed = 0;      // nodes popped; queue insertions (incl. updates)
    int failedLeg = -1;                // index of the leg with no path, or -1
};

// Runs A* leg by leg along `route`. Returns false (and sets r.failedLeg) if a leg has no path.
// Set record = false for timing runs, so steps are not stored.
bool runAstar(const Graph& g, const std::vector<int>& route, Result& r, bool record);
