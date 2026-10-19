#include <cstdlib>
#include "graph.h"

Graph::Graph(char *graph_file, bool _directed, double _scale)
{
	max_v = 50000000;
	max_e = 100000000;

	n = 0;
	m = 0;
	max_size = 0;
	directed = _directed;

	graph_edge = new HyperEdge [max_e + 1];

	vertex_id = new int [max_v + 1];

	int u, v;
	int sign;

	ifstream fin(graph_file);
    string line;

	int num = 0;
	int count = 0;
	int maxESize = 0;
	vector<vector<int>> allData;
    while (getline(fin, line)) {

        istringstream iss(line);
        string word;
        vector<int> tokens;

        while (iss >> word) {
            int v = get_vertex_id(stoi(word));
            tokens.push_back(v);

        }

		if (tokens.size() == 1) {
			num++;
			continue;
		}

		int currSize = tokens.size();
		max_size = max(max_size, currSize);

		if (count % 1000 == 0) {
			cout << "input for line " << count << "\n";
		}
		count++;

		allData.push_back(tokens);

    }

	cout << "all Data size is " << allData.size() * _scale << " ====================================================\n";
	for (auto i = 0; i < allData.size() * _scale; i++) {
		insert_edge(allData[i]);
	}

	cout << "data reading is finished\n";
	cout << "total usefule edge num is " << m << "\n";
	cout << "edge num where size = 1 is :" << num << "\n";
	cout << "valid m = " << m << "\n";

	fin.close();
	old_vertex_map = vertex_map;
	int singleNode = 0;
	int effect = 0;
	bool no_compact = false;
	for (auto i = 1; !no_compact && i <= m; i++) {

		int head = -1;
		for (auto v : graph_edge[i].node) {
			if (E[v].size() == 1 ) {
				head = v;
				effect++;
				break;
			}
		}
		if (head == -1) continue;

		for (auto it = graph_edge[i].node.begin(); it != graph_edge[i].node.end();) {
			if (E[*it].size() == 1 && *it != head) {

				reverse_compact[head].push_back(*it);
				vertex_map[vertex_id[*it]] = vertex_map[vertex_id[head]];
				it = graph_edge[i].node.erase(it);

				singleNode++;
			} else {
				++it;
			}

		}

	}

	cout << "effect on " << effect << " edges\n";

	cout << "valid node num n = " << n - singleNode << "\n";
	cout << "single node number = " << singleNode << "\n";

	cout << "max E size is " << max_size << "\n";
	int maxDeg = 0;
	double avgDeg;
	for (auto i = 1; i <= n; i++) {
		int currSize = E[i].size();
		maxDeg = max(currSize, maxDeg);
		avgDeg += E[i].size();
	}
	avgDeg /= n;
	cout << "Max Degree is " << maxDeg << "\n";
	cout << "Avg Degree is " << avgDeg << "\n";

}

Graph::~Graph()
{
	if (graph_edge) delete[] graph_edge;

	if (vertex_id) delete[] vertex_id;

}

void Graph::insert_edge(vector<int> nodes)
{
	m++;

    for (auto v : nodes) {

		E[v].push_back(m);

        graph_edge[m].node.push_back(v);
	}

	graph_edge[m].length = nodes.size();

}

void Graph::delete_edge(int u, int v, int edge_id)
{

}

int Graph::get_vertex_id(int v)
{
	if (!vertex_map[v])
	{
		if (n < max_v)
		{
			vertex_map[v] = ++n;
			vertex_id[n] = v;
		}
		else
		{
			cout << "max_v is too small.\n";
			exit(0);
		}
	}
	return vertex_map[v];
}
