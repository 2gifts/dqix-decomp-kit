#!/bin/bash
# Budget-driven pull fleet: split a spend allowance across N slots, each pulling from the shared
# pool until its share is gone.  Usage: pull_fleet.sh <main|NNN> <total_budget_usd> [slots]
#
# The knob is MONEY, which is the thing actually being managed. Functions-per-worker and
# worker-count were both proxies for it and neither bounded spend: the wall-clock timeout could not
# stop a fast runaway, and two sessions took $1,135 of one week's $2,828 with nothing in the way.
# Here the ceiling is arithmetic -- N slots x their share, and every session reports its real cost.
#
# Integration is NOT done here. Slots only produce gated source in the module directory; landing it
# stays with finish_wave, which owns the wave lock, the drift culling and the push. Run this INSTEAD
# of run_module for a module, never alongside it -- both would claim from the same pool and pay for
# the same functions twice.
SP="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
MOD="${1:-main}"
TOTAL="${2:-20}"
SLOTS="${3:-4}"

rm -f "$SP/STOP_PULL"
PER=$(python -c "print(round($TOTAL / $SLOTS, 4))")
echo "$(date '+%m-%d %H:%M') pull_fleet $MOD: \$$TOTAL across $SLOTS slots (\$$PER each)" \
  >> "$SP/wlog/pull_fleet.log"

pids=()
for ((s=1; s<=SLOTS; s++)); do
  bash "$SP/pull_worker.sh" "$MOD" "$s" "$PER" &
  pids+=($!)
done
wait "${pids[@]}"

# One report, from the slot logs, so the run can be compared against autotune's $/function baseline.
python - "$MOD" <<'PY' >> "$SP/wlog/pull_fleet.log"
import glob, os, re, sys
SP = "$SP"
mod = sys.argv[1]
matched = tried = 0
spent = 0.0
for p in glob.glob(f"{SP}/wlog/pull_{mod}_s*.log"):
    for line in open(p, encoding="utf-8", errors="replace"):
        m = re.search(r"\b(MATCH|miss) [0-9a-f]{8} \$([0-9.]+)", line)
        if m:
            tried += 1
            spent += float(m.group(2))
            matched += m.group(1) == "MATCH"
per = f"${spent/matched:.2f}/function" if matched else "no matches"
print(f"  result: {matched}/{tried} matched, ${spent:.2f} spent, {per}")
PY
tail -1 "$SP/wlog/pull_fleet.log"
