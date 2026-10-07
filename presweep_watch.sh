#!/bin/bash
# Sweep the addresses a worker is ABOUT to be given, so no slot ever waits for it.
# Usage: bash presweep_watch.sh [seconds between passes]
#
# presweep at claim time is the right THING in the wrong PLACE: the rules take minutes, and a slot
# blocked on them is a slot not working. Running ahead of the queue makes the claim-time call a
# no-op -- it finds the marker and returns immediately -- while the sweep itself happens on CPU
# nobody is waiting for.
#
# The order walked is the order claim.py will serve: the near-miss priority queue first, then each
# module's next few addresses. presweep.py skips anything already swept at its current artifact, so
# a pass over an unchanged queue costs almost nothing.
KIT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
SP="$(python "$KIT/kitpaths.py" state)"
REPO="$(python "$KIT/kitpaths.py" repo)"
EVERY="${1:-600}"
LOG="$SP/wlog/presweep_watch.log"
# REFUSE TO BE THE SECOND COPY. Every relaunch without this stacks another loop on the same queue:
# three accumulated in one minute of restarts, all sweeping the same addresses.
if [ -f "$SP/presweep_watch.pid" ] && kill -0 "$(cat "$SP/presweep_watch.pid" 2>/dev/null)" 2>/dev/null; then
  echo "$(date '+%H:%M') already running as $(cat "$SP/presweep_watch.pid") -- not starting a second" >> "$LOG"
  exit 0
fi
echo "=== presweep_watch up $(date '+%m-%d %H:%M:%S') (pid $$, every ${EVERY}s) ===" >> "$LOG"
echo $$ > "$SP/presweep_watch.pid"
trap 'rm -f "$SP/presweep_watch.pid"' EXIT
cd "$REPO" || exit 2

while :; do
  [ -e "$SP/STOP_PULL" ] && { echo "$(date '+%H:%M') stop flag" >> "$LOG"; break; }
  for _mod in $(python "$KIT/claim.py" --pools 2>/dev/null | awk '{print $1}' | head -4); do
    _marg="$_mod"; [ "$_mod" = "main" ] || _marg="$_mod"
    for _a in $(python "$KIT/claim.py" "$_mod" --peek "${PRESWEEP_AHEAD:-4}" 2>/dev/null \
                | awk '{print $1}' | grep -E '^[0-9a-f]{8}$'); do
      [ -e "$SP/STOP_PULL" ] && break 3
      _r=$(timeout "${PRESWEEP_TIMEOUT:-1800}" python "$KIT/presweep.py" "$_marg" "$_a" --deep 2>/dev/null | tr -d '\r')
      case "$_r" in
        MATCH*)    echo "$(date '+%H:%M') $_mod $_a MATCHED ahead of the queue -- ${_r#MATCH }" >> "$LOG" ;;
        IMPROVED*) echo "$(date '+%H:%M') $_mod $_a ${_r}" >> "$LOG" ;;
      esac
    done
  done
  sleep "$EVERY"
done
