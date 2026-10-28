#!/bin/bash
# Runs the test cases against ./pathfinder and prints PASS/FAIL for each (use: make test).
cd "$(dirname "$0")/.." || exit 1
BIN=./pathfinder
SAMPLE=graphs/sample.graph
G50=graphs/graph50.graph
pass=0
fail=0

# run "<input lines>" -> sets $out (all output) and $code (exit status)
run() { out=$(printf "$1" | $BIN 2>&1); code=$?; }
has() { grep -qF -- "$1" <<<"$out"; }
report() {
    if [ "$3" = 1 ]; then echo "PASS  $1  $2"; pass=$((pass + 1)); else echo "FAIL  $1  $2"; fail=$((fail + 1)); fi
}
t() { # id, description, condition result (0 = true)
    if [ "$3" -eq 0 ]; then report "$1" "$2" 1; else report "$1" "$2" 0; fi
}

[ -x "$BIN" ] || { echo "Build first: make"; exit 1; }

run "$SAMPLE\n4\n15\n0\n"
has "Same optimal cost: 45.00"; t TC-01 "sample graph 4 -> 15, no waypoints: A* and Dijkstra agree (cost 45.00)" $?

run "$G50\n0\n49\n1\n24\n"
has "Same optimal cost: 133.80"; t TC-02 "graph50 0 -> 49 via 24: optimal cost 133.80" $?

run "$SAMPLE\n5\n5\n0\n"
has "Total cost       : 0.00"; t TC-03 "start equals end: cost 0" $?

run "$G50\n0\n49\n3\n9\n40\n25\n"
has "Same optimal cost"; r1=$?; [ $code -eq 0 ] && [ -f visualization.html ]; r2=$?
t TC-04 "three waypoints: succeeds, costs agree, HTML written" $((r1 + r2))

run "tests/disconnected.graph\n0\n3\n0\n"
has "No path from 0 to 3"; r1=$?; [ $code -ne 0 ]; r2=$?
t TC-05 "disconnected graph: reports no path, non-zero exit" $((r1 + r2))

run "tests/short_edges.graph\n0\n3\n0\n"
has "heuristic scaled"; r1=$?; has "Same optimal cost: 15.00"; r2=$?
t TC-06 "edges shorter than straight-line distance: heuristic scaled, still optimal (15.00)" $((r1 + r2))

run "tests/bad_format.graph\n"
has "Could not read a valid graph"; r1=$?; [ $code -ne 0 ]; r2=$?
t TC-07 "invalid file: error shown, then exits at end of input" $((r1 + r2))

run "tests/duplicate_id.graph\n"
has "Could not read a valid graph"; t TC-08 "duplicate node id in file is rejected" $?

run "$SAMPLE\n99\n0\n15\n0\n"
has "Invalid: enter a number from 0 to 15"; r1=$?; has "Same optimal cost"; r2=$?
t TC-09 "node id out of range: re-prompts, then continues" $((r1 + r2))

run "$SAMPLE\nabc\n0\n15\n0\n"
has "Please enter a whole number"; r1=$?; has "Same optimal cost"; r2=$?
t TC-10 "non-numeric input: re-prompts, then continues" $((r1 + r2))

bad=0
for i in $(seq 0 29); do
    s=$(( (i * 7) % 50 )); e=$(( (i * 13 + 5) % 50 ))
    run "$G50\n$s\n$e\n0\n"
    if has "WARNING" || [ $code -ne 0 ]; then bad=$((bad + 1)); echo "      mismatch for $s -> $e"; fi
done
t TC-11 "30 route pairs on graph50: A* and Dijkstra always find the same cost" $bad

run "$G50\n0\n49\n1\n24\n"
ok=0
grep -q '"steps"' visualization.html || ok=1
grep -q '<!--SCRIPT-->\|<!--STYLE-->' visualization.html && ok=1
if command -v node >/dev/null; then
    sed -n '/<script>/,/<\/script>/p' visualization.html | sed '1d;$d' > /tmp/pathfinder_check.js
    node --check /tmp/pathfinder_check.js || ok=1
fi
t TC-12 "visualization.html is complete and its script is valid" $ok

echo
echo "$pass passed, $fail failed"
[ $fail -eq 0 ]
