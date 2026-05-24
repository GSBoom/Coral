// Cache-friendliness test for Auxo — node out-flow query
// Usage: ./test_auxo_cache -dataset <path> -width <w> -maxedges <n> -query <file> -output <file>
// Wrap with: perf stat -e LLC-load-misses,LLC-loads,cache-misses,cache-references

#include "querysupportstruct.h"
#include <fstream>
#include <string>
#include <cstring>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    string dataset   = "../../DataSet/Delicious-ui/delicious-ui";
    string queryfile = "cache_query_nodes.txt";
    string output    = "cache_auxo_result.txt";
    int fpl = 16, cols = 2, candiNum = 1, width = 512;
    int maxedges = 5000000;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-dataset") == 0)  dataset  = argv[++i];
        if (strcmp(argv[i], "-query") == 0)    queryfile = argv[++i];
        if (strcmp(argv[i], "-output") == 0)   output   = argv[++i];
        if (strcmp(argv[i], "-width") == 0)    width    = atoi(argv[++i]);
        if (strcmp(argv[i], "-maxedges") == 0) maxedges = atoi(argv[++i]);
    }

    Auxo auxo(width, cols, candiNum, fpl);

    // === Phase 1: Pre-load edges ===
    ifstream ifs(dataset);
    if (!ifs.is_open()) { cerr << "Error: " << dataset << endl; return 1; }

    int64_t s, d; int w; unsigned int t;
    int loaded = 0;
    double insertTime = 0;
    while (!ifs.eof() && (maxedges == 0 || loaded < maxedges)) {
        ifs >> s >> d >> w >> t;
        if (ifs.fail()) break;
        auxo.insert(to_string(s), to_string(d), 1, insertTime);
        loaded++;
        if (loaded % 500000 == 0) cerr << "Auxo pre-loaded " << loaded << " edges" << endl;
    }
    ifs.close();
    cerr << "INSERT_DONE" << endl;

    // Signal readiness + wait for perf to attach
    string readyFile = output + ".ready";
    { ofstream rf(readyFile); rf << getpid() << endl; rf.close(); }
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
    double queryTime = 0;
    volatile long long totalWeight = 0;  // volatile to prevent optimization

    for (int i = 0; i < nQueries; i++) {
        int w = auxo.nodeValueQuery(queries[i], 0, queryTime);  // type=0 = out-flow
        totalWeight += w;
    }

    // === Phase 4: Output ===
    ofstream ofs(output);
    ofs << "structure: Auxo" << endl;
    ofs << "width: " << width << endl;
    ofs << "edges_loaded: " << loaded << endl;
    ofs << "queries: " << nQueries << endl;
    ofs << "total_time_us: " << (int64_t)queryTime << endl;
    ofs << "checksum: " << totalWeight << endl;
    ofs.close();

    return 0;
}
