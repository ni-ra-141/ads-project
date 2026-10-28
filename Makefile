CXX = g++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra
OBJS = main.o graph.o astar.o dijkstra.o export.o

pathfinder: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp graph.h astar.h dijkstra.h export.h
	$(CXX) $(CXXFLAGS) -c $<

test: pathfinder
	bash tests/run_tests.sh

clean:
	rm -f $(OBJS) pathfinder visualization.html

.PHONY: test clean
