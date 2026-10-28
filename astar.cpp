#include "astar.h"
#include <algorithm>
#include <cmath>
#include <set>
using namespace std;

static const double INF = 1e18;

// One A* leg (s -> t). The open set is a std::set ordered by (f, node), so it acts as a
// priority queue with decrease-key (erase the old entry, insert the new one).
static bool astarLeg(const Graph& g, int s, int t, int leg, Result& r, bool record) {
    vector<double> dist(g.n, INF);
    vector<int> parent(g.n, -1);
    vector<bool> closed(g.n, false);
    vector<int> closedList;
    set<pair<double, int>> open;
    int expandedBefore = r.expanded;

    auto h = [&](int v) { 
        return g.scale * hypot(g.x[v] - g.x[t], g.y[v] - g.y[t]); 
    };
    
    auto snapshot = [&](int cur, const vector<QItem>& upd, const vector<int>& path) {
        if(!record) return;
        Step st;
        st.leg = leg;
        st.cur = cur;
        for(auto& p : open) st.queue.push_back({p.second, p.first, dist[p.second]});
        st.updates = upd;
        st.closed = closedList;
        st.path = path;
        r.steps.push_back(st);
    };

    dist[s] = 0;
    open.insert({h(s), s});
    r.pushed++;
    snapshot(-1, {}, {});

    while(!open.empty()) {
        int u = open.begin()->second;
        open.erase(open.begin());
        closed[u] = true;
        closedList.push_back(u);
        r.expanded++;

        vector<QItem> upd;
        if(u == t) {
            vector<int> p;
            for(int v = t; v != -1; v = parent[v]) p.push_back(v);
            reverse(p.begin(), p.end());
            r.cost += dist[t];
            r.legCost.push_back(dist[t]);
            r.legExpanded.push_back(r.expanded - expandedBefore);
            r.path.insert(r.path.end(), p.begin() + (r.path.empty() ? 0 : 1), p.end());
            snapshot(u, upd, p);
            return true;
        }
        for(const Edge& e : g.adj[u]) {
            if(closed[e.to]) continue;
            double ng = dist[u] + e.w;
            if(ng < dist[e.to]) {
                if(dist[e.to] < INF) open.erase({dist[e.to] + h(e.to), e.to});
                dist[e.to] = ng;
                parent[e.to] = u;
                double f = ng + h(e.to);
                open.insert({f, e.to});
                r.pushed++;
                upd.push_back({e.to, f, ng});
            }
        }
        snapshot(u, upd, {});
    }
    return false;
}

bool runAstar(const Graph& g, const vector<int>& route, Result& r, bool record) {
    for(size_t i = 0; i + 1 < route.size(); i++)
        if(!astarLeg(g, route[i], route[i + 1], (int)i, r, record)) {
            r.failedLeg = (int)i;
            return false;
        }
    return true;
}
