#include "graph.h"

struct Pair
{
	int hID;
	int overlap;
	Pair(int _hID, int _overlap): hID(_hID), overlap(_overlap) {};
	bool operator< (const Pair &x) const
	{
		return hID < x.hID;
	}
};

struct QPair
{
	int vID;
	int overlap;
	QPair(int _vID, int _overlap): vID(_vID), overlap(_overlap) {};

};

struct Pair_in_queue {
	int hID;
	int overlap;
	Pair_in_queue(int _hID, int _overlap): hID(_hID), overlap(_overlap){};
	bool operator< (const Pair_in_queue &x) const
	{
		return overlap < x.overlap;
	}
};

struct nbr_struct {
	bool init = false;
	int max = 0;
	vector<pair<int, int>> left;
	vector<pair<int, int>> right;
};

class SL
{
private:

	int max_v, max_e;
	int n, m;

	bool directed;

	vector<Pair> *label, *tmpLabel, *ete_label;

	int *idx;
	int *order;
	priority_queue<Pair_in_queue> Q;
	vector<int> *E;
	HyperEdge *graph_edge;
	map<int, int> *vertex_map;
	map<int, int> *old_vertex_map;
	map<int, vector<int>> *reverse_compact;
	int * vertex_id;
	long long neighbour[10000000];
	int *visited;
	int *visited_h;

	int *global_visited_h;

	int *temp;

	long long total_memory;

	int total_count;

	struct nbr_struct* nbr;
	vector<pair<int, int>> *allNbr = nullptr;

	vector<QPair> * D;
	vector<QPair> * D_e;
	vector<QPair> * QPairs;
	int * C;

	set<int> * W;

	int *vmap_direct = nullptr;

public:

	SL(Graph *graph);
	~SL();

	void construct(string scalePath);
	void construct_for_a_vertex(HyperEdge * head, int u, bool update);
	void add_triplet(vector<Pair> *currLabel, int u, int h, int overlap, bool update);

	int reach(int src, int dst, bool original_id);

	int ete_edge_reach(int src, int dst);

	int baseLine(int src, int dst,  bool original_id);

	void prune_dom_of(int e);
	vector<pair<int,int>> pend_de_del;
	vector<int> rec_stamp;
	int rec_gen = 0;
	bool restore_dropped = true;
	vector<QPair> *dropped_by_hub = nullptr;
	void init_vmap_direct();

	vector<long long> flat_lab_off;
	vector<Pair> flat_lab;
	void build_flat();
	vector<int> soa_id, soa_w;
	void build_soa();
	vector<int> fused;
	void build_fused();
	int reach_fused(int src, int dst);

	bool insert_node(int selected_edge, int inserted_node);

};
