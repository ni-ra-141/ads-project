// Step-by-step A* player. `D` (graph + recorded steps) is defined by export.cpp before this script.
var steps = D.steps, last = steps.length - 1, i = 0;
var route = D.route;
var role = {};
route.forEach(function (v, k) { role[v] = k === 0 ? "start" : k === route.length - 1 ? "end" : "way"; });

// Layout: fit the coordinates into a wide canvas, keeping the aspect ratio.
var SW = 1000, PAD = 55, R = 15;
var xs = D.nodes.map(function (n) { return n[1]; }), ys = D.nodes.map(function (n) { return n[2]; });
var minX = Math.min.apply(null, xs), maxX = Math.max.apply(null, xs);
var minY = Math.min.apply(null, ys), maxY = Math.max.apply(null, ys);
var spanX = (maxX - minX) || 1, spanY = (maxY - minY) || 1;
var SH = Math.max(380, Math.min(760, (SW - 2 * PAD) * spanY / spanX + 2 * PAD));
var sc = Math.min((SW - 2 * PAD) / spanX, (SH - 2 * PAD) / spanY);
var ox = (SW - spanX * sc) / 2, oy = (SH - spanY * sc) / 2;
function px(x) { return ox + (x - minX) * sc; }
function py(y) { return oy + (y - minY) * sc; }
function key(a, b) { return Math.min(a, b) + "-" + Math.max(a, b); }
function $(id) { return document.getElementById(id); }

$("route").innerHTML = route.map(function (v, k) {
  return "<b>" + v + "</b>" + (k < route.length - 1 ? " &rarr; " : "");
}).join("");

function render() {
  var st = steps[i], closed = {}, inq = {}, fval = {}, par = {}, done = {};
  st.c.forEach(function (v) { closed[v] = 1; });
  st.q.forEach(function (e) { inq[e[0]] = 1; fval[e[0]] = e[1]; });
  for (var j = 0; j <= i; j++) {
    var s = steps[j];
    if (s.leg === st.leg) s.u.forEach(function (e) { par[e[0]] = s.cur; });
    for (var m = 0; m + 1 < s.p.length; m++) done[key(s.p[m], s.p[m + 1])] = 1;
  }
  var tree = {};
  Object.keys(par).forEach(function (v) { if (closed[v] || inq[v]) tree[key(v, par[v])] = 1; });

  var svg = '<svg viewBox="0 0 ' + SW + " " + SH + '" xmlns="http://www.w3.org/2000/svg">';
  var labels = "";
  D.edges.forEach(function (e) {
    var a = D.nodes[e[0]], b = D.nodes[e[1]], k = key(e[0], e[1]);
    var x1 = px(a[1]), y1 = py(a[2]), x2 = px(b[1]), y2 = py(b[2]);
    var c = "#cbd5e1", w = 1.5;
    if (done[k]) { c = "#16a34a"; w = 6; } else if (tree[k]) { c = "#60a5fa"; w = 3.5; }
    svg += '<line x1="' + x1 + '" y1="' + y1 + '" x2="' + x2 + '" y2="' + y2 + '" stroke="' + c +
           '" stroke-width="' + w + '" stroke-linecap="round"/>';
    labels += '<text x="' + (x1 + x2) / 2 + '" y="' + ((y1 + y2) / 2 + 4) + '" font-size="11" fill="#64748b" ' +
              'text-anchor="middle" stroke="#fff" stroke-width="4" paint-order="stroke">' + e[2] + "</text>";
  });
  svg += labels;
  var ring = { start: "#16a34a", end: "#dc2626", way: "#9333ea" };
  D.nodes.forEach(function (n) {
    var id = n[0], x = px(n[1]), y = py(n[2]);
    var fill = st.cur === id ? "#fb923c" : closed[id] ? "#bfdbfe" : inq[id] ? "#fde68a" : "#ffffff";
    var stroke = role[id] ? ring[role[id]] : "#94a3b8", sw = role[id] ? 4 : 2;
    svg += '<circle cx="' + x + '" cy="' + y + '" r="' + R + '" fill="' + fill + '" stroke="' + stroke +
           '" stroke-width="' + sw + '"/>';
    svg += '<text x="' + x + '" y="' + (y + 4.5) + '" font-size="13" font-weight="600" fill="#0f172a" text-anchor="middle">' + id + "</text>";
    if (inq[id])
      svg += '<text x="' + x + '" y="' + (y + R + 13) + '" font-size="11" fill="#92400e" text-anchor="middle" ' +
             'stroke="#fff" stroke-width="3" paint-order="stroke">f=' + fval[id] + "</text>";
  });
  $("graph").innerHTML = svg + "</svg>";

  // "This step" card
  var leg = "Leg " + (st.leg + 1) + " of " + (route.length - 1) + ": " + route[st.leg] + " &rarr; " + route[st.leg + 1];
  var msg = "<div class='hint'>" + leg + "</div>";
  if (st.cur < 0) msg += "Start by putting node <b>" + route[st.leg] + "</b> in the queue.";
  else {
    msg += "Popped node <b>" + st.cur + "</b> (lowest f).<br>";
    if (st.p.length) msg += "It is the target. Path: <b>" + st.p.join(" &rarr; ") + "</b>";
    else if (st.u.length) msg += "Pushed or updated: " + st.u.map(function (e) { return "<span class='chip'>" + e[0] + "</span>"; }).join("");
    else msg += "No neighbours improved.";
  }
  if (i === last) {
    msg += "<br><b>Finished.</b> Total cost " + D.cost + ", " + D.expanded + " nodes expanded.";
    msg += "<div class='compare'>Dijkstra would have expanded <b>" + D.dijkstra + "</b> nodes for the same route (A*: " + D.expanded + ").</div>";
  }
  $("msg").innerHTML = msg;

  // Priority queue table
  var upd = {};
  st.u.forEach(function (e) { upd[e[0]] = 1; });
  var q = "<table><tr><th>Node</th><th>g</th><th>h</th><th>f</th></tr>";
  st.q.slice(0, 12).forEach(function (e, r) {
    q += "<tr class='" + (r === 0 ? "next " : "") + (upd[e[0]] ? "new" : "") + "'><td>" + e[0] + "</td><td>" + e[2] +
         "</td><td>" + (e[1] - e[2]).toFixed(2) + "</td><td>" + e[1] + "</td></tr>";
  });
  q += "</table>";
  if (!st.q.length) q = "<div class='hint'>(empty)</div>";
  if (st.q.length > 12) q += "<div class='more'>+" + (st.q.length - 12) + " more</div>";
  else if (st.q.length) q += "<div class='more'>Bold row = popped next. Shaded = just added or updated.</div>";
  $("queue").innerHTML = q;

  $("count").textContent = "Step " + i + " / " + last;
  $("prev").disabled = $("reset").disabled = i === 0;
  $("next").disabled = i === last;
}
function go(d) { var n = i + d; if (n >= 0 && n <= last) { i = n; render(); } }
$("next").onclick = function () { go(1); };
$("prev").onclick = function () { go(-1); };
$("reset").onclick = function () { i = 0; render(); };
document.onkeydown = function (e) { if (e.key === "ArrowRight") go(1); if (e.key === "ArrowLeft") go(-1); };
render();
