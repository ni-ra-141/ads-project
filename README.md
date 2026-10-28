# A* Shortest Pathfinding with Step-by-Step Visualization

Finds the shortest route between nodes of a weighted graph using A*, prints an analysis in the
terminal (with Dijkstra as a baseline), and generates `visualization.html`, where you step through
the A* search one click at a time and watch the priority queue change.

## Build and run

```
make                 # builds ./pathfinder  (or: g++ -O2 -std=c++17 *.cpp -o pathfinder)
./pathfinder         # then answer the prompts
make test            # runs the test cases in tests/run_tests.sh
```

Prompts: graph file (for example `graphs/graph50.graph`), start node, end node, number of
intermediate nodes, then each intermediate node. Intermediate nodes are visited in the order
entered. Bad input is re-asked instead of ending the program.

Open the generated `visualization.html` in any browser (no server needed). Use **Next**,
**Previous** and **Reset**, or the left/right arrow keys.

## What you get

- **Terminal:** for A* and for Dijkstra, the path, total cost, nodes expanded, queue insertions,
  and average time, plus a per-leg breakdown when there are waypoints and a comparison summary.
- **Page:** the graph (A* only), the priority queue as a table of node, g, h, f with the next node
  to pop in bold and new entries shaded, and a note of what each step did. On the last step it
  also says how many nodes Dijkstra would have expanded.

## .graph file format (undirected)

```
N M
id x y        (N lines, ids 0..N-1, each used once)
u v weight    (M lines)
```

A* needs the coordinates for its heuristic (straight-line distance). If some edge is shorter than
the straight-line distance between its ends, the heuristic is scaled down automatically so the
result stays optimal; the program prints a note when this happens.

## Files and owners

| Person | Files | Responsibility |
|---|---|---|
| 1. Graph | `graph.h`, `graph.cpp` | Graph structures, loading and validating `.graph` files |
| 2. Algorithms | `astar.h`, `astar.cpp`, `dijkstra.h`, `dijkstra.cpp` | A* with step recording, Dijkstra baseline |
| 3. Main program | `main.cpp` | User input, timing, terminal analysis |
| 4. Visualization | `export.h`, `export.cpp`, `template.html`, `style.css`, `script.js` | Builds the HTML page from the results |

Dependencies: `graph` is used by `astar`, `dijkstra` and `export`; `main` uses all of them.

`template.html`, `style.css` and `script.js` must stay in the same folder as the `pathfinder`
program. The program combines them into one self-contained `visualization.html`.

## Other folders

- `graphs/`: sample graphs (`sample.graph` with 16 nodes, `graph50.graph` with 50 nodes).
- `tests/`: edge-case graphs and `run_tests.sh` (12 test cases: normal routes, waypoints, start
  equals end, no path, scaled heuristic, invalid files, bad input, A* vs Dijkstra on 30 routes,
  and a check of the generated HTML).
