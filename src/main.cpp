#include "HI.h"

int main(int argc, char *argv[])
{
	if (argc < 6) {
		cout << "Usage: ./hl data/DatasetName.txt D|U query.txt output.txt scale [t]" << endl;
		return 0;
	}
	bool directed = false;
	if (string(argv[2]) == "D")
		directed = true;
	else if (string(argv[2]) != "U")
	{
		cout << "Parameter Error" << endl;
		return 0;
	}
	double scale = stod(argv[5]);

	string dictName = argv[1];
	dictName.erase(dictName.size() - 4);
	dictName.erase(0, 5);
	string dictPath = "scale/" + dictName;
	string resultDir = dictPath + "/query_time/" + to_string(static_cast<int>(scale * 100)) + "/";
	std::filesystem::create_directories(resultDir);
	string filePath = dictPath + "/scale.txt";

	srand(time(NULL));

	Graph * graph = new Graph(argv[1], directed, scale);
	SL * alg = new SL(graph);
	alg->construct(filePath);
	cout << "construct complete\n";

	if (!std::filesystem::exists(dictPath + "/insert.txt")) {
		std::ofstream insert_gen(dictPath + "/insert.txt", std::ofstream::trunc);
		for (auto i = 0; i < 1000; i++) {
			int u = rand() % graph->m + 1;
			int v = rand() % graph->n + 1;
			insert_gen << u << " " << v << "\n";
		}
	}

	std::ifstream insertquery(dictPath + "/insert.txt");
	std::ofstream insertLog(dictPath + "/insert.log", std::ios::trunc);
	int edge, node, total_insert = 0, loop_cnt = 0;
	long long total_ns = 0;
	while (insertquery >> edge >> node) {
		++loop_cnt;
		auto start_time = std::chrono::high_resolution_clock::now();
		bool res = alg->insert_node(edge, node);
		auto end_time = std::chrono::high_resolution_clock::now();
		long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
		total_ns += ns;
		total_insert += res;
		insertLog << loop_cnt << " (" << edge << ", " << graph->vertex_id[node] << ") " << ns << " ns\n";
	}
	insertLog << "total insertions = " << loop_cnt << ", valid insertions = " << total_insert
	          << ", total time = " << total_ns << " ns, average time = "
	          << (loop_cnt ? total_ns / loop_cnt : 0) << " ns\n";
	insertLog.close();
	cout << "total valid insert = " << total_insert << ", average insertion time = "
	     << (loop_cnt ? total_ns / loop_cnt : 0) << " ns\n";

	alg->init_vmap_direct();
	alg->build_flat();
	alg->build_soa();
	alg->build_fused();

	int query_mismatch = 0;
	for (auto overlap = 0; overlap <= 4; overlap++) {
		std::ofstream minTime(resultDir + "minTime" + to_string(overlap) + ".txt", std::ios::trunc);
		std::ofstream reachTime(resultDir + "spanReachTime" + to_string(overlap) + ".txt", std::ios::trunc);
		std::ofstream baseTime(resultDir + "basetime" + to_string(overlap) + ".txt", std::ios::trunc);
		for (int total = 0; total < 1000; total++) {
			int i = rand() % graph->n + 1;
			int j = rand() % graph->n + 1;

			auto start_time = std::chrono::high_resolution_clock::now();
			int resMin = alg->reach_fused(i, j);
			auto end_time = std::chrono::high_resolution_clock::now();
			minTime << std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count() << "\n";

			start_time = std::chrono::high_resolution_clock::now();
			int resReach = alg->reach(i, j, 0);
			end_time = std::chrono::high_resolution_clock::now();
			reachTime << std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count() << "\n";

			start_time = std::chrono::high_resolution_clock::now();
			int resBase = alg->baseLine(i, j, 0);
			end_time = std::chrono::high_resolution_clock::now();
			baseTime << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << "\n";

			if (resMin != resBase || resReach != resBase) {
				query_mismatch++;
				cout << "MISMATCH at (" << i << ", " << j << "): index = " << resMin
				     << ", reach = " << resReach << ", baseline = " << resBase << "\n";
			}
		}
	}

	if (query_mismatch == 0) {
		cout << "================================PASS================================\n";
	} else {
		cout << "================================FAIL: " << query_mismatch << " mismatches================================\n";
	}
	return 0;
}
