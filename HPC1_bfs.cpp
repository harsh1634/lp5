#include <iostream>
#include <omp.h>
using namespace std;

// Maximum number of vertices we support
const int MAXN = 100;

// Graph data structures (adjacency‑matrix representation)
int adj[MAXN][MAXN];      // adj[u][v] == 1 means edge between u and v
bool visited[MAXN];       // visited flag for each vertex
int parent[MAXN];         // to reconstruct BFS tree / path

// Arrays representing the current “frontier” of BFS and the next level
int frontier[MAXN], next_frontier[MAXN];

int main()
{
    int n, m;
    cout << "Enter number of vertices n and edges m:\n";
    cin >> n >> m;

    // --- Initialize graph and helper arrays ---
    for (int i = 0; i < n; i++)
    {
        visited[i] = false;   // no vertex visited yet
        parent[i]  = -1;      // no parent assigned
        for (int j = 0; j < n; j++)
            adj[i][j] = 0;    // clear all edges
    }

    // --- Read edges from user ---
    cout << "Enter edges (u v) 0-based:\n";
    for (int i = 0; i < m; i++)
    {
        int u, v;
        cin >> u >> v;
        // for undirected graph, mark both [u][v] and [v][u]
        adj[u][v] = adj[v][u] = 1;
    }

    // --- Read start and target vertices ---
    int start, target;
    cout << "Enter start vertex and target vertex:\n";
    cin >> start >> target;

    // --- Prepare BFS ---
    int front_size = 1;      // current frontier has 1 vertex
    int next_size  = 0;      // next frontier initially empty
    frontier[0]    = start;  // start BFS from this vertex
    visited[start] = true;   // mark start as visited
    parent[start]  = -1;     // root has no parent
    bool found     = (start == target);  // check trivial case

    // --- BFS main loop: process level by level ---
    while (front_size > 0 && !found)
    {
        next_size = 0;  // reset size of next frontier

        // Parallelize over all vertices in the current frontier
        #pragma omp parallel for shared(frontier, next_frontier, visited, parent, found)
        for (int i = 0; i < front_size; i++)
        {
            int u = frontier[i];         // current vertex to expand

            // Explore all possible neighbors v of u
            for (int v = 0; v < n; v++)
            {
                // If edge exists, v unvisited, and we haven’t found target yet
                if (!found && adj[u][v] && !visited[v])
                {
                    bool doAdd = false;

                    // Protect check‑and‑set of visited[v] and parent[v]
                    #pragma omp critical
                    {
                        if (!visited[v] && !found) {
                            visited[v] = true;  // claim v
                            parent[v]  = u;     // record BFS tree
                            doAdd     = true;   // we will add v to next frontier
                            if (v == target)
                                found = true;   // signal that we found the target
                        }
                    }

                    // If we successfully claimed v, append it to next_frontier
                    if (doAdd)
                    {
                        int idx;
                        // Atomically grab a unique index in next_frontier
                        #pragma omp atomic capture
                            idx = next_size++;
                        next_frontier[idx] = v;
                    }
                }
            }
        }

        // Move next_frontier into frontier for the next iteration
        for (int i = 0; i < next_size; i++)
            frontier[i] = next_frontier[i];
        front_size = next_size;
    }

    // --- After BFS, reconstruct and print path if found ---
    if (found)
    {
        int path[MAXN], len = 0;
        // Walk back from target to start via parent[] pointers
        for (int v = target; v != -1; v = parent[v])
            path[len++] = v;

        cout << "Found! Path: ";
        // Print in correct order: start → … → target
        for (int i = len - 1; i >= 0; i--) {
            cout << path[i];
            if (i > 0) cout << " -> ";
        }
        cout << "\n";
    }
    else {
        cout << "Target not reachable from start.\n";
    }

    return 0;
}
