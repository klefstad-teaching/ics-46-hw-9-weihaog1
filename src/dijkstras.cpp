#include "dijkstras.h"
#include <algorithm>  // Add this line for reverse()

struct Node {
    int vertex;
    int distance;
    
    Node(int v, int d) : vertex(v), distance(d) {}
    
    // Define comparison for priority queue (min heap)
    bool operator>(const Node& other) const {
        return distance > other.distance;
    }
};

// Implementation of Dijkstra's algorithm
vector<int> dijkstra_shortest_path(const Graph& G, int source, vector<int>& previous) {
    int n = G.numVertices;
    vector<int> distance(n, INF);
    vector<bool> visited(n, false);
    previous.resize(n, -1);
    
    // Priority queue (min heap) for Dijkstra's algorithm
    priority_queue<Node, vector<Node>, greater<Node>> pq;
    
    // Initialize source vertex
    distance[source] = 0;
    pq.push(Node(source, 0));
    
    while (!pq.empty()) {
        // Get vertex with minimum distance
        int u = pq.top().vertex;
        pq.pop();
        
        // Skip if already processed
        if (visited[u]) {
            continue;
        }
        
        visited[u] = true;
        
        // Process all adjacent vertices
        for (const Edge& edge : G[u]) {
            int v = edge.dst;
            int weight = edge.weight;
            
            // Relaxation step
            if (!visited[v] && distance[u] != INF && distance[u] + weight < distance[v]) {
                distance[v] = distance[u] + weight;
                previous[v] = u;
                pq.push(Node(v, distance[v]));
            }
        }
    }
    
    return distance;
}

// Extract the shortest path from source to destination
vector<int> extract_shortest_path(const vector<int>& distances, const vector<int>& previous, int destination) {
    vector<int> path;
    
    // Check if there's a path
    if (distances[destination] == INF) {
        return path;  // Empty path if no path exists
    }
    
    // Reconstruct the path from destination to source
    for (int at = destination; at != -1; at = previous[at]) {
        path.push_back(at);
    }
    
    // Reverse the path to get source to destination
    reverse(path.begin(), path.end());
    
    return path;
}

// Print the path and total cost
void print_path(const vector<int>& path, int total) {
    if (path.empty()) {
        cout << "\nTotal cost is " << total << endl;
        return;
    }
    
    // Print path vertices with spaces
    for (size_t i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i < path.size() - 1) {
            cout << " ";
        }
    }
    cout << " \nTotal cost is " << total << endl;
}
