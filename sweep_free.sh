#!/usr/bin/env bash
# Launch the free repair sweep across N shards. Zero tokens: every shard is a local compile plus
# wgate, so a bad candidate costs CPU and nothing else.
#
#   bash sweep_free.sh [nshards]
#
# Per-shard logs go to $SP/wlog/repairsweep_s<i>.log. Old logs are truncated first: a stale log from
# a previous run reads as live progress and has faked both usage limits and strikes before.
set -u
SP="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="${DQIX_REPO:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && { pwd -W 2>/dev/null || pwd; })/dqix-decomp}"
N="${1:-4}"

mkdir -p "$SP/wlog"
for i in $(seq 1 "$N"); do
    : > "$SP/wlog/repairsweep_s$i.log"
done

for i in $(seq 1 "$N"); do
    (cd "$REPO" && REPAIR_SHARD="$i/$N" python "$SP/repairsweep.py" \
        >> "$SP/wlog/repairsweep_s$i.log" 2>&1) &
    echo "shard $i/$N pid $!"
done
wait
