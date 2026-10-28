// Person 4: builds the self-contained visualization.html from the search result.
#pragma once
#include <string>
#include <vector>
#include "astar.h"
#include "graph.h"
using namespace std;

// Reads template.html, style.css and script.js from webDir, embeds the graph and the recorded
// A* steps as JSON, and writes one self-contained HTML file to outFile.
// dijkstraExpanded is shown on the last step of the page. Returns false (with a message on
// stderr) if a web file cannot be read or the output cannot be written.
bool writeHtml(const string& webDir, const string& outFile, const Graph& g, const vector<int>& route, const Result& astar, int dijkstraExpanded);
