---
name: dqix-stop
description: Emergency full stop for the DQIX decomp — kill every driver, worker, sweep and background job, set the stop flags, and verify nothing survived. Use the moment the user says stop, halt, kill it, shut it down, "stop everything", or invokes /dqix-stop.
---

# Full stop

The user wants everything to stop **now**. Do this before answering anything, before finishing an
explanation, and before any other tool call. Do not ask what to stop or whether to save work first —
nothing here loses work: matched source stays on disk and every phase records its state in
`$SP/wlog/`.

    KIT   the kit checkout (scripts, docs, skills): $DQIX_KIT when set, else this session's
          working directory
    SP    the state directory (attempts, logs, claims, staging): `python $KIT/kitpaths.py state`
    REPO  $DQIX_REPO, or ../dqix-decomp

## 1. Stop it

    bash "$KIT/fullstop.sh"          # spend stops now; CPU-only work is left to finish
    bash "$KIT/fullstop.sh" --hard   # also kill integration and sweeps
    bash "$KIT/fullstop.sh" --dry    # report both tiers, kill nothing

It sets `FLEET_STOPPED` / `STOP_PULL` / `STOP_RESUME` first, then works in two tiers:

**Tier 1 — anything that spends tokens, killed immediately.** Headless `claude.exe -p` workers and
every driver, supervisor and sweep that launches them, all in ONE PowerShell pass rather than a
sequence — killing the supervisor first and the workers last leaves a window for the dying driver to
start one more worker. It then re-checks twice, because that race is real, and a detached
`claude.exe` survives a POSIX kill and keeps burning with nothing collecting its output.

**Tier 2 — CPU-only jobs, allowed to finish.** `finish_wave`, the integrators, `repairsweep`,
`colorsweep`, `vtry` and friends cost electricity and nothing else, and killing an integration pass
mid-flight empties `src/` and strands every matched file it was landing. The default reports them
with their pid and age and leaves them alone; locks are left in place while a job still holds them.
Use `--hard` only when the user wants everything dead, and check the repo afterwards.

It never touches interactive `claude.exe` sessions (a worker is identified by ` -p `), and it
excludes this session's own Bash tool calls, which carry the command text verbatim and would
otherwise look like drivers.

## 2. Stop what this session started

`fullstop.sh` kills processes, not your own background tasks. Also:

* Stop every background Bash task you launched this session (`TaskStop`), and any Monitor you armed.
* **`TaskStop` on a Monitor kills the task, not the process it spawned.** A `health.sh` monitor keeps
  running as a detached `bash health.sh`, and every `tail -F | grep` watcher keeps its own `tail` and
  `grep`. Kill those explicitly — they are what the user sees still running after a "clean" stop:

      powershell -NoProfile -Command "Get-CimInstance Win32_Process -Filter \"Name='bash.exe'\" | Where-Object { \$_.CommandLine -match 'health\.sh' -and \$_.CommandLine -notmatch 'shell-snapshots' } | ForEach-Object { Stop-Process -Id \$_.ProcessId -Force }"
      powershell -NoProfile -Command "Get-CimInstance Win32_Process | Where-Object { \$_.Name -in @('tail.exe','grep.exe') } | ForEach-Object { Stop-Process -Id \$_.ProcessId -Force }"

  `fullstop.sh` does NOT cover either: on 2026-08-25 it printed "TIER 1 none running / TIER 2 none
  running" while two `health.sh` monitors (one an orphan from a session that had already died) and
  eight `tail`/`grep` watchers were alive.
* Only if you used `--hard` on a running `finish_wave`: check `git -C $REPO status --short`. Mass
  deletions mean it died between clean and restore — recover with `git -C $REPO checkout -- src/`
  before reporting done. A `finish_wave` left to finish normally needs nothing.

## 3. Verify, then report

    powershell -NoProfile -Command "Get-CimInstance Win32_Process | Where-Object { \$_.CommandLine -match 'pull_all|pull_worker|resume_sweep|resume_one|run_all|supervise|run_module|finish_wave' -or (\$_.Name -eq 'claude.exe' -and \$_.CommandLine -match ' -p ') } | Select-Object ProcessId,Name"

Expect nothing. A stop is a **verified state, not a belief** — "I killed the fleet" has been wrong
before, because a detached `claude.exe -p` survives a POSIX kill, gets reparented, and keeps
spending with no driver collecting its output. If anything is still listed, run `bash
"$KIT/killfleet.sh" --all`, then check again.

**`killfleet.sh --all` only became true on 2026-08-25.** It killed `run_all`/`run_overlay`/`run_main`
— all retired on 2026-08-20 — and never `pull_all`/`pull_worker`, so the escalation this step tells
you to reach for would kill the supervisor, report success, and leave the fleet spending. It now
names the live scripts. If you are working from an older checkout of `$SP`, check that first.

**`supervise.sh` can no longer resurrect a stop.** It used to relaunch the deleted `run_all.sh` (so
auto-resume was silently dead), and `pull_all` clears `STOP_PULL` on launch — which would have undone
this whole procedure the moment the supervisor next looked. It now launches `pull_all` and refuses
while `STOP_PULL` or `FLEET_STOPPED` exists. The flags are what hold the stop; do not delete them to
"clean up".

**That command self-matches — read the command lines before believing it.** Your own Bash tool call
carries the pattern verbatim, so it lists 4-6 `bash.exe` processes every time. A real survivor has a
command line naming the script (`bash.exe pull_worker.sh 017 1 5`); this session's shells all read
`bash.exe -c "source .../shell-snapshots/..."`. Add
`-and \$_.CommandLine -notmatch 'shell-snapshots'` to the filter and also cover `health.sh`,
`tail.exe` and `grep.exe`, which the pattern above misses entirely.

Report in three lines: what was killed, what survived (ideally nothing), and whether any repo repair
was needed. Then stop working — do not resume, restart, or "finish the last one" unless the user
says so.

## Restarting later

The flags stay set, so nothing restarts on its own. A later session resumes with `/dqix-continue`,
which reads state off disk and re-enters the plan.
