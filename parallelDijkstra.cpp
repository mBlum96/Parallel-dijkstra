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
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


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
                cout<<"Thread "<<threadId<<" unlocked mutex"<<endl;
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


int main() {
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  const char *hello = "Hello from server";

  // Create a server socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Set socket options
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);

  // Bind the socket to an address and port
  if (::bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // Start listening for connections
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  // Accept incoming connections
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  // Read data from the client
  valread = read(new_socket, buffer, 1024);
  printf("%s\n", buffer);

  // Parse the input to extract the parameters for the parallel Dijkstra algorithm
    int numVertices, numEdges, src, numThreads;
    sscanf(buffer, "%d %d %d %d", &numVertices, &numEdges, &src, &numThreads);
    printf("%d %d %d %d", numVertices, numEdges, src, numThreads);

    

  // Create a graph object
  Graph g(numVertices);
  // Add the edges to the graph
  //read from the client and add edges to the graph
    for (int i = 0; i < numEdges; i++) {
        int u, v, w;
        valread = read(new_socket, buffer, 1024);
        sscanf(buffer, "%d %d %d", &u, &v, &w);
        g.addEdge(u, v, w);
    }

  // Run the parallel Dijkstra algorithm
  parallelDijkstra(g, src, numThreads);
  // Send the result back to the client
  stringstream ss;
  for (int i = 0; i < g.V; i++) {
    ss << g.dist[i] << " ";
  }
  string result = ss.str();
  send(new_socket, result.c_str(), result.length(), 0);

  return 0;
}
