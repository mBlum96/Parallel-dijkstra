//This is a parallel dijkstra algorithm 

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <iomanip>
#include <limits>

using namespace std;
using namespace std::chrono;

class Edge
{
public:
	int dest;
	int weight;
	Edge(int d, int w) : dest(d), weight(w) {};
};

class Graph
{
public:
	int V;
	vector<vector<Edge> > adjList;
	vector<int> dist;
	vector<int> prev;

	Graph(int v) : V(v)
	{
		adjList.resize(v);
		dist.resize(v);
		prev.resize(v);
	}

	void addEdge(int src, int dest, int weight)
	{
		adjList[src].push_back(Edge(dest, weight));
	}

	void printGraph()
	{
		for (int i = 0; i < V; i++)
		{
			std::cout << i << ": ";
			for (auto edge : adjList[i])
			{
				std::cout << edge.dest << "(" << edge.weight << ") ";
			}
			std::cout << endl;
		}
	}

	void printDist()
	{
		for (int i = 0; i < V; i++)
		{
			std::cout << i << ": " << dist[i] << endl;
		}
	}

	void printPrev()
	{
		for (int i = 0; i < V; i++)
		{
			std::cout << i << ": " << prev[i] << endl;
		}
	}
};

class Compare
{
public:
	bool operator()(pair<int, int> p1, pair<int, int> p2)
	{
		return p1.second > p2.second;
	}
};

void dijkstra(Graph &g, int src)
{
	priority_queue<pair<int, int>, vector<pair<int, int> >, Compare> pq;
    //set all distances to infinity
    fill(g.dist.begin(), g.dist.end(), numeric_limits<int>::max());
	g.dist[src] = 0;
	pq.push(make_pair(src, 0));

	while (!pq.empty())
	{
		int u = pq.top().first;
		pq.pop();
		for (auto edge : g.adjList[u])
		{
			int v = edge.dest;
			int weight = edge.weight;
			if (g.dist[v] > g.dist[u] + weight)
			{
				g.dist[v] = g.dist[u] + weight;
				g.prev[v] = u;
				pq.push(make_pair(v, g.dist[v]));
			}
        }
    }
    
}

//parallelDijkstra
void parallelDijkstra(Graph &g, int src, int nThreads)
{
    //set all distances to infinity
    fill(g.dist.begin(), g.dist.end(), numeric_limits<int>::max());
    g.dist[src] = 0;
    //create priority queue and mutex
    priority_queue<pair<int, int>, vector<pair<int, int> >, Compare> pq;
    mutex pqMutex;
    //create threads
    vector<thread> threads;
    //create boolean array to check if vertex is in queue
    vector<bool> inQueue(g.V, false);
    //create thread function
    auto threadFunc = [&](int threadId)
    {
        while (true)
        {
            //lock mutex
            pqMutex.lock();
            //check if queue is empty
            if (pq.empty())
            {
                //unlock mutex
                pqMutex.unlock();
                break;
            }
            //get vertex with smallest distance
            int u = pq.top().first;
            pq.pop();
            //unlock mutex
            pqMutex.unlock();
            //relax edges (means try to update distance using the vertex with smallest distance)
            for (auto edge : g.adjList[u])
            {
                int v = edge.dest;
                int weight = edge.weight;
                //lock mutex
                pqMutex.lock();
                //check if distance is smaller
                if (g.dist[v] > g.dist[u] + weight)
                {
                    g.dist[v] = g.dist[u] + weight;
                    g.prev[v] = u;
                    //check if vertex is in queue
                    if (!inQueue[v])
                    {
                        //push vertex to queue
                        pq.push(make_pair(v, g.dist[v]));
                        inQueue[v] = true;
                    }
                }
                //unlock mutex
                pqMutex.unlock();
            }
        }
    };
    //create threads
    for (int i = 0; i < nThreads; i++)
    {
        threads.push_back(thread(threadFunc, i));
    }
    //push source vertex
    pq.push(make_pair(src, 0));
    inQueue[src] = true;
    //join threads
    for (auto &thread : threads)
    {
        thread.join();
    }
}

//main for 4 threads
//random graph with 10 vertices and 20 edges
//random source vertex
//random weights
int main(){
    srand(time(NULL));
    int V = 10;
    int E = 20;
    int src = rand() % V;
    Graph g(V);
    for (int i = 0; i < E; i++)
    {
        int src = rand() % V;
        int dest = rand() % V;
        int weight = rand() % 10;
        g.addEdge(src, dest, weight);
    }
    cout << "Graph:" << endl;
    g.printGraph();
    cout << endl;
    //count time for dijkstra
    auto start = chrono::high_resolution_clock::now();
    cout << "Dijkstra:" << endl;
    dijkstra(g, src);
    // g.printDist();
    //print distance from source vertex to all other vertices
    cout << "Distance from source vertex " << src << " to all other vertices:" << endl;
    for (int i = 0; i < V; i++)
    {
        cout << i << ": " << g.dist[i] << endl;
    }
    cout << endl;
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Time taken by Dijkstra: " << duration.count() << " microseconds" << endl;
    
    //count time for parallelDijkstra
    start = chrono::high_resolution_clock::now();
    cout << "Parallel Dijkstra:" << endl;
    parallelDijkstra(g, src, 4);
    g.printDist();
    cout << endl;
    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Time taken by Parallel Dijkstra: " << duration.count() << " microseconds" << endl;
    return 0;
}
