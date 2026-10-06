#!/bin/bash
# Backstop for the STOP banner: kill a session that keeps gating after the gate told it to stop.
# usage: gatewatch.sh <main|NNN> <addr> <session> <pid>
SP="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
MOD="$1"; ADDR="$2"; SESS="$3"; PID="$4"
GRACE="${GATEWATCH_GRACE:-180}"
POLL="${GATEWATCH_POLL:-20}"
LOG="$SP/wlog/gatewatch.log"
first=0

[ -z "$PID" ] && exit 2
while kill -0 "$PID" 2>/dev/null; do
  sleep "$POLL"
  st=$(python "$SP/gatelog.py" "$MOD" "$ADDR" "$SESS" 2>/dev/null | head -1)
  case "$st" in
    STALL*)
      now=$(date +%s)
      [ "$first" = 0 ] && first=$now
      if [ $((now - first)) -ge "$GRACE" ]; then
        echo "$(date '+%H:%M') $MOD:$ADDR $st -- killed $PID after ${GRACE}s past the STOP banner" >> "$LOG"
        kill -TERM "$PID" 2>/dev/null
        exit 0
      fi
      ;;
    *) first=0 ;;
  esac
done
exit 0
