#!/bin/bash
# RELAUNCH SUPERVISOR. Keeps run_all.sh alive across usage-limit stops, crashes and reboots.
#
# WHY. run_overlay.sh already sleeps-and-resumes when the CLI states a parseable reset time. But when
# the reset is unparseable or beyond RESET_MAXWAIT (weekly limit, full-day exhaustion) it does the
# right thing for safety — saves the work, sets USAGE_LIMIT_STOP, and exits — and then NOTHING ever
# restarts it. That was the single largest wall-clock loss in the project: the driver stopped on
# 2026-07-29 08:18 at 68.75% and sat idle for FIVE DAYS waiting for a human to type `bash run_all.sh`.
# At the ~150 functions/day it had been averaging, that idle window cost more than any codegen blocker.
#
# run_all.sh clears USAGE_LIMIT_STOP on launch and holds run_all.lock while alive, so this loop is
# just: if it is not running, start it. The lock makes a double-launch impossible.
# Usage: bash supervise.sh    (run detached; it never exits on its own)
KIT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
SP="$(python "$KIT/kitpaths.py" state)"
REPO="$(python "$KIT/kitpaths.py" repo)"
# pull_all.sh replaced run_all.sh on 2026-08-20 and this file kept relaunching the DELETED script,
# so the whole point of the supervisor -- coming back by itself after a usage-limit stop -- had been
# silently dead ever since. `pull_all` writes its own pid file and removes it on exit, so use that
# rather than inventing a second lock.
LOCK="$SP/pull_all.pid"
LOG="$SP/wlog/supervise.log"
cd "$REPO" || exit 2
# PID FILE, not a process-name grep. Git Bash `ps` prints only the interpreter path (`/usr/bin/bash`)
# and never the script name, so `ps | grep supervise` reports DOWN for a perfectly healthy supervisor —
# which is worse than no check at all, because it invites "fixing" a second one into existence.
echo $$ > "$SP/supervise.pid"
trap 'rm -f "$SP/supervise.pid"' EXIT
echo "=== supervisor up $(date '+%m-%d %H:%M:%S') (pid $$) ===" >> "$LOG"

while true; do
  if [ -f "$LOCK" ] && kill -0 "$(cat "$LOCK" 2>/dev/null)" 2>/dev/null; then
    sleep 900; continue                      # pull_all alive — nothing to do
  fi
  # AN OPERATOR STOP IS NOT A CRASH. `pull_all` clears STOP_PULL on launch, so without this check
  # the supervisor would undo /dqix-stop the moment it next looked -- the fleet coming back from the
  # dead with nobody watching it spend. A usage-limit stop leaves these flags alone, which is
  # exactly the case this loop exists for.
  if [ -e "$SP/STOP_PULL" ] || [ -e "$SP/FLEET_STOPPED" ]; then
    sleep 900; continue
  fi
  # stale pid file from a killed/crashed run would block pull_all's own guard; clear it first
  [ -f "$LOCK" ] && rm -f "$LOCK"
  echo "$(date '+%m-%d %H:%M:%S') pull_all not running -> launching" >> "$LOG"
  bash "$KIT/pull_all.sh" >> "$SP/wlog/pull_all_stdout.log" 2>&1
  echo "$(date '+%m-%d %H:%M:%S') pull_all exited (rc=$?)" >> "$LOG"
  # Back off before relaunching. If it stopped on a usage limit the account needs time to refill;
  # relaunching immediately would spawn a wave that instantly re-hits the limit, and every truncated
  # worker pays full startup cost for zero functions. 30 min is short against a 5-hour window and
  # short enough that a crash-loop bug is visible in the log rather than silently idle for days.
  sleep 1800
done
