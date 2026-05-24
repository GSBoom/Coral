// AuxoPro migration time — track per-split cost
#include "querysupportstruct.h"
#include <fstream>
#include <string>
#include <cstring>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    string dataset  = "data/delicious-ui";
    string output   = "results/migrate_auxopro.txt";
    int fpl = 16, cols = 2, candiNum = 4, width = 1000;
    int maxedges = 0;
    int stopAfterSplits = 1;  // 默认出了第1次分裂就停

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-dataset") == 0)  dataset  = argv[++i];
        if (strcmp(argv[i], "-output") == 0)   output   = argv[++i];
        if (strcmp(argv[i], "-width") == 0)    width    = atoi(argv[++i]);
        if (strcmp(argv[i], "-maxedges") == 0) maxedges = atoi(argv[++i]);
        if (strcmp(argv[i], "-p") == 0)        candiNum = atoi(argv[++i]);
        if (strcmp(argv[i], "-stopsplits") == 0) stopAfterSplits = atoi(argv[++i]);
    }

    AuxoPro auxopro(width, cols, candiNum, fpl);

    ifstream ifs(dataset);
    if (!ifs.is_open()) { cerr << "Error: " << dataset << endl; return 1; }

    ofstream ofs(output);
    cerr << "AuxoPro migrate: w=" << width << " p=" << candiNum << " matrixSize=" << (width*width) << endl;
    ofs << "# width=" << width << " p=" << candiNum << " matrixSize=" << (width*width) << "\n";
    ofs << "# split_no\tedge_no\tinsert_time_us\tfullCount\tmem_MB\n";
    ofs.flush();

    int64_t s, d; long long w; unsigned int t;
    int loaded = 0;
    double totalTime = 0;
    double prevFullCount = 0;

    int splitsDone = 0;

    while (!ifs.eof() && (maxedges == 0 || loaded < maxedges)) {
        ifs >> s >> d >> w >> t;
        if (ifs.fail()) break;

        double beforeFull = auxopro.fullCount;
        double tIns = 0;
        auxopro.insert(to_string(s), to_string(d), 1, tIns);
        totalTime += tIns;
        loaded++;

        if (auxopro.fullCount > beforeFull) {
            splitsDone++;
            uint64_t mem = auxopro.memoryAllocated1 + 2*auxopro.memoryAllocated2 + auxopro.memoryAllocated3;
            ofs << (int)auxopro.fullCount << "\t" << loaded << "\t"
                << (int64_t)tIns << "\t" << (int)auxopro.fullCount << "\t"
                << (mem * 8.0 / 1048576) << "\n";
            ofs.flush();
            cerr << "split #" << (int)auxopro.fullCount
                 << " edge=" << loaded
                 << " time=" << (int64_t)tIns << " us" << endl;
            if (splitsDone >= stopAfterSplits) break;
        }

        if (loaded % 500000 == 0) cerr << "loaded " << loaded << " edges, "
            << (int)auxopro.fullCount << " splits" << endl;
    }
    ifs.close();

    ofs << "\n# done: edges=" << loaded << " splits=" << (int)auxopro.fullCount
        << " total_us=" << (int64_t)totalTime << "\n";
    ofs.close();

    cerr << "Done. " << (int)auxopro.fullCount << " splits." << endl;
    return 0;
}
