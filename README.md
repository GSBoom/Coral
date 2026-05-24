# Coral

Cache-friendly graph stream summarization.

## Build

```bash
cd experiments && make -j
```

## Run

```bash
./test_coral_mw4_cache -dataset <path> -width <w> -maxedges <n> -query <qfile> -output <ofile>
```

## Parameters

| Parameter | Meaning |
|-----------|---------|
| `-dataset` | Edge list file (format: `src dst weight timestamp`) |
| `-width` | Matrix side length `m` (m×m buckets) |
| `-maxedges` | Number of edges to insert. `0` = all |
| `-query` | File with one node ID per line for out-flow queries |
| `-output` | Result output path |
| `-p` | Candidate buckets per edge (Auxo/GSS only) |
| `-s` | Rooms per bucket (GSS only) |

## Coral Constructor

```cpp
Coral(width, fingerprint_length, maxHotRows, falsePositiveRate, cmpNum, hotRowTimes)
```

Default: `Coral(w, 16, 1000, 0.01, 16, 4)`.

**MatrixWidth**: compile-time `#define` in `Coral.h`. 2 = 8 cells/bucket, 4 = 32 cells/bucket. `coral_mw4/` uses MatrixWidth=4.

## Baselines

- `test_auxo_cache` — Auxo (PET tree)
- `test_gss_cache` — GSS (compressed matrix + buffer)
- `test_auxopro_migrate` — AuxoPro migration time

## Perf Measurement (Linux)

```bash
sudo sh -c 'echo -1 > /proc/sys/kernel/perf_event_paranoid'
bash run_llc_vs_width.sh <dataset>
bash run_mw4.sh <dataset>
```
