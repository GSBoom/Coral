#!/bin/bash
set -e

DATASET="${1:-data/delicious-ui}"
MAXEDGES=${2:-0}
WIDTHS="4000 6000 8000"
OUTDIR="results"
EVENTS="longest_lat_cache.miss,longest_lat_cache.reference,L1-dcache-load-misses,L1-dcache-loads,cache-misses,cache-references,\
L2-load-misses,L2-loads,\
cycles,instructions"

mkdir -p $OUTDIR
COREDIR="$OUTDIR"
QUERY="$OUTDIR/query_nodes.txt"

if [ ! -f "$QUERY" ]; then
    echo "Generating query file..."
    ./gen_queries -dataset "$DATASET" -n 5000 -output "$QUERY"
fi

echo "=== Coral MW4 (MatrixWidth=4, 32 cells/basket) ==="

for w in $WIDTHS; do
    out="$OUTDIR/result_coral_mw4_w${w}_r1.txt"
    perf_out="$OUTDIR/perf_coral_mw4_w${w}_r1.txt"
    ready="$out.ready"

    echo "  Coral_MW4 w=$w: inserting..."
    ./test_coral_mw4_cache \
        -dataset "$DATASET" -width $w -maxedges $MAXEDGES \
        -query "$QUERY" \
        -output "$out" &
    PID=$!

    while [ ! -f "$ready" ]; do sleep 5; done
    rm -f "$ready"
    echo "  Coral_MW4 w=$w: insertion done, measuring queries..."

    taskset -c 0 perf stat -e $EVENTS -o "$perf_out" -p $PID &
    PERF_PID=$!
    wait $PID
    wait $PERF_PID 2>/dev/null

    echo "  Coral_MW4 w=$w: done"
done

echo ""
echo "=== DONE ==="
ls -lt $OUTDIR/perf_coral_mw4_*
