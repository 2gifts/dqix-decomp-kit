#!/bin/bash
# How many DQIX jobs are actually alive. Prints a count, or `--list` for pid + command line.
#
#   bash psq.sh              # count of integration/sweep/worker processes
#   bash psq.sh --list       # one line each
#   bash psq.sh --kind work  # workers only (a claude.exe with ` -p `)
#
# EVERY AD-HOC `Get-CimInstance ... -match 'finish_wave'` MATCHES ITS OWN QUERY. The command line of
# the powershell process running the query contains the pattern text, and so do the two bash shells
# wrapping it, so a check for "is anything running" answers "yes, 4" against an idle machine. That
# read as a live integration for 36 minutes on 2026-09-09 while nothing at all was running. The fix
# is not a cleverer pattern -- it is excluding the querying process and its own ancestors by PID.
KIT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
SP="$(python "$KIT/kitpaths.py" state)"
MODE="${1:---count}"
KIND="${3:-all}"
[ "$1" = "--kind" ] && { KIND="$2"; MODE="--count"; }

case "$KIND" in
  work) PAT='pull_worker\.sh|pull_all\.sh|supervise\.sh|run_all\.sh' ;;
  job)  PAT='integrate_fast\.sh|integrate_all\.sh|finish_wave\.sh|ov_recover\.py|repairsweep\.py|colorsweep\.py|proppurge\.py|transmassive\.py' ;;
  *)    PAT='pull_worker\.sh|pull_all\.sh|supervise\.sh|run_all\.sh|integrate_fast\.sh|integrate_all\.sh|finish_wave\.sh|ov_recover\.py|repairsweep\.py|colorsweep\.py|proppurge\.py|transmassive\.py' ;;
esac

powershell.exe -NoProfile -Command "
  \$all = Get-CimInstance Win32_Process
  \$byid = @{}; foreach (\$q in \$all) { \$byid[[int]\$q.ProcessId] = \$q }
  \$self = @{}; \$self[\$PID] = 1; \$c = \$PID
  for (\$i = 0; \$i -lt 12; \$i++) {
    if (-not \$byid.ContainsKey(\$c)) { break }
    \$self[\$c] = 1; \$c = [int]\$byid[\$c].ParentProcessId }
  \$hit = @(\$all | Where-Object {
    -not \$self.ContainsKey([int]\$_.ProcessId) -and
    \$_.CommandLine -match '$PAT' -and
    \$_.CommandLine -notmatch 'Win32_Process|psq\.sh' })
  if ('$MODE' -eq '--list') {
    \$hit | ForEach-Object {
      \$c2 = \$_.CommandLine; if (\$c2.Length -gt 100) { \$c2 = \$c2.Substring(0,100) }
      '{0}  {1}' -f \$_.ProcessId, \$c2 }
  } else { \$hit.Count }" 2>/dev/null | tr -d '\r' | sed '/^$/d'
