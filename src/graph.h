#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <ctime>
#include <cfloat>
#include <numeric>
#include <climits>
#include <functional>
#include <chrono>
#include <time.h>
#include <filesystem>
#include <cmath>
#include <unordered_set>
using namespace std;

struct HyperEdge
{
	vector<int> node;
    int length;
	HyperEdge *last, *next;
};

class Graph
{
public:

	int max_v, max_e;
	int n, m, max_size;

	bool directed;
	double scale;

	HyperEdge *graph_edge;

    vector<int> E[20000000];

	int *vertex_id;
	map<int, int> vertex_map, old_vertex_map;

	map<int, vector<int>> reverse_compact;

	Graph(char *graph_file, bool directed, double scale);
	~Graph();

	void insert_edge(vector<int> nodes);

	void delete_edge(int u, int v, int edge_id);

	int get_vertex_id(int v);
};
