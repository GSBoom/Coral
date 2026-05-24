#!/bin/bash
# LLC miss vs matrix width — Coral MW4
# Same data volume for all widths, fair comparison
set -e

DATASET="${1:-data/delicious-ui}"
NQUERIES=5000
MAXEDGES=20000000
WIDTHS="7168"
OUTDIR="results_llc_width"
EVENTS="longest_lat_cache.miss,longest_lat_cache.reference,\
L1-dcache-load-misses,L1-dcache-loads,\
cache-misses,cache-references,cycles,instructions"

mkdir -p $OUTDIR
./gen_queries -dataset "$DATASET" -n $NQUERIES -output "$OUTDIR/query_nodes.txt"

for w in $WIDTHS; do
    echo "=== Coral MW4 w=$w ==="

    out="$OUTDIR/result_coral_w${w}.txt"
    perf_out="$OUTDIR/perf_coral_w${w}.txt"
    ready="$out.ready"

    ./test_coral_mw4_cache \
        -dataset "$DATASET" -width $w -maxedges $MAXEDGES \
        -query "$OUTDIR/query_nodes.txt" \
        -output "$out" &
    PID=$!

    while [ ! -f "$ready" ]; do sleep 3; done
    rm -f "$ready"

    taskset -c 0 perf stat -e $EVENTS -o "$perf_out" -p $PID &
    PERF_PID=$!
    wait $PID
    wait $PERF_PID 2>/dev/null

    echo "  Done."
done

echo "=== ALL DONE ==="
