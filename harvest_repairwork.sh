#!/usr/bin/env bash
# Re-gate every repairsweep work copy and stage the ones that still MATCH.
#
#   bash harvest_repairwork.sh [--stage]     without --stage it only reports
#
# repairsweep.py gates its repaired copy in repair_work/ and copies a HIT to staging/ exactly once.
# If a later wave clears that staging copy while rejecting some other candidate for the same module,
# the proven-matching file is left in repair_work/ -- which resumable.py excludes by design and no
# integrator gathers from. Measured 08-20: five HIT copies from the 08-19 sweep still gated MATCH a
# day later while their addresses sat in the resume queue as unmatched work to pay a worker for.
set -u
SP="$(cd "$(dirname "$0")" && pwd)"
REPO="${DQIX_REPO:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && { pwd -W 2>/dev/null || pwd; })/dqix-decomp}"
STAGE_IT=0
[ "${1:-}" = "--stage" ] && STAGE_IT=1
LOG="$SP/wlog/harvest_repairwork.log"
cd "$REPO" || exit 2

: > "$LOG"
# Only the copies repairsweep already recorded as a HIT. repair_work/ holds 3205 files -- gating all
# of them costs hours to re-derive verdicts that were written down at the time.
HITS=$(cat "$SP"/wlog/repairsweep*.log "$SP/repair_hits.txt" 2>/dev/null \
       | sed -n 's/^HIT \([a-z0-9]*\) \(0[0-9a-f]\{7\}\).*/\1_\2/p; s/^\([a-z0-9]*\) \(0[0-9a-f]\{7\}\) .*/\1_\2/p' \
       | sort -u)
[ -z "$HITS" ] && { echo "harvest_repairwork: no recorded HITs"; exit 0; }

match=0; stale=0; skipped=0
for b in $HITS; do
  f="$SP/repair_work/$b.cpp"
  [ -e "$f" ] || continue
  MOD="${b%%_*}"; ADDR="${b##*_}"
  # Already committed work re-gates as a duplicate, not a win.
  if [ -z "$(python "$SP/resumable.py" "$ADDR" 2>/dev/null)" ]; then
    skipped=$((skipped + 1)); continue
  fi
  v=$(python "$SP/wgate.py" "$MOD" "$ADDR" "$f" 2>&1 | head -1)
  case "$v" in
    MATCH*)
      match=$((match + 1))
      echo "MATCH $MOD $ADDR" >> "$LOG"
      if [ "$STAGE_IT" = 1 ]; then
        D="$SP/staging/$([ "$MOD" = main ] && echo main || echo "ov$MOD")"
        mkdir -p "$D"; cp "$f" "$D/$ADDR.cpp"
      fi
      ;;
    *) stale=$((stale + 1)); echo "STALE $MOD $ADDR $v" >> "$LOG" ;;
  esac
done
echo "harvest_repairwork: $match match, $stale stale, $skipped already-committed (staged=$STAGE_IT)" | tee -a "$LOG"
