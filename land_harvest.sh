#!/usr/bin/env bash
# Integrate what harvest_repairwork.sh staged, respecting the secure-area rule.
#
#   bash land_harvest.sh          run detached; progress in wlog/land_harvest.log
#
# 0x02000000-0x02000800 is the DS secure area. Its stubs drift the layout cumulatively, so a batch
# of them reds a wave that each one alone would have passed. They land ONE PER WAVE, and they are
# never skiplisted -- doing that was already tried and was wrong.
set -u
SP="$(cd "$(dirname "$0")" && pwd)"
LOG="$SP/wlog/land_harvest.log"
HOLD="$SP/secure_queue"
SECURE_END=$((0x02000800))
mkdir -p "$HOLD" "$SP/wlog"

wave() {
  local mod="$1" label="$2"
  echo "$(date '+%H:%M') finish_wave $mod ($label)" >> "$LOG"
  bash "$SP/finish_wave.sh" "$mod" >> "$SP/wlog/land_${mod}.log" 2>&1
  local rc=$?
  echo "$(date '+%H:%M') rc=$rc $(tail -1 "$SP/wlog/land_${mod}.log" | cut -c1-160)" >> "$LOG"
}

echo "=== land_harvest up $(date '+%m-%d %H:%M:%S') ===" >> "$LOG"

# Park the secure-area candidates so the bulk waves cannot batch them.
for f in "$SP"/staging/main/*.cpp; do
  [ -e "$f" ] || continue
  a=$(basename "$f" .cpp | grep -o '0[0-9a-f]\{7\}' | head -1)
  [ -z "$a" ] && continue
  if [ $((0x$a)) -lt "$SECURE_END" ]; then
    mv "$f" "$HOLD/" && echo "$(date '+%H:%M') parked secure $a" >> "$LOG"
  fi
done

for m in 008 017 main; do
  ls "$SP/staging/$([ "$m" = main ] && echo main || echo "ov$m")"/*.cpp >/dev/null 2>&1 || continue
  wave "$m" "bulk"
done

# Secure area: one per wave, in address order.
for f in $(ls "$HOLD"/*.cpp 2>/dev/null | sort); do
  a=$(basename "$f" .cpp | grep -o '0[0-9a-f]\{7\}' | head -1)
  mv "$f" "$SP/staging/main/" || continue
  wave main "secure $a"
done

echo "$(date '+%H:%M') land_harvest DONE" >> "$LOG"
