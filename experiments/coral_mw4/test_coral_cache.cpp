// Cache-friendliness test for Coral — node out-flow query
// Usage: ./test_coral_cache -dataset <path> -width <w> -maxedges <n> -query <file> -output <file>
// Wrap with: perf stat -e LLC-load-misses,LLC-loads,cache-misses,cache-references

#include <fstream>
#include <string>
#include "Coral.h"
#include <cstring>
#include <cstdint>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    string dataset   = "../DataSet/Delicious-ui/delicious-ui";
    string queryfile = "cache_query_nodes.txt";
    string output    = "cache_coral_result.txt";
    int width = 512;
    int maxedges = 5000000;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-dataset") == 0)  dataset  = argv[++i];
        if (strcmp(argv[i], "-query") == 0)    queryfile = argv[++i];
        if (strcmp(argv[i], "-output") == 0)   output   = argv[++i];
        if (strcmp(argv[i], "-width") == 0)    width    = atoi(argv[++i]);
        if (strcmp(argv[i], "-maxedges") == 0) maxedges = atoi(argv[++i]);
    }

    // Coral(width, fingerprint_length, maxhotrownumber, falsePositiveRate, comparenumber, hotrowtimes)
    Coral coral(width, 16, 1000, 0.01, 16, 4);

    // === Phase 1: Pre-load edges ===
    ifstream ifs(dataset);
    if (!ifs.is_open()) { cerr << "Error: " << dataset << endl; return 1; }

    int64_t s, d; long long w; unsigned int t;
    int loaded = 0;
    while (!ifs.eof() && (maxedges == 0 || loaded < maxedges)) {
        ifs >> s >> d >> w >> t;
        if (ifs.fail()) break;
        coral.insert(to_string(s), to_string(d), 1);
        loaded++;
        if (loaded % 500000 == 0) cerr << "Coral pre-loaded " << loaded << " edges" << endl;
    }
    ifs.close();
    cerr << "INSERT_DONE" << endl;
    string coralReadyFile = output + ".ready";
    { ofstream rf(coralReadyFile); rf << getpid() << endl; rf.close(); }
    sleep(5);
    // === Phase 2: Read query node IDs ===
    vector<string> queries;
    ifstream qfs(queryfile);
    if (!qfs.is_open()) { cerr << "Error: " << queryfile << endl; return 1; }
    string node;
    while (getline(qfs, node)) {
        if (!node.empty()) queries.push_back(node);
    }
    qfs.close();
    int nQueries = queries.size();

    // === Phase 3: Run out-flow queries ===
    volatile long long totalWeight = 0;

    for (int i = 0; i < nQueries; i++) {
        int w = coral.nodeWeightQuery(queries[i], 0);  // type=0 = out-flow
        totalWeight += w;
    }

    // === Phase 4: Output ===
    ofstream ofs(output);
    ofs << "structure: Coral_MW4" << endl;
    ofs << "width: " << width << endl;
    ofs << "edges_loaded: " << loaded << endl;
    ofs << "queries: " << nQueries << endl;
    ofs << "checksum: " << totalWeight << endl;
    ofs.close();

    return 0;
}
