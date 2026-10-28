#include "graph.h"
#include <algorithm>
#include <cmath>
#include <fstream>
using namespace std;

bool loadGraph(const string& file, Graph& g) {
    ifstream in(file);
    int m;
    if (!(in >> g.n >> m) || g.n <= 0 || m < 0) return false;
    g.x.assign(g.n, 0);
    g.y.assign(g.n, 0);
    g.adj.assign(g.n, {});
    g.edges.clear();
    g.scale = 1.0;

    vector<bool> seen(g.n, false);
    for (int i = 0; i < g.n; i++) {
        int id;
        double px, py;
        if (!(in >> id >> px >> py) || id < 0 || id >= g.n || seen[id]) return false;
        seen[id] = true;
        g.x[id] = px;
        g.y[id] = py;
    }
    for (int i = 0; i < m; i++) {
        int u, v;
        double w;
        if (!(in >> u >> v >> w) || u < 0 || v < 0 || u >= g.n || v >= g.n || w < 0) return false;
        g.adj[u].push_back({v, w});
        g.adj[v].push_back({u, w});
        g.edges.push_back({u, v, w});
        double d = hypot(g.x[u] - g.x[v], g.y[u] - g.y[v]);
        if (d > 0) g.scale = min(g.scale, w / d);
    }
    return true;
}
