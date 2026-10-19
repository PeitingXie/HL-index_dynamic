#include "HI.h"
#include <cassert>

SL::SL(Graph *graph)
{
	max_v = graph -> max_v;
	max_e = graph -> max_e;
	n = graph -> n;
	m = graph -> m;
	directed = graph -> directed;

	E = graph->E;
	graph_edge = graph->graph_edge;

	vertex_map = &(graph -> vertex_map);
	old_vertex_map = &(graph -> old_vertex_map);
	vertex_id = graph->vertex_id;
	reverse_compact = &(graph->reverse_compact);

	total_memory = 0;

	label = new vector<Pair> [n + 1];
	ete_label = new vector<Pair> [m + 1];
	tmpLabel = new vector<Pair> [n + 1];
	visited = new int [n + 1];
	visited_h = new int [m + 1];
	fill(visited_h, visited_h + m + 1, 0);

	global_visited_h = new int [m+1];
	fill(global_visited_h, global_visited_h + m + 1, 0);

	fill(neighbour, neighbour + m + 1, 0LL);
	for (auto i = 1; i <= m; i++) {
		for (auto v: graph_edge[i].node) {

			neighbour[i] += E[v].size();
		}
	}

	idx = new int [m + 1];

	iota(idx + 1, idx + m + 1, 1);
	sort(idx + 1, idx + m + 1, [&](int i, int j){return (long long)(neighbour[i] + 1) > (long long)(neighbour[j] + 1);});

	order = new int [m + 1];
	iota(order + 1, order + m + 1, 1);
	for (int i = 1; i <= m; i++)
		order[idx[i]] = i;

	nbr = new (std::nothrow) nbr_struct[m+1];

	D = new vector<QPair>[m + 1];
	D_e = new vector<QPair>[m + 1];
	QPairs = new vector<QPair>[m + 1];
	C = new int [m + 1];

	W = new set<int> [n + 1];
	total_count = 0;

	for (auto i = 1; i <= m; i++) {
		graph_edge[i].node.push_back(n + i);
	}

}

SL::~SL()
{

	if (idx) delete[] idx;
	if (order) delete[] order;
	if (nbr) delete[] nbr;
	if (visited_h) delete[] visited_h;
	if (global_visited_h) delete[] global_visited_h;
	if (visited) delete[] visited;

	if (allNbr) delete[] allNbr;
	if (D) delete[] D;
	if (D_e) delete[] D_e;
	if (QPairs) delete[] QPairs;
	if (tmpLabel) delete[] tmpLabel;
	if (C) delete[] C;
	if (W) delete[] W;

}

void SL::add_triplet(vector<Pair> *currLabel, int u, int h, int overlap, bool update)
{

	if (currLabel != ete_label) {
		auto it = std::upper_bound(
			QPairs[h].begin(),
			QPairs[h].end(),
			QPair(u, overlap),
			[](const QPair& a, const QPair& b) {
				return a.overlap > b.overlap;
			}
		);
		QPairs[h].insert(it, QPair(u, overlap));
	} else {

		auto it = std::lower_bound(
			D_e[h].begin(),
			D_e[h].end(),
			u,
			[](const QPair& p, int id) { return p.vID < id; }
		);
		if (it != D_e[h].end() && it->vID == u) {
			if (it->overlap < overlap) it->overlap = overlap;
		} else {
			D_e[h].insert(it, QPair(u, overlap));
		}
	}

	if (!currLabel[u].size()) {
		currLabel[u].push_back(Pair(h, overlap));
	} else {

		Pair temp(h, overlap);

		auto it = std::lower_bound(currLabel[u].begin(), currLabel[u].end(), temp,
        [](const Pair a, const Pair& b) {
            return a.hID < b.hID;
        });

		if (it == currLabel[u].end() || it->hID != h) {

			currLabel[u].insert(it, temp);
		}

	}
}

void SL::construct_for_a_vertex(HyperEdge * head,  int u, bool update) {
	int count = 0;
	int num = 0;
	ofstream myfile;
  	myfile.open ("out.txt");
	int max_h = 0;
	long long total = 0;
	int execute = 0;

	while (!Q.empty()) {
		Pair_in_queue pair = Q.top();
		Q.pop();
		int h = pair.hID;
		int overlap = pair.overlap;
		execute ++;

		if (overlap > nbr[h].max) {

			add_triplet(ete_label, h, u, overlap, true);

			for (auto v : graph_edge[idx[h]].node) {
				if (v > n) {
					continue;

				} else {
					if (visited[v] == 0) {
						visited[v] = overlap;
						add_triplet(tmpLabel, v, u, overlap, update);
					}
				}
			}
		} else {
			continue;
		}
		if (!nbr[h].init) {

			nbr[h].init = 1;
			nbr[h].max = overlap;
			map<int, int> m;

			for (auto v : graph_edge[idx[h]].node) {

				if (v > n) continue;
				for (auto nextH : E[v]) {
					if (order[nextH] <= u || order[nextH] == h) continue;

					if (m.find(order[nextH]) != m.end()) {
						m[order[nextH]]++;
					} else {
						m.insert(make_pair(order[nextH], 1));
					}
				}

			}

			for (auto it = m.begin(); it != m.end(); it++) {

				Q.push(Pair_in_queue(it->first, min(overlap, it->second)));
				if (it->second > nbr[h].max && h > 1) {

					nbr[h].left.emplace_back(make_pair(it->first, it->second));
				}
			}
			m.clear();

		} else if (overlap <= nbr[h].max) {
		} else {
			nbr[h].max = overlap;
			for (auto it1 = nbr[h].left.begin(); it1 != nbr[h].left.end(); ) {
				if (it1->first > u) {
					Q.push(Pair_in_queue(it1->first, min(overlap, it1->second)));
				}
				if (it1->second <= nbr[h].max) {
					it1 = nbr[h].left.erase(it1);
				} else {
					++it1;
				}
			}
		}

	}
	myfile.close();
}

void SL::construct(string scalePath) {

	cout << "ready to construct\n";

	double avg = 0;
	for (auto i = 1; i <= n; i++) {
		avg += E[i].size();
	}
	avg /= n;
	cout << "average neighbour = " << avg << "\n";

	vector<vector<int>> hot;
	vector<int> centrality;
	for (auto i = 1; i <= n; i++) {
		centrality.push_back(E[i].size());
	}
	sort(centrality.begin(), centrality.end());
	double level = 0.85;
	int count = 0;
	double val = level + count * 0.03;
	while (val < 1) {
		cout << "at level " << val << " " << centrality[n * val] << "\n";
		count += 1;
		val = level + count * 0.03;

	}

	int singleNode = 0;

	cout << "total n = " << n << ", single node = " << singleNode << "\n";

	ofstream myConstructionfile;
	myConstructionfile.open ("construction.txt");
	cout << "start construction\n";

	auto start_time = std::chrono::high_resolution_clock::now();
	for (auto i = 1; i <= m; i++) {

		Q.push(Pair_in_queue(i, graph_edge[idx[i]].length));

		fill(visited, visited + n + 1, 0);
		fill(visited_h, visited_h + m + 1, 0);
		construct_for_a_vertex(graph_edge, i, false);

		if (i % 1000 == 1) {
		cout << "Edge  " << i << " has ben constructed " << "\n";
		}
		std::priority_queue<Pair_in_queue>().swap(Q);

	}

	auto end_time = std::chrono::high_resolution_clock::now();
	auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	myConstructionfile.close();

	std::ofstream file(scalePath, std::ios::app);
	if (!file.is_open()) {
        std::cerr << "failed to open file: " << scalePath << std::endl;
        return;
    }

	file << "construct time is " << elapsed_time.count() << "\n";

	for (auto i = 1; i<=m; i++) {
		graph_edge[i].node.pop_back();
		graph_edge[i].node.shrink_to_fit();
	}
	cout << "construction finished\n";

	cout << "construct time is " << elapsed_time.count() << "\n";
	int c;

	double vte = 0;

	cout << "here\n";
	double ete = 0;

	for (auto i = 1; i <=n; i++) {
		vte += tmpLabel[i].size();
	}
	cout << "tmp total vte = " << vte << "\n";
	file << "tmp total vte = " << vte << "\n";
	vte /= n;

	for (auto i = 1; i <= m; i++) {
		ete += ete_label[i].size();
	}

	cout << "tmp average vte = " << vte << "\n";

	long nonMinimaltotalBytes = 0;
	for (int i = 1; i < n + 1; ++i) {
		nonMinimaltotalBytes += sizeof(Pair) * tmpLabel[i].size();
	}

	if (restore_dropped && !dropped_by_hub) dropped_by_hub = new vector<QPair>[m + 2];
	bool no_minimize = false;

	int sumNum = 0;
	int* vis = new int[n + 1]();

	start_time = std::chrono::high_resolution_clock::now();
	for (auto i = 1; i <= m; ++i) {

		if (QPairs[i].size() == 0) continue;
		if (QPairs[i].size() == 1) {
			bool re = false;
			for (auto it = tmpLabel[QPairs[i][0].vID].begin(); it != tmpLabel[QPairs[i][0].vID].end(); ++it) {
				if (it->hID != i && it->overlap >= QPairs[i][0].overlap) {
					re = no_minimize ? false : true;
					break;
				}
				if (re) {
					auto it2 = std::lower_bound(tmpLabel[QPairs[i][0].vID].begin(), tmpLabel[QPairs[i][0].vID].end(), Pair(i, 1),
					[](const Pair a, const Pair& b) {
						return a.hID < b.hID;
					});
					if (it2 != tmpLabel[QPairs[i][0].vID].end() && it2->hID == i) {
						if (restore_dropped)
							dropped_by_hub[i].push_back(QPair(QPairs[i][0].vID, QPairs[i][0].overlap));
						tmpLabel[QPairs[i][0].vID].erase(it2);
					}
				} else {
					auto temp = Pair(i, QPairs[i][0].overlap);
					auto it_1 = std::lower_bound(label[QPairs[i][0].vID].begin(), label[QPairs[i][0].vID].end(), temp,
					[](const Pair a, const Pair& b) {
						return a.hID < b.hID;
					});
					label[QPairs[i][0].vID].insert(it_1, temp);
				}
			}
			continue;
		}
		unordered_set<int> nonDup;

		std::map<int, std::vector<std::pair<int, int>>> inverted;

		for (auto it = QPairs[i].begin(); it != QPairs[i].end(); ++it) {
			for (auto p = tmpLabel[it->vID].begin(); p != tmpLabel[it->vID].end(); ++p) {
				if (p->overlap >= it->overlap  && p->hID != i) {
					inverted[p->hID].push_back(make_pair(it->vID, it->overlap));
				}
			}
		}

		for (auto it = QPairs[i].begin(); it != QPairs[i].end();) {

			unordered_set<int> candidates;
			bool redundant = false;
			for (auto p = tmpLabel[it->vID].begin(); p != tmpLabel[it->vID].end(); ++p) {
				if (redundant) break;
				if (p->hID == i) continue;

				for (auto p_1 = inverted[p->hID].begin(); p_1 != inverted[p->hID].end(); ++p_1) {
					if (redundant) break;

					if (p->overlap >= p_1->second && vis[p_1->first] != i) {
						candidates.insert(p_1->first);
					}

					if (candidates.size() == QPairs[i].size()) redundant = true;
				}
			}

			auto found = nonDup.find(it->vID);

			if (no_minimize) redundant = false;
			if (!redundant || found != nonDup.end()) {

				auto temp = Pair(i, it->overlap);
				auto it_1 = std::lower_bound(label[it->vID].begin(), label[it->vID].end(), temp,
				[](const Pair a, const Pair& b) {
				    return a.hID < b.hID;
				});

				label[it->vID].insert(it_1, temp);
				for (auto it2 = QPairs[i].begin(); it2 != QPairs[i].end(); ++it2) {
					if (candidates.find(it2->vID) == candidates.end()) {
						nonDup.insert(it2->vID);
					}
				}

			} else {

				auto it2 = std::lower_bound(tmpLabel[it->vID].begin(), tmpLabel[it->vID].end(), Pair(i, 1),
				[](const Pair a, const Pair& b) {
				    return a.hID < b.hID;
				});
				if (it2 != tmpLabel[it->vID].end() && it2->hID == i) {
					if (restore_dropped)
						dropped_by_hub[i].push_back(QPair(it->vID, it->overlap));
					tmpLabel[it->vID].erase(it2);
				}

			}

			nonDup.erase(it->vID);

			vis[it->vID] = i;
			it = QPairs[i].erase(it);
			if (nonDup.size() == QPairs[i].size()) {

				for(; it != QPairs[i].end(); ++it) {
					auto temp = Pair(i, it->overlap);
					auto it_1 = std::lower_bound(label[it->vID].begin(), label[it->vID].end(), temp,
					[](const Pair a, const Pair& b) {
						return a.hID < b.hID;
					});
					label[it->vID].insert(it_1, temp);
				}

				break;
			}
		}

	}

	end_time = std::chrono::high_resolution_clock::now();
	elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	cout << "minimal time is " << elapsed_time.count() << "\n";
	file << "minimal time is " << elapsed_time.count() << "\n";
	cout << "sumNum = " << sumNum << "\n";

	delete[] vis;

	vte = 0;
	ete = 0;

	for (auto i = 0; i <= n; i++) {

		for (auto p : label[i]) {

		}
		vte += label[i].size();
	}

	cout << "total minimal vte = " << vte << "\n";
	file << "total minimal vte = " << vte << "\n";

	vte /= n;

	for (auto i = 1; i <= m; i++) {
		ete += ete_label[i].size();
	}
	ete /= m;
	cout << "average vte = " << vte << "\n";
	cout << "average ete = " << ete << "\n";

	long totalBytes = 0;
	for (int i = 1; i < n + 1; ++i) {
		totalBytes += sizeof(Pair) * label[i].size();
	}

	file << "VTE index size is " << totalBytes / 1024 << "kb\n";
	file << "VTE Container size is " << sizeof(vector<Pair>) * n /1024 << "kb\n";

	cout << "Minimal/non Minimal VTE index size is " << totalBytes / 1024 << ", "<< nonMinimaltotalBytes / 1024 << "kb\n";
	file << "Minimal/non Minimal VTE index size is " << totalBytes / 1024 << ", "<< nonMinimaltotalBytes / 1024 << "kb\n";

	long eteBytes = 0;
	for (int i = 1; i < m + 1; ++i) {
		eteBytes += sizeof(Pair) * ete_label[i].size();
	}
	cout << "ETE index size is " << eteBytes / 1024 << "kb\n";
	cout << "ETE Container size is " << sizeof(vector<Pair>) * m /1024 << "kb\n";

	long graphSize = 0;

	for (auto i = 1; i <= n; i++) {
		graphSize += E[i].size() * sizeof(int);
	}
	cout << "Graph size is " << graphSize /1024 << "kb\n";
	file << "Graph size is " << graphSize /1024 << "kb\n";

	cout << "construction finished\n";

}



static std::vector<int> ins_mark;
static int ins_gen = 0;
static std::vector<int> ins_xw;
static std::vector<Pair> ins_x_old;

bool SL::insert_node(int selected_edge, int inserted_node) {
	ins_x_old = label[(*vertex_map)[vertex_id[inserted_node]]];

    std::vector<int>::iterator it;
    {

		const int mapped_node = (*vertex_map)[vertex_id[inserted_node]];
		for (it = graph_edge[selected_edge].node.begin(); it != graph_edge[selected_edge].node.end(); ++it) {
			if (*it == mapped_node) return false;
		}

    }

    {

        fill(visited_h, visited_h + m + 1, 0);
        if (E[inserted_node].size() == 1) {

			if (inserted_node == (*vertex_map)[vertex_id[inserted_node]]) {

				int head = -1;

				for (auto it_r = (*reverse_compact)[inserted_node].begin(); it_r != (*reverse_compact)[inserted_node].end(); ) {
					if (head == -1) {
						head = *it_r;

						(*vertex_map)[vertex_id[*it_r]] = *it_r;

						label[*it_r] = label[(*vertex_map)[vertex_id[inserted_node]]];
						graph_edge[*(E[inserted_node].begin())].node.push_back(head);

						it_r = (*reverse_compact)[inserted_node].erase(it_r);

					} else {

						(*vertex_map)[vertex_id[*it_r]] = (*vertex_map)[vertex_id[head]];
						++it_r;
					}
				}
				(*reverse_compact)[head] = (*reverse_compact)[inserted_node];
			} else {
				label[inserted_node] = label[(*vertex_map)[vertex_id[inserted_node]]];

				for (auto it_r = (*reverse_compact)[(*vertex_map)[vertex_id[inserted_node]]].begin(); it_r != (*reverse_compact)[(*vertex_map)[vertex_id[inserted_node]]].end();) {
					if (*it_r == inserted_node) {
						it_r = (*reverse_compact)[(*vertex_map)[vertex_id[inserted_node]]].erase(it_r);
					} else {
						it_r++;
					}
				}

				(*vertex_map)[vertex_id[inserted_node]] = (*old_vertex_map)[vertex_id[inserted_node]];

				bool exist = false;
				for (auto in_it = graph_edge[*(E[inserted_node].begin())].node.begin(); in_it != graph_edge[*(E[inserted_node].begin())].node.end(); ++in_it) {
					if (*in_it == inserted_node) exist = true;
				}
				if (!exist) {
					graph_edge[*(E[inserted_node].begin())].node.push_back(inserted_node);
				}

			}

        }

    }

	graph_edge[selected_edge].length++;

    {

		graph_edge[selected_edge].node.push_back(inserted_node);

        it = std::lower_bound(E[inserted_node].begin(), E[inserted_node].end(), selected_edge);
        E[inserted_node].insert(it, selected_edge);
    }

	for (auto it = ete_label[order[selected_edge]].begin(); it != ete_label[order[selected_edge]].end(); ++it) {
		auto it2 = std::lower_bound(
			label[inserted_node].begin(), label[inserted_node].end(), it->hID,
			[](const Pair& p, int id){ return p.hID < id; }
		);
		if (it2 == label[inserted_node].end() ||  it2->hID != it->hID) {
			label[inserted_node].insert(it2, Pair(it->hID, it->overlap));
		} else {
			it2->overlap = max(it2->overlap, it->overlap);
		}
	}

	vector<pair<int,Pair>> triplets;
	{
		int re = order[selected_edge];
		auto set_entry = [&](int z, int t, int k) {
			auto it3 = std::lower_bound(ete_label[z].begin(), ete_label[z].end(), t,
				[](const Pair& p, int id){ return p.hID < id; });
			if (it3 != ete_label[z].end() && it3->hID == t) {
				if (it3->overlap >= k) return;
				it3->overlap = k;
			} else {
				ete_label[z].insert(it3, Pair(t, k));
			}
			auto &DD = D_e[t];
			auto ip = std::lower_bound(DD.begin(), DD.end(), z,
				[](const QPair& p, int id){ return p.vID < id; });
			if (ip != DD.end() && ip->vID == z) {
				if (ip->overlap < k) ip->overlap = k;
			} else {
				DD.insert(ip, QPair(z, k));
			}
			triplets.push_back(make_pair(z, Pair(t, k)));
		};
		auto top_at = [&](int f, int k) {
			for (auto &p : ete_label[f]) if (p.overlap >= k) return p.hID;
			return -1;
		};
		set_entry(re, re, graph_edge[selected_edge].length);
		if ((int)ins_mark.size() < n + 2) ins_mark.assign(n + 2, 0);
		vector<int> xedges = E[inserted_node];
		for (int c : xedges) {
			if (c == selected_edge) continue;
			++ins_gen;
			int k = 0;
			for (int v : graph_edge[selected_edge].node) ins_mark[v] = ins_gen;
			for (int v : graph_edge[c].node) if (ins_mark[v] == ins_gen) k++;
			if (k == 0) continue;
			int tA = top_at(re, k), tB = top_at(order[c], k);
			if (tA < 0 || tB < 0 || tA == tB) continue;
			int t = min(tA, tB), s = max(tA, tB);
			vector<QPair> members;
			for (auto &q : D_e[s]) if (q.overlap >= k) members.push_back(q);
			for (auto &q : members) set_entry(q.vID, t, k);
		}
	}
	map<int, map<int, int>> insert;
	vector<int> ete_touched2;
	vector<pair<int,QPair>> pend_de_ins;
	{
		for (auto it = triplets.begin(); it != triplets.end(); ++it) {

			int from = it->first;
			ete_touched2.push_back(from);
			const Pair& inserted_pair = it->second;

			int to = inserted_pair.hID;
			int overlap = inserted_pair.overlap;

			pend_de_ins.push_back(make_pair(to, QPair(from, overlap)));

			for (auto vID : graph_edge[idx[from]].node) {
				auto& inner = insert[vID];
				auto& cur   = inner[to];
				cur = std::max(cur, overlap);

			}
			if (restore_dropped && dropped_by_hub && !dropped_by_hub[to].empty()) {
				for (auto &dq : dropped_by_hub[to]) {
					auto& inner2 = insert[dq.vID];
					auto& cur2   = inner2[to];
					cur2 = std::max(cur2, dq.overlap);
				}
				dropped_by_hub[to].clear();
			}

		}
	}

	if (!pend_de_ins.empty()) {
		sort(pend_de_ins.begin(), pend_de_ins.end(),
			[](const pair<int,QPair>& a, const pair<int,QPair>& b){
				if (a.first != b.first) return a.first < b.first;
				return a.second.vID < b.second.vID; });
		size_t i0 = 0;
		while (i0 < pend_de_ins.size()) {
			int to_e = pend_de_ins[i0].first;
			size_t i1 = i0;
			while (i1 < pend_de_ins.size() && pend_de_ins[i1].first == to_e) i1++;
			auto &DD = D_e[to_e];
			vector<QPair> merged;
			merged.reserve(DD.size() + (i1 - i0));
			size_t a = 0, b = i0;
			while (a < DD.size() && b < i1) {
				if (DD[a].vID < pend_de_ins[b].second.vID) merged.push_back(DD[a++]);
				else if (DD[a].vID > pend_de_ins[b].second.vID) merged.push_back(pend_de_ins[b++].second);
				else { QPair q = DD[a++]; if (q.overlap < pend_de_ins[b].second.overlap) q.overlap = pend_de_ins[b].second.overlap; b++; merged.push_back(q); }
			}
			while (a < DD.size()) merged.push_back(DD[a++]);
			while (b < i1) merged.push_back(pend_de_ins[b++].second);
			DD.swap(merged);
			i0 = i1;
		}
		pend_de_ins.clear();
	}

	{
	  {
	    sort(ete_touched2.begin(), ete_touched2.end());
	    ete_touched2.erase(unique(ete_touched2.begin(), ete_touched2.end()), ete_touched2.end());
	    {
	      pend_de_del.clear();
	      for (int te : ete_touched2) prune_dom_of(te);
	      if (!pend_de_del.empty()) {
	        sort(pend_de_del.begin(), pend_de_del.end());
	        if ((int)rec_stamp.size() < m + 2) rec_stamp.assign(m + 2, 0);
	        size_t i0 = 0;
	        while (i0 < pend_de_del.size()) {
	          int ex = pend_de_del[i0].first;
	          size_t i1 = i0;
	          while (i1 < pend_de_del.size() && pend_de_del[i1].first == ex) i1++;
	          ++rec_gen;
	          for (size_t kk = i0; kk < i1; kk++) rec_stamp[pend_de_del[kk].second] = rec_gen;
	          auto &DD = D_e[ex];
	          size_t wr = 0;
	          for (size_t rd = 0; rd < DD.size(); rd++) {
	            if (rec_stamp[DD[rd].vID] == rec_gen) continue;
	            if (wr != rd) DD[wr] = DD[rd];
	            wr++;
	          }
	          if (wr < DD.size()) DD.erase(DD.begin() + wr, DD.end());
	          i0 = i1;
	        }
	      }
	    }
	  } }

	{

		for (auto it = insert.begin(); it != insert.end(); ++it) {
			auto vID = it->first;

			const auto& inner = it->second;
			for (auto it2 = inner.begin(); it2 != inner.end(); ++it2) {

				int id2 = it2->first;
				int value = it2->second;

				auto it3 = std::lower_bound(
					label[vID].begin(), label[vID].end(), id2,
					[](const Pair& p, int id){ return p.hID < id; }
				);

				if (it3 != label[vID].end()) {

					if (it3->hID ==  id2) {

						it3->overlap = max(it3->overlap, value);
					} else {

						label[vID].insert(it3, Pair(id2, value));
					}

				} else {

					label[vID].insert(it3, Pair(id2, value));
				}


			}

		}
	}

	if (restore_dropped && dropped_by_hub) {
		int xi = (*vertex_map)[vertex_id[inserted_node]];
		if ((int)ins_xw.size() < m + 2) ins_xw.assign(m + 2, 0);
		for (auto &p : label[xi]) ins_xw[p.hID] = p.overlap;
		auto query_x = [&](int u) {
			int ov = (u == xi) ? 1 : 0;
			for (auto &p : label[u]) {
				int a = ins_xw[p.hID];
				if (a > ov && p.overlap > ov) ov = min(a, p.overlap);
			}
			return ov;
		};
		size_t a = 0;
		for (auto &hx : label[xi]) {
			while (a < ins_x_old.size() && ins_x_old[a].hID < hx.hID) a++;
			bool had = (a < ins_x_old.size() && ins_x_old[a].hID == hx.hID);
			if (had && ins_x_old[a].overlap >= hx.overlap) continue;
			int h = hx.hID, wx = hx.overlap, w_old = had ? ins_x_old[a].overlap : 0;
			auto &dl = dropped_by_hub[h];
			size_t w = 0;
			for (size_t r = 0; r < dl.size(); r++) {
				int u = dl[r].vID, s = dl[r].overlap;
				bool restored = false;
				if (s > w_old) {
					int target = min(wx, s);
					auto it3 = std::lower_bound(label[u].begin(), label[u].end(), h,
						[](const Pair& p, int id){ return p.hID < id; });
					bool has = (it3 != label[u].end() && it3->hID == h);
					if (!(has && it3->overlap >= target) && query_x(u) < target) {
						restored = true;
						if (has) it3->overlap = max(it3->overlap, s);
						else label[u].insert(it3, Pair(h, s));
					}
				}
				if (!restored) { if (w != r) dl[w] = dl[r]; w++; }
			}
			dl.erase(dl.begin() + w, dl.end());
		}
		for (auto &p : label[xi]) ins_xw[p.hID] = 0;
	}

    return true;
}







int SL::ete_edge_reach(int src, int dst) {
	int overlap = 0;

	auto srcIt = ete_label[src].begin();
	auto dstIt = ete_label[dst].begin();
	while (srcIt != ete_label[src].end() && dstIt != ete_label[dst].end()) {
		if (srcIt->hID > dstIt->hID ) {
			dstIt++;
		} else if (srcIt->hID < dstIt->hID) {
			srcIt++;
		} else {
			if (srcIt->overlap > overlap && dstIt->overlap > overlap) {
				overlap = min(srcIt->overlap, dstIt->overlap);
			}
			srcIt++;
			dstIt++;
		}

	}
	return overlap;
}

int SL::reach(int src, int dst, bool original_id) {
	if (original_id)
	{
		src = (*vertex_map)[src];
		dst = (*vertex_map)[dst];
	} else {
		src = (*vertex_map)[vertex_id[src]];
		dst = (*vertex_map)[vertex_id[dst]];
	}

	int overlap = 0;
	if (src == dst) {
		overlap = 1;
	}
	auto srcIt = label[src].begin();
	auto dstIt = label[dst].begin();
	while (srcIt != label[src].end() && dstIt != label[dst].end()) {
		if (srcIt->hID > dstIt->hID ) {
			dstIt++;
		} else if (srcIt->hID < dstIt->hID) {
			srcIt++;
		} else {
			if (srcIt->overlap > overlap && dstIt->overlap > overlap) {
				overlap = min(srcIt->overlap, dstIt->overlap);
			}
			srcIt++;
			dstIt++;
		}

	}
	return overlap;
}





int SL::baseLine(int src, int dst, bool original_id) {

	auto start_time = std::chrono::high_resolution_clock::now();

	if (original_id)
	{
		src = (*vertex_map)[src];
		dst = (*vertex_map)[dst];
	}

	vector<int> in_visit, out_visit;
	for (auto i = 0; i <= m; i++) {
		in_visit.push_back(0);
		out_visit.push_back(0);
	}
	priority_queue<Pair_in_queue> Q_in, Q_out;

	for (auto h : E[dst]) {

		Q_in.push(Pair_in_queue(h, graph_edge[h].length));
	}

	for (auto h : E[src]) {

		Q_out.push(Pair_in_queue(h, graph_edge[h].length));
	}
	int result = 0;
	int token = 0;

	if (src == dst) {
		result = 1;
	}

	while (!Q_in.empty() || !Q_out.empty()) {

		if (!token) {
			int size = Q_in.size();
			for (auto i = 0; i < size; i++) {

				auto end = std::chrono::high_resolution_clock::now();
				auto time_spent = std::chrono::duration_cast<std::chrono::seconds>(end - start_time);

				auto pair = Q_in.top();
				auto h = pair.hID;
				auto o = pair.overlap;

				Q_in.pop();
				if (out_visit[h]) {
					result = max(result, min(o, out_visit[h]));

				}
				if (in_visit[h] >= o) continue;
				in_visit[h] = o;

				map<int, int> m;

				for (auto v : graph_edge[h].node) {
					if (v > n) continue;
					for (auto nextH : E[v]) {

						if (m.find(nextH) != m.end()) {
							m[nextH]++;
						} else {
							m.insert(make_pair(nextH, 1));
						}
					}
				}

				for (auto it = m.begin(); it != m.end(); it++) {

					if (it->second <= result) continue;
					if (in_visit[it->first] >= min(it->second, o)) continue;

					Q_in.push(Pair_in_queue(it->first, min(it->second, o)));
				}
				m.clear();
			}
			token = 1;
		} else {
			int size = Q_out.size();

			for (auto i = 0; i < size; i++) {
				auto end = std::chrono::high_resolution_clock::now();
				auto time_spent = std::chrono::duration_cast<std::chrono::seconds>(end - start_time);

				auto pair = Q_out.top();
				auto h = pair.hID;
				auto o = pair.overlap;

				Q_out.pop();

				if (in_visit[h]) {
					result = max(result, min(o, in_visit[h]));

				}

				if (out_visit[h] >= o) continue;
				out_visit[h] = o;

				map<int, int> m;

				for (auto v : graph_edge[h].node) {
					if (v > n) continue;
					for (auto nextH : E[v]) {

						if (m.find(nextH) != m.end()) {
							m[nextH]++;
						} else {
							m.insert(make_pair(nextH, 1));
						}
					}
				}

				for (auto it = m.begin(); it != m.end(); it++) {

					if (it->second <= result) continue;
					if (out_visit[it->first] >= min(it->second, o)) continue;

					Q_out.push(Pair_in_queue(it->first, min(it->second, o)));
				}
				m.clear();
			}
			token = 0;
		}

	}
	return result;
}









void SL::init_vmap_direct() {
	if (vmap_direct) return;
	vmap_direct = new int[n + 2];
	vmap_direct[0] = 0;
	for (int v = 1; v <= n; v++) vmap_direct[v] = (*vertex_map)[vertex_id[v]];
}





void SL::prune_dom_of(int e) {
	auto &L = ete_label[e];
	int pmax = 0;
	size_t w = 0;
	for (size_t r = 0; r < L.size(); r++) {
		if (L[r].overlap > pmax) {
			pmax = L[r].overlap;
			if (w != r) L[w] = L[r];
			w++;
		} else {
			pend_de_del.push_back(make_pair(L[r].hID, e));
		}
	}
	if (w < L.size()) L.erase(L.begin() + w, L.end());
}

static vector<int> g_stamp, g_w1, g_w2;
static int g_st = 0;
static inline void thr_mark_reset(int m) {
	if ((int)g_stamp.size() < m + 2) { g_stamp.assign(m + 2, 0); g_w1.assign(m + 2, 0); g_w2.assign(m + 2, 0); }
	++g_st;
}

void SL::build_flat() {
	long long tot = 0;
	for (int u = 1; u <= n; u++) tot += (long long)label[u].size();
	flat_lab_off.assign(n + 2, 0);
	flat_lab.clear(); flat_lab.reserve(tot);
	for (int u = 1; u <= n; u++) {
		flat_lab_off[u] = (long long)flat_lab.size();
		for (size_t t = 0; t < label[u].size(); t++) flat_lab.push_back(label[u][t]);
	}
	flat_lab_off[n + 1] = (long long)flat_lab.size();
}

void SL::build_soa() {
	long long tot = 0;
	for (int u = 1; u <= n; u++) tot += (long long)label[u].size();
	soa_id.clear(); soa_id.reserve(tot);
	soa_w.clear();  soa_w.reserve(tot);
	flat_lab_off.assign(n + 2, 0);
	for (int u = 1; u <= n; u++) {
		flat_lab_off[u] = (long long)soa_id.size();
		for (size_t t = 0; t < label[u].size(); t++) {
			soa_id.push_back(label[u][t].hID);
			soa_w.push_back(label[u][t].overlap);
		}
	}
	flat_lab_off[n + 1] = (long long)soa_id.size();
	cout << "[SOA] label entries = " << tot << "\n";
}

void SL::build_fused() {
	fused.assign((size_t)(n + 2) * 4, 0);
	for (int v = 1; v <= n; v++) {
		int x = vmap_direct[v];
		fused[(size_t)v * 4]     = x;
		fused[(size_t)v * 4 + 1] = (int)flat_lab_off[x];
		fused[(size_t)v * 4 + 2] = (int)flat_lab_off[x + 1];
	}
	cout << "[FUSE] table bytes = " << (long long)fused.size() * 4 << "\n";
}

int SL::reach_fused(int src, int dst) {
	const int *f = fused.data();
	int ra = f[(size_t)src * 4], rb = f[(size_t)dst * 4];
	int i = f[(size_t)src * 4 + 1], ie = f[(size_t)src * 4 + 2];
	int j = f[(size_t)dst * 4 + 1], je = f[(size_t)dst * 4 + 2];
	int overlap = (ra == rb) ? 1 : 0;
	const int *id = soa_id.data(); const int *w = soa_w.data();
	while (i < ie && j < je) {
		int ai = id[i], bj = id[j];
		if (ai > bj) j++;
		else if (ai < bj) i++;
		else {
			int x = w[i], y = w[j];
			int mn = x < y ? x : y;
			if (mn > overlap) overlap = mn;
			i++; j++;
		}
	}
	return overlap;
}




