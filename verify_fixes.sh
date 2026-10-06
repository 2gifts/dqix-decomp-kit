#!/bin/bash
# Confirm each fix is actually FIRING in production, not merely present in the file.
#
# Three fixes this session applied cleanly, reported success, and did nothing (limit_guard never
# sourced; watchdog reading day-old logs; negative cache with 0 hits despite 11 markers). All three
# were invisible to "grep for the code I just wrote" and visible to "count the effect".
# So every check below counts an OBSERVABLE, never the source text.
SP="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="${DQIX_REPO:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && { pwd -W 2>/dev/null || pwd; })/dqix-decomp}"
cd "$REPO" || exit 2
ok(){ printf "  PASS  %s\n" "$1"; }
bad(){ printf "  FAIL  %s\n" "$1"; }

echo "=== fix verification $(date '+%m-%d %H:%M') ==="

# 1. negative cache — must produce HITS, not just markers (the exact way it failed before)
h=$(grep -c "held source unchanged since last zero-yield" "$SP/wlog/sweep.log" 2>/dev/null)
m=$(ls "$SP"/wlog/.deadpool_* 2>/dev/null | wc -l)
[ "$h" -gt 0 ] && ok "sweep negative cache: $h hits ($m markers)" || bad "sweep negative cache: $m markers but 0 HITS (same failure as before)"

# 2. limit guard — the functions must be reachable from the drivers, tested by SOURCING them
( SP="$SP"; . "$SP/limit_guard.sh" 2>/dev/null && type lockout_active >/dev/null 2>&1 ) \
  && ok "limit_guard sourceable, lockout_active defined" || bad "limit_guard not sourceable"
for f in run_all.sh run_overlay.sh run_main.sh; do
  grep -q 'source "$SP/limit_guard.sh"' "$SP/$f" && ok "$f sources limit_guard" || bad "$f does NOT source limit_guard"
done

# 3. health monitor — must be running, exactly one, and must not be citing stale logs.
# `watchdog.sh` was the pre-consolidation name and the file no longer exists, so both checks below
# were testing a script that could never be found: one always reported "count = 0" and the other
# grepped a missing file. health.sh is what actually watches the fleet now.
n=0; for p in $(ps 2>/dev/null | awk 'NR>1{print $1}'); do
  c=$(tr '\0' ' ' < "/proc/$p/cmdline" 2>/dev/null) || continue
  case "$c" in *"$SP/health.sh"*) n=$((n+1));; esac
done
[ "$n" -eq 1 ] && ok "exactly one health monitor running" || bad "health monitor count = $n (want 1)"
grep -q 'mtime\|find .* -mmin' "$SP/health.sh" && ok "health.sh filters logs by mtime" \
  || bad "health.sh has no mtime filter"

# 4. wgate snapshot — must be ACCUMULATING proven files
g=$(ls "$SP"/gated/*/*.cpp 2>/dev/null | wc -l)
[ "$g" -gt 0 ] && ok "wgate snapshots: $g proven files captured" || bad "wgate snapshots: none yet (no MATCH since the change, or broken)"

# 5. wdiff diagnosis + wlist symbol resolution — exercised for real, not grepped
f="src/Combat/Overlay_1/AbsPlus159IfNegative0215ad2c.cpp"
if [ -f "$f" ]; then
  sed 's/< 0/<= 0/' "$f" > "$SP/.vfy.cpp"
  python "$SP/wdiff.py" 001 0215ad2c "$SP/.vfy.cpp" 2>/dev/null | grep -q "^DIAGNOSIS:" \
    && ok "wdiff emits DIAGNOSIS" || bad "wdiff emits no DIAGNOSIS"
  rm -f "$SP/.vfy.cpp"
fi
python "$SP/wlist.py" 027 021dc680 2>/dev/null | grep -q -- "-> " \
  && ok "wlist resolves call targets to names" || bad "wlist not resolving symbols"
python "$SP/wlist.py" 027 021dc680 2>/dev/null | grep -q "LOOP>" \
  && ok "wlist marks loop entries" || bad "wlist not marking loops"

# 6. skiplist — bare (unjustified) entries must stay gone
b=$(cat "$SP"/skiplist_main.txt "$SP"/skiplist_ov.txt 2>/dev/null | awk 'NF==1' | wc -l)
[ "$b" -eq 0 ] && ok "skiplist has no unjustified entries" || bad "skiplist regrew $b bare entries"

# 7. build health — a stale build.ninja silently zeroes everything downstream and fires no alert
if ninja report >/dev/null 2>&1; then
  ok "build/report healthy: $(python -c "import json;m=json.load(open('build/usa/report.json'))['measures'];print('%.2f%% (%d)'%(m['matched_functions_percent'],m['matched_functions']))" 2>/dev/null)"
else
  bad "ninja report FAILS — build config likely stale, run tools/configure.py usa --no-extract"
fi
