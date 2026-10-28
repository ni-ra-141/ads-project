#include "dijkstra.h"
#include <algorithm>
#include <set>
using namespace std;

static const double INF = 1e18;

// Plain Dijkstra for one leg, stopping as soon as the target is popped (same as A* with h = 0).
static bool dijkstraLeg(const Graph& g, int s, int t, Result& r) {
    vector<double> dist(g.n, INF);
    vector<int> parent(g.n, -1);
    vector<bool> done(g.n, false);
    set<pair<double, int>> pq;
    int expandedBefore = r.expanded;

    dist[s] = 0;
    pq.insert({0.0, s});
    r.pushed++;
    while (!pq.empty()) {
        int u = pq.begin()->second;
        pq.erase(pq.begin());
        done[u] = true;
        r.expanded++;
        if (u == t) {
            vector<int> p;
            for (int v = t; v != -1; v = parent[v]) p.push_back(v);
            reverse(p.begin(), p.end());
            r.cost += dist[t];
            r.legCost.push_back(dist[t]);
            r.legExpanded.push_back(r.expanded - expandedBefore);
            r.path.insert(r.path.end(), p.begin() + (r.path.empty() ? 0 : 1), p.end());
            return true;
        }
        for (const Edge& e : g.adj[u]) {
            if (done[e.to]) continue;
            double nd = dist[u] + e.w;
            if (nd < dist[e.to]) {
                if (dist[e.to] < INF) pq.erase({dist[e.to], e.to});
                dist[e.to] = nd;
                parent[e.to] = u;
                pq.insert({nd, e.to});
                r.pushed++;
            }
        }
    }
    return false;
}

bool runDijkstra(const Graph& g, const vector<int>& route, Result& r) {
    for (size_t i = 0; i + 1 < route.size(); i++)
        if (!dijkstraLeg(g, route[i], route[i + 1], r)) {
            r.failedLeg = (int)i;
            return false;
        }
    return true;
}
