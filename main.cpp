// Person 3: user input, timing and terminal analysis.
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include "astar.h"
#include "dijkstra.h"
#include "export.h"
#include "graph.h"
using namespace std;

// Asks until the user enters a whole number in [lo, hi]. Returns false on end of input.
static bool readInt(const string& prompt, int lo, int hi, int& out) {
    while (true) {
        cout << prompt;
        if (!(cin >> out)) {
            if (cin.eof()) return false;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a whole number.\n";
            continue;
        }
        if (out < lo || out > hi) {
            cout << "Invalid: enter a number from " << lo << " to " << hi << ".\n";
            continue;
        }
        return true;
    }
}

static void printPath(const vector<int>& p) {
    for (size_t i = 0; i < p.size(); i++) cout << p[i] << (i + 1 < p.size() ? " -> " : "\n");
}

static void printResult(const string& title, const vector<int>& route, const Result& r, int nodes) {
    cout << "\n" << title << "\n";
    cout << "  Full path        : ";
    printPath(r.path);
    cout << "  Total cost       : " << r.cost << "\n";
    cout << "  Nodes expanded   : " << r.expanded << " (of " << nodes << ")\n";
    cout << "  Queue insertions : " << r.pushed << " (includes updates)\n";
    cout << "  Time             : " << r.micros << " us (average, search only)\n";
    if (r.legCost.size() > 1) {
        cout << "  Per leg:\n";
        for (size_t i = 0; i < r.legCost.size(); i++)
            cout << "    " << route[i] << " -> " << route[i + 1] << " : cost " << r.legCost[i] << ", "
                 << r.legExpanded[i] << " nodes expanded\n";
    }
}

// Average time of repeated runs, in microseconds.
template <class F>
static double averageMicros(F run) {
    const int reps = 500;
    auto t0 = chrono::steady_clock::now();
    for (int i = 0; i < reps; i++) run();
    return chrono::duration<double, micro>(chrono::steady_clock::now() - t0).count() / reps;
}

int main(int, char** argv) {
    // The web files (template.html, style.css, script.js) are looked up next to the program.
    string exe = argv[0], webDir = ".";
    size_t slash = exe.find_last_of("/\\");
    if (slash != string::npos) webDir = exe.substr(0, slash);

    // 1. Graph file
    Graph g;
    string file;
    while (true) {
        cout << "Graph file: ";
        if (!(cin >> file)) return 1;
        Graph tmp;
        if (loadGraph(file, tmp)) {
            g = tmp;
            break;
        }
        cout << "Could not read a valid graph from '" << file << "'. Try again.\n";
    }
    cout << "Loaded " << g.n << " nodes, " << g.edges.size() << " edges (ids 0.." << g.n - 1 << ").\n";
    if (g.scale < 1.0)
        cout << "Note: some edges are shorter than the straight-line distance; heuristic scaled by "
             << g.scale << " to stay admissible.\n";

    // 2. Start, end and intermediate nodes (visited in the order entered)
    int start, end, k;
    if (!readInt("Start node: ", 0, g.n - 1, start) || !readInt("End node: ", 0, g.n - 1, end) ||
        !readInt("Number of intermediate nodes: ", 0, g.n, k))
        return 1;
    vector<int> route = {start};
    for (int i = 0; i < k; i++) {
        int v;
        if (!readInt("Intermediate node " + to_string(i + 1) + ": ", 0, g.n - 1, v)) return 1;
        route.push_back(v);
    }
    route.push_back(end);

    // 3. Run both algorithms (A* records steps for the page, Dijkstra is analysis only)
    Result astar, dijk;
    bool okA = runAstar(g, route, astar, true);
    bool okD = runDijkstra(g, route, dijk);
    if (!okA || !okD) {
        int leg = okA ? dijk.failedLeg : astar.failedLeg;
        cout << "\nNo path from " << route[leg] << " to " << route[leg + 1] << ".\n";
        return 1;
    }
    astar.micros = averageMicros([&] { Result t; runAstar(g, route, t, false); });
    dijk.micros = averageMicros([&] { Result t; runDijkstra(g, route, t); });

    // 4. Terminal analysis
    cout << fixed << setprecision(2);
    cout << "\nRoute: ";
    printPath(route);
    printResult("A* (Euclidean heuristic)", route, astar, g.n);
    printResult("Dijkstra (baseline)", route, dijk, g.n);

    cout << "\nComparison with Dijkstra\n";
    if (fabs(astar.cost - dijk.cost) > 1e-6) cout << "  WARNING: costs differ.\n";
    else cout << "  Same optimal cost: " << astar.cost << "\n";
    cout << "  Nodes expanded   : A* " << astar.expanded << ", Dijkstra " << dijk.expanded;
    int diff = dijk.expanded - astar.expanded;
    if (diff > 0) cout << "  (A* expanded " << 100.0 * diff / dijk.expanded << "% fewer)";
    else if (diff < 0) cout << "  (A* expanded " << 100.0 * -diff / dijk.expanded << "% more)";
    cout << "\n  Time             : A* " << astar.micros << " us, Dijkstra " << dijk.micros << " us\n";

    // 5. HTML visualization
    if (!writeHtml(webDir, "visualization.html", g, route, astar, dijk.expanded)) return 1;
    cout << "\nWrote visualization.html (open it in a browser).\n";
    return 0;
}
