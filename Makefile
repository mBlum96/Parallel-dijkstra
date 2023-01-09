CXX = g++
CFLAGS = -pthread -std=c++20 -Wall -O3 -g

SOURCES = parallelDijkstra.cpp
OBJECTS = parallelDijkstra.o

parallelDijkstra: $(OBJECTS)
	$(CXX) $(CFLAGS) -o parallelDijkstra $(OBJECTS)

parallelDijkstra.o: parallelDijkstra.cpp
	$(CXX) -c $(CFLAGS) -o parallelDijkstra.o parallelDijkstra.cpp

clean:
	rm -f *.o
	rm -f parallelDijkstra