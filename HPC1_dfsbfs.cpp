#include <iostream>
#include <queue>
#include <stack>
#include <omp.h>

using namespace std;

class graph
{
public:
	int graphAdjacencyMatrix[20][20] = {0}; // in cpp dynamic allocation of array size is not allowed

	void addEdge(int i, int j)
	{
		graphAdjacencyMatrix[i][j] = 1;
		graphAdjacencyMatrix[j][i] = 1;
	}

	void parallelBFS(int startNode, int n)
	{
		queue<int> q;
		bool visited[20] = {false};

		q.push(startNode);
		visited[startNode] = true;
		cout << startNode << " ";

		while (!q.empty())
		{
			int temp;

#pragma omp critical
			{
				temp = q.front();
				q.pop();
			}

#pragma omp parallel for
			for (int i = 0; i < n; i++)
			{
				if (graphAdjacencyMatrix[temp][i] == 1 && visited[i] == false)
				{
#pragma omp critical
					{
						visited[i] = true;
						cout << i << " ";
						q.push(i);
					}
				}
			}
		}
		cout << endl;
	}

	void parallelBFSSearch(int startNode, int targetNode, int n)
	{
		queue<int> q;
		bool visited[20] = {false};
		int parent[20];

		// initialize parent[] to -1 (no parent)
		for (int i = 0; i < n; i++)
			parent[i] = -1;

		// start BFS from startNode
		q.push(startNode);
		visited[startNode] = true;
		parent[startNode] = -1;

		bool found = (startNode == targetNode);

		// BFS loop: level by level
		while (!q.empty() && !found)
		{
			int u;
#pragma omp critical
			{
				u = q.front();
				q.pop();
			}

// explore all neighbors of u in parallel
#pragma omp parallel for shared(found)
			for (int v = 0; v < n; v++)
			{
				// if edge u→v, not yet visited, and we haven't found target
				if (!found && graphAdjacencyMatrix[u][v] == 1 && !visited[v])
				{
					bool doPush = false;
#pragma omp critical
					{
						// re‑check inside critical
						if (!visited[v] && !found)
						{
							visited[v] = true; // claim v
							parent[v] = u;	   // record path
							doPush = true;
							if (v == targetNode)
								found = true; // signal that we found it
						}
					}
					if (doPush)
					{
#pragma omp critical
						q.push(v);
					}
				}
			}
		}

		// after BFS, either found==true or queue empty
		if (found)
		{
			// reconstruct path from target back to start
			int path[20], len = 0;
			for (int v = targetNode; v != -1; v = parent[v])
				path[len++] = v;

			// print path in correct order
			cout << "Path: ";
			for (int i = len - 1; i >= 0; i--)
			{
				cout << path[i];
				if (i > 0)
					cout << " -> ";
			}
			cout << "\n";
		}
		else
		{
			cout << "Target " << targetNode << " not reachable from " << startNode << "\n";
		}
	}

	void parallelDFS(int startNode, int n)
	{
		stack<int> st;
		bool visited[20] = {false};

		st.push(startNode);
		visited[startNode] = true;

		while (!st.empty())
		{
			int temp;

#pragma omp critical
			{
				temp = st.top();
				st.pop();
				cout << temp << " ";
			}

#pragma omp parallel for
			for (int i = 0; i < n; i++)
			{
				if (graphAdjacencyMatrix[temp][i] == 1 && visited[i] == false)
				{
#pragma omp critical
					{
						visited[i] = true;
						st.push(i);
					}
				}
			}
		}
		cout << endl;
	}

	void parallelDFSSearch(int startNode, int targetNode, int n)
	{
		stack<int> st;
		bool visited[20] = {false};
		int parent[20];
		bool found = false;

		// initialize parent pointers
		for (int i = 0; i < n; i++)
			parent[i] = -1;

		// start from the root
		st.push(startNode);
		visited[startNode] = true;

		// DFS loop
		while (!st.empty() && !found)
		{
			int u;
#pragma omp critical
			{
				u = st.top();
				st.pop();
				// check immediately if this is the target
				if (u == targetNode)
				{
					found = true;
				}
			}
			if (found)
				break;

// explore neighbors in parallel
#pragma omp parallel for shared(found)
			for (int v = 0; v < n; v++)
			{
				if (!found && graphAdjacencyMatrix[u][v] == 1 && !visited[v])
				{
					bool doPush = false;
#pragma omp critical
					{
						if (!visited[v] && !found)
						{
							visited[v] = true;
							parent[v] = u; // record how we reached v
							doPush = true;
							if (v == targetNode)
								found = true; // signal success
						}
					}
					if (doPush)
					{
#pragma omp critical
						st.push(v);
					}
				}
			}
		}

		// reconstruct path if found
		if (found)
		{
			int path[20], len = 0;
			for (int v = targetNode; v != -1; v = parent[v])
				path[len++] = v;

			cout << "Path: ";
			for (int i = len - 1; i >= 0; i--)
			{
				cout << path[i] << (i ? " -> " : "\n");
			}
		}
		else
		{
			cout << "Target " << targetNode
				 << " not reachable from " << startNode << "\n";
		}
	}
};

int main()
{
	graph g;

	cout << "Enter the number of nodes in graph:";
	int nodes;
	cin >> nodes;

	cout << "Enter the number of edges in graph:";
	int edges;
	cin >> edges;

	cout << "Enter the edges in form of (u,v)";
	int u, v;
	for (int i = 0; i < edges; i++)
	{
		cin >> u >> v;
		g.addEdge(u, v);
	}

	// printing the adjacency matrix
	for (int i = 0; i < nodes; i++)
	{
		for (int j = 0; j < nodes; j++)
		{
			cout << g.graphAdjacencyMatrix[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;

	int start, target;
	cout << "Enter the start node and target node: " << endl;
	cin >> start >> target;
	
	cout << "BFS traversal is: " << endl;
	double BFSstart = omp_get_wtime();
	g.parallelBFS(start, nodes);
	double BFSend = omp_get_wtime();
	cout << "Time taken by parallel BFS:" << BFSend - BFSstart << " seconds." << endl;

	cout << "DFS traversal is: ";
	double DFSstart = omp_get_wtime();
	g.parallelDFS(0, nodes);
	double DFSend = omp_get_wtime();
	cout << "Time taken by parallel DFS:" << DFSend - DFSstart << " seconds" << endl;

	cout << "BFS: ";
	double BFS_start = omp_get_wtime();
	g.parallelBFSSearch(start, target, nodes);
	double BFS_end = omp_get_wtime();
	cout << "Time taken by parallel BFS:" << BFS_end - BFS_start << " seconds." << endl;

	cout << "DFS: ";
	double DFS_start = omp_get_wtime();
	g.parallelDFSSearch(start, target, nodes);
	double DFS_end = omp_get_wtime();
	cout << "Time taken by parallel DFS:" << DFS_end - DFS_start << " seconds." << endl;

	return 0;
}
