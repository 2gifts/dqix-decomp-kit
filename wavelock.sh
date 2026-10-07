wave_lock_acquire() {
  local tries="$1" i own
  LOCK="$SP/wave.lock"
  for ((i = 1; i <= tries; i++)); do
    if mkdir "$LOCK" 2>/dev/null; then echo $$ > "$LOCK/pid"; return 0; fi
    own=$(cat "$LOCK/pid" 2>/dev/null)
    if [ -n "$own" ] && ! kill -0 "$own" 2>/dev/null; then
      echo "wave.lock owner $own is dead -- clearing stale lock"
      rm -f "$LOCK/pid"; rmdir "$LOCK" 2>/dev/null; continue
    fi
    if [ -z "$own" ] && [ -n "$(find "$LOCK" -maxdepth 0 -mmin +5 2>/dev/null)" ]; then
      echo "wave.lock has no owner and is stale -- clearing"
      rmdir "$LOCK" 2>/dev/null; continue
    fi
    [ "$i" -lt "$tries" ] && sleep 20
  done
  return 1
}

wave_lock_release() {
  [ "$(cat "$LOCK/pid" 2>/dev/null)" = "$$" ] || return 0
  rm -f "$LOCK/pid"; rmdir "$LOCK" 2>/dev/null
}
