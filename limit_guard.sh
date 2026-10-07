#!/bin/bash
# Shared usage-limit guard. `source` it; it defines kill_workers, lockout_active, enter_lockout and
# probe_limit, and nothing else runs at source time.
#
# RULE (user-stated, twice): at a WEEKLY usage limit every worker is KILLED and no new wave is spawned
# until the reset. Measured 2026-08-11 — since the weekly reset the fleet had burned ~$1,482 equivalent
# (22.9M output, 3.35B cache-read) for 120 functions, while the account was on the way to being locked
# again. Relaunching into a locked account is pure loss: each spawned worker pays full startup (worker
# doc + scaffold + symbols) and then dies on the first API call. That happened 75 times across four days.
KIT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
SP="$(python "$KIT/kitpaths.py" state)"
LOCKFILE="$SP/WEEKLY_LOCKOUT"     # contains a single epoch second: when spawning may resume

# kill_workers -- SIGKILL every `claude -p` worker and its `timeout` wrapper, leaving drivers alone.
# Matches on the worker prompt text rather than on the binary name, so the interactive session that is
# supervising all this is never a candidate. Safe to call when there are none.
kill_workers() {
  local n=0 p c
  for p in $(ps 2>/dev/null | awk 'NR>1{print $1}'); do
    c=$(tr '\0' ' ' < "/proc/$p/cmdline" 2>/dev/null)
    case "$c" in
      *"DQIX decomp worker"*) kill -9 "$p" 2>/dev/null && n=$((n+1)) ;;
    esac
  done
  [ "$n" -gt 0 ] && echo "limit_guard: killed $n worker processes"
  return 0
}

# lockout_active -- true while a recorded weekly lockout has not yet expired. Callers must consult this
# BEFORE spawning anything. A stale file is self-clearing: once the deadline passes it is removed.
lockout_active() {
  [ -f "$LOCKFILE" ] || return 1
  local until now
  until=$(cat "$LOCKFILE" 2>/dev/null); now=$(date +%s)
  case "$until" in ''|*[!0-9]*) rm -f "$LOCKFILE"; return 1 ;; esac
  if [ "$now" -ge "$until" ]; then rm -f "$LOCKFILE"; return 1; fi
  return 0
}

# enter_lockout <seconds> -- record the deadline and kill the fleet. Idempotent.
enter_lockout() {
  local secs=${1:-3600}
  echo $(( $(date +%s) + secs )) > "$LOCKFILE"
  kill_workers
  echo "limit_guard: WEEKLY LOCKOUT recorded, no spawning for ${secs}s"
}

# probe_limit -- 0 when spawning is worth doing, non-zero when the account is limited or the CLI
# cannot be reached. One cheap call before a wave beats discovering a limit with a fleet in flight.
#
# FAILS CLOSED, AND SAYS WHY. An undefined probe_limit made `! probe_limit` true forever, so every
# wave took the limit branch and the fleet silently never spawned. A refusal must always print its
# reason, or the next person reads "usage limit" and believes it.
probe_limit() {
  local out rc
  out=$(timeout -k 5 180 claude -p "reply with the single word ok" \
        --model "${PROBE_MODEL:-haiku}" --permission-mode dontAsk 2>&1)
  rc=$?
  case "$out" in
    *"usage limit"*|*"Usage limit"*|*"session limit"*|*"weekly limit"*|*"rate limit"*)
      echo "limit_guard: probe says the account is limited: ${out%%$'\n'*}"; return 1 ;;
  esac
  if [ "$rc" -ne 0 ] || [ -z "$out" ]; then
    echo "limit_guard: probe could not reach the CLI (rc=$rc): ${out%%$'\n'*}"; return 1
  fi
  return 0
}
