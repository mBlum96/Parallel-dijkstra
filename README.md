This is my final implementation of the parallel Dijkstra
this was written after the end of the Parallel programming class I took
because I didn't want to ditch the project unfinished and dicided to finish
it on my own after the class was done.

The parallel version works like this:
1. each thread pops the priority queue of the graph 
2. it then proceedes to relax the edges just like the regular dijkstra will
3. every time there is a change to the distance vector, it is done in an atomic fashion

This way each thread can take care of a different vertex that is discovered in the graph and is added to the priority queue instead of a single thread that is doing so.

As is expected, the time that it takes a single thread to perform dijkstra is a lot faster then multithreaded when the number of vertecis and edges is small but when we consider larger graphs of 1000s and more vertices and edges, the parallel implemintation provides superior performance.
