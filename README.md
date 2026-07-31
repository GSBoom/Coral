# Coral: Cache-friendly Efficient Graph Stream Summarization

Coral is a cache-friendly data structure for graph-stream summarization. This
repository contains the Coral implementation and a compact experiment driver
for validating the insertion and out-flow query workflow.

The full experimental datasets and generated results are not included.

## Repository structure

| Path | Description |
| --- | --- |
| `coral/` | Coral implementation |
| `experiments/` | Coral experiment driver |
| `examples/` | Synthetic stream, query set, statistics, and expected result |
| `Makefile` | Build entry point |

## Requirements

- a C++17-compatible compiler;
- GNU Make;
- a Linux/POSIX environment.

## Build

Build Coral from the repository root:

```bash
make -j"$(nproc)"
```

The executable is generated as `experiments/test_coral_cache`.

## Input format

Each graph-stream record contains four whitespace-separated fields:

```text
source_id destination_id weight timestamp
```

The query file contains one source-node identifier per line:

```text
node_id
```

## Run Coral

The included fixture contains 100,000 records, comprising 80,000 distinct
edges and 20,000 repeated occurrences over 4,096 source nodes.

```bash
./experiments/test_coral_cache \
  -dataset examples/medium_stream.txt \
  -width 512 \
  -maxedges 0 \
  -query examples/medium_query_nodes.txt \
  -output medium_result.txt
```

The generated `medium_result.txt` should match
`examples/expected_medium_result.txt`.

## Paper-scale experiment entry

The same executable accepts a larger graph stream, matrix width, query file,
and output path through the options shown above. Experiment-specific parameter
settings are supplied by the corresponding paper configuration rather than
duplicated in this README.

## Dataset placement

The full datasets are not uploaded because of their size. Place a dataset in a
local `data/` directory or any accessible location and pass its path through
the `-dataset` option. The fixture in `examples/` is intended only for
functional verification.
