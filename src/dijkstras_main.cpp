#include "dijkstras.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // Default to small.txt if no input file provided
    string input_file = "small.txt";
    
    // Check for command line arguments
    if (argc > 1) {
        input_file = argv[1];
    }
    
    try {
        // Load graph from file
        Graph G;
        file_to_graph(input_file, G);
        
        cout << "Running Dijkstra's algorithm on " << input_file << endl;
        cout << "Graph has " << G.numVertices << " vertices" << endl;
        
        // Set source vertex (0 by default)
        int source = 0;
        
        // Run Dijkstra's algorithm
        vector<int> previous;
        vector<int> distances = dijkstra_shortest_path(G, source, previous);
        
        // Print paths and costs from source to all vertices
        cout << "Shortest paths from vertex " << source << ":" << endl;
        for (int i = 0; i < G.numVertices; i++) {
            if (i == source) {
                continue;  // Skip the source vertex itself
            }
            
            vector<int> path = extract_shortest_path(distances, previous, i);
            if (!path.empty()) {
                cout << "To vertex " << i << ": ";
                print_path(path, distances[i]);
            } else {
                cout << "No path to vertex " << i << endl;
            }
        }
        
        // Print total cost to each vertex
        cout << "\nTotal costs from vertex " << source << ":" << endl;
        for (int i = 0; i < G.numVertices; i++) {
            if (distances[i] == INF) {
                cout << "Vertex " << i << ": Unreachable" << endl;
            } else {
                cout << "Vertex " << i << ": " << distances[i] << endl;
            }
        }
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
