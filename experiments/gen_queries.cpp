// Generate query node file from dataset
// Usage: ./gen_queries -dataset <path> -n <num_queries> -output <file>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <cstring>
#include <cstdlib>
#include <cstdint>

using namespace std;

int main(int argc, char* argv[]) {
    string dataset = "../../DataSet/Delicious-ui/delicious-ui";
    string output  = "cache_query_nodes.txt";
    int nQueries   = 10000;
    int maxRead    = 10000000;  // scan first 10M edges for node IDs

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-dataset") == 0) dataset  = argv[++i];
        if (strcmp(argv[i], "-n") == 0)       nQueries = atoi(argv[++i]);
        if (strcmp(argv[i], "-output") == 0)  output   = argv[++i];
    }

    ifstream ifs(dataset);
    if (!ifs.is_open()) { cerr << "Error: " << dataset << endl; return 1; }

    vector<string> nodePool;
    set<string> seen;
    int64_t s, d; long long w; unsigned int t;
    int read = 0;

    while (!ifs.eof() && read < maxRead) {
        ifs >> s >> d >> w >> t;
        if (ifs.fail()) break;
        string src = to_string(s);
        string dst = to_string(d);
        if (seen.insert(src).second) nodePool.push_back(src);
        if (seen.insert(dst).second) nodePool.push_back(dst);
        read++;
        if (read % 1000000 == 0) cerr << "scanned " << read << " edges, " << nodePool.size() << " unique nodes" << endl;
    }
    ifs.close();

    cerr << "Total unique nodes in pool: " << nodePool.size() << endl;

    // Sample nQueries randomly
    srand(42);
    ofstream ofs(output);
    for (int i = 0; i < nQueries && i < (int)nodePool.size(); i++) {
        // Fisher-Yates style sampling
        int j = i + rand() % (nodePool.size() - i);
        swap(nodePool[i], nodePool[j]);
        ofs << nodePool[i] << endl;
    }
    ofs.close();

    cerr << "Wrote " << min(nQueries, (int)nodePool.size()) << " query nodes to " << output << endl;
    return 0;
}
