#include "export.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

static string num(double v) {
    ostringstream o;
    o << fixed << setprecision(2) << v;
    return o.str();
}

static string ints(const vector<int>& v) {
    string s = "[";
    for (size_t i = 0; i < v.size(); i++) s += (i ? "," : "") + to_string(v[i]);
    return s + "]";
}

static string items(const vector<QItem>& v) {
    string s = "[";
    for (size_t i = 0; i < v.size(); i++)
        s += string(i ? "," : "") + "[" + to_string(v[i].node) + "," + num(v[i].f) + "," + num(v[i].g) + "]";
    return s + "]";
}

static string stepsJson(const Result& r) {
    string s = "[";
    for (size_t i = 0; i < r.steps.size(); i++) {
        const Step& st = r.steps[i];
        s += string(i ? "," : "") + "{\"leg\":" + to_string(st.leg) + ",\"cur\":" + to_string(st.cur) +
             ",\"q\":" + items(st.queue) + ",\"u\":" + items(st.updates) + ",\"c\":" + ints(st.closed) +
             ",\"p\":" + ints(st.path) + "}";
    }
    return s + "]";
}

static bool readFile(const string& path, string& out) {
    ifstream in(path, ios::binary);
    if (!in) {
        cerr << "Cannot read '" << path << "'. Keep template.html, style.css and script.js next to the program.\n";
        return false;
    }
    stringstream ss;
    ss << in.rdbuf();
    out = ss.str();
    return true;
}

static bool replaceOnce(string& text, const string& marker, const string& with) {
    size_t pos = text.find(marker);
    if (pos == string::npos) {
        cerr << "template.html is missing the marker " << marker << "\n";
        return false;
    }
    text.replace(pos, marker.size(), with);
    return true;
}

bool writeHtml(const string& webDir, const string& outFile, const Graph& g, const vector<int>& route,
               const Result& astar, int dijkstraExpanded) {
    string page, css, js;
    string dir = webDir.empty() ? "" : webDir + "/";
    if (!readFile(dir + "template.html", page) || !readFile(dir + "style.css", css) ||
        !readFile(dir + "script.js", js))
        return false;

    string data = "{\"route\":" + ints(route) + ",\"cost\":" + num(astar.cost) +
                  ",\"expanded\":" + to_string(astar.expanded) + ",\"dijkstra\":" + to_string(dijkstraExpanded) +
                  ",\"nodes\":[";
    for (int i = 0; i < g.n; i++)
        data += string(i ? "," : "") + "[" + to_string(i) + "," + num(g.x[i]) + "," + num(g.y[i]) + "]";
    data += "],\"edges\":[";
    for (size_t i = 0; i < g.edges.size(); i++)
        data += string(i ? "," : "") + "[" + to_string(g.edges[i].u) + "," + to_string(g.edges[i].v) + "," +
                num(g.edges[i].w) + "]";
    data += "],\"steps\":" + stepsJson(astar) + "}";

    if (!replaceOnce(page, "<!--STYLE-->", "<style>\n" + css + "</style>") ||
        !replaceOnce(page, "<!--SCRIPT-->", "<script>\nvar D = " + data + ";\n" + js + "</script>"))
        return false;

    ofstream out(outFile);
    if (!out) {
        cerr << "Cannot write '" << outFile << "'.\n";
        return false;
    }
    out << page;
    return true;
}
