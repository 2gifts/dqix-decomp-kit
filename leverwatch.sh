#!/usr/bin/env bash
SP="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ONCE=0
[ "$1" = "--once" ] && { ONCE=1; shift; }
INTERVAL="${1:-300}"
SEEN="${LEVERWATCH_SEEN:-$SP/wlog/leverwatch_seen.txt}"
touch "$SEEN"

while :; do
  fired=0
  while read -r addr text; do
    [ -n "$addr" ] || continue
    grep -qxF "$addr" "$SEEN" && continue
    echo "$addr" >> "$SEEN"
    echo "LEVER NEEDS PROMOTING $addr: $text"
    fired=1
  done < <(cd "$SP" && python levercheck.py --keys 2>/dev/null)
  [ "$ONCE" = 1 ] && [ "$fired" = 1 ] && exit 0
  sleep "$INTERVAL"
done
