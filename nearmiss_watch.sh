#!/bin/bash
# Keep the near-miss priority queue current. Usage: bash nearmiss_watch.sh [seconds]
#
# The queue is built from blockers.tsv, which the gate appends to on every worker verdict -- so a
# queue built once is correct for one moment and then decays: every new near-miss goes unqueued and
# every one the fleet lands stays queued until someone remembers to rebuild. `nearmiss.py` drops
# addresses that are already delinked, so regenerating is idempotent and safe to run on a timer.
#
# This is a standalone loop only because pull_all.sh was mid-function with an opus worker when it
# was written; it belongs in that loop next to the repair sweep, at the next quiesce.
KIT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
SP="$(python "$KIT/kitpaths.py" state)"
EVERY="${1:-900}"
LOG="$SP/wlog/nearmiss_watch.log"
if [ -f "$SP/nearmiss_watch.pid" ] && kill -0 "$(cat "$SP/nearmiss_watch.pid" 2>/dev/null)" 2>/dev/null; then
  echo "$(date '+%H:%M') already running as $(cat "$SP/nearmiss_watch.pid") -- not starting a second" >> "$LOG"
  exit 0
fi
echo "=== nearmiss_watch up $(date '+%m-%d %H:%M:%S') (pid $$, every ${EVERY}s) ===" >> "$LOG"
echo $$ > "$SP/nearmiss_watch.pid"
trap 'rm -f "$SP/nearmiss_watch.pid"' EXIT

while :; do
  [ -e "$SP/STOP_PULL" ] && { echo "$(date '+%H:%M') stop flag" >> "$LOG"; break; }
  n=$(python "$KIT/nearmiss.py" --write-priority 2>&1 | grep -cE '^ *[0-9]+ +[0-9]+ ')
  echo "$(date '+%H:%M') queue rebuilt: $n near-miss function(s)" >> "$LOG"
  sleep "$EVERY"
done
