#include <iostream>
#include <omp.h>
using namespace std;

const int MAXN = 100;
int adj[MAXN][MAXN];
bool visited[MAXN];
int parent[MAXN];
int target;                // the vertex we’re searching for
volatile bool found = false;  // flag set when target is discovered

// Parallel DFS‑search using OpenMP tasks
void dfs_search(int u, int n) {
    // If another task already found the target, bail out
    bool f;
    #pragma omp atomic read
    f = found;
    if (f) return;

    // Process this node
    if (u == target) {
        #pragma omp critical
        {
            if (!found) {
                found = true;
                // we will reconstruct path later in main()
            }
        }
        return;               // no need to go further from target
    }

    // For each neighbor v, if unvisited, spawn a task
    for (int v = 0; v < n; v++) {
        bool doTask = false;
        #pragma omp critical
        {
            if (!found && adj[u][v] && !visited[v]) {
                visited[v] = true;
                parent[v] = u;  // record path
                doTask = true;
            }
        }
        if (doTask) {
            #pragma omp task firstprivate(v)
            dfs_search(v, n);
        }
    }

    // Wait for all child‑tasks of this call
    #pragma omp taskwait
}

int main(){
    int n, m;
    cout << "Enter number of vertices n and edges m:\n";
    cin >> n >> m;

    // read edges
    for(int i=0;i<n;i++){
        visited[i] = false;
        parent[i] = -1;
        for(int j=0;j<n;j++)
            adj[i][j] = 0;
    }
    cout << "Enter edges (u v) 0-based:\n";
    for(int i=0;i<m;i++){
        int u,v; 
        cin >> u >> v;
        adj[u][v] = adj[v][u] = 1;
    }

    cout << "Enter start vertex and target vertex:\n";
    int start;
    cin >> start >> target;

    // mark start visited and parent
    visited[start] = true;
    parent[start] = -1;

    // launch the parallel search
    #pragma omp parallel
    {
        #pragma omp single
        dfs_search(start, n);
    }

    if (found) {
        // reconstruct path from start→...→target
        int path[MAXN], len = 0;
        for(int v = target; v != -1; v = parent[v])
            path[len++] = v;
        cout << "Found! Path: ";
        for(int i = len-1; i >= 0; i--)
            cout << path[i] << (i? " -> ":"\n");
    } else {
        cout << "Target not reachable from start.\n";
    }

    return 0;
}
