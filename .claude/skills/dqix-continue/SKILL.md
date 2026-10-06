---
name: dqix-continue
description: Pick up the DQIX decomp work in a FRESH session after the previous one hit a usage limit, died, was abandoned, or simply got long — kill orphans, read the real state off disk, repair what the dead session left behind, then re-enter the standing plan. Use when the user says "continue", "pick up where it left off", "the session ran out", or invokes /dqix-continue.
---

# DQIX continue

The previous session is gone and will not be resumed. Everything you need is on disk. **Do not
reconstruct state from a conversation summary or from memory files** — memories record what was true
when they were written, and the last session may have died mid-address.

    SP    the kit root (this session's working directory)
    REPO  $DQIX_REPO, or ../dqix-decomp

**Nothing that matters may live in a path something else clears** — not `%TEMP%`, not
`$REPO/build` (`ninja -t clean`), not a per-session scratchpad. If a script still points at a
`%TEMP%` path, it is pointing at nothing.

## 1. Stop the bleeding first

**Check `git log` before killing anything** — another session may be committing to
`decomp-matching`. A bare `claude.exe` is an interactive session someone may be using; only a
`claude.exe` with ` -p ` in its command line is a worker of ours.

    bash $SP/fullstop.sh          # spend dies now; CPU-only jobs are left to finish
    bash $SP/fullstop.sh --dry    # report both tiers, kill nothing

Tier 1 (workers, drivers, supervisors) is killed immediately and re-checked twice, because a
detached `claude.exe -p` survives a POSIX kill, gets reparented, and keeps spending with nothing
collecting its output. Tier 2 (`finish_wave`, `ov_recover`, the sweeps) costs electricity only and
is left alone — **killing an integration pass mid-flight empties `src/` and strands every matched
file it was landing.** Use `--hard` only if the user wants everything dead, then check
`git -C $REPO status --short` for mass deletions and recover with `git -C $REPO checkout -- src/`.

Verify rather than believe. Build the match pattern from variables so the query never matches its
own command line:

    powershell -NoProfile -Command "Get-CimInstance Win32_Process | Where-Object { \$_.Name -eq 'claude.exe' -and \$_.CommandLine -match ' -p ' } | Select-Object ProcessId"

Also sweep the leftovers that cost nothing but confuse every later check: detached `tail -f` /
`grep --line-buffered` monitors from dead sessions, and orphaned `colorsweep.py` children.

## 2. Establish the limit state

    cat $SP/WEEKLY_LOCKOUT        # epoch seconds when spawning may resume, or absent
    ls -la $SP/USAGE_LIMIT_STOP $SP/FLEET_STOPPED $SP/STOP_PULL 2>/dev/null

If the file is absent, read the last worker's stderr for the reset line and parse it with
`python $SP/until_reset.py "<the resets … line>"` — it prints seconds to reset, or
`-1` when the reset is days out, which is the weekly limit.

**Main-thread work is NOT free.** This session's tokens and every worker's come from one budget;
hand-matching here spends it, same as a worker. Only the local sweeps (compiles, no model) cost
nothing but CPU. Never describe or choose hand work as "free". Never spawn a worker into
a lockout — each one pays full startup and then dies on its first API call.

## 3. Verify the pipeline before trusting it

    cd $REPO
    python $SP/selfcheck.py          # every invariant holds — a red is REAL, not a standing exception
    python $SP/regress.py            # 0 failed  (--slow adds the end-to-end crack tests)
    python $SP/pipetest.py           # 0 wrong, 0 gate holes

**There are no standing reds.** If `selfcheck` is not all green, the last pipeline edit is the
suspect — do not wave it through.

**`regress.py --slow` is mandatory after any edit to `wgate.py`, `wdiff.py` or `colorsweep.py`** —
`selfcheck` goes red until it has been run, and that red is the reminder, not a fault.

`python $SP/progress.py --print` regenerates `STATE.md` (fleet, coverage, HEAD, staged work) and
prints it.

**After any rebuild, exercise the WINNING path before trusting a "0 hits" report** — stage a hit,
land a commit. Three of the four match-eaters only fire on success.

## 4. Read the real state (no spend)

**Read `$SP/OPEN_WORK.md` first.** It is the handoff: which idioms are cracked and which colorsweep
rule encodes each one, which residues are open with the experiments already disproven, and what the
last session was in the middle of. Process lists and coverage tell you what was running; only this
file tells you what was being *worked on*.

    python $SP/progress.py --print                # THE state: fleet, bytes+funcs, staged, health
    git -C $REPO log --oneline -8
    git -C $REPO status --short | head -20
    tail -3 $SP/wlog/pull_all.log                 # what the dispatcher was doing when it died
    cat $SP/wlog/repair_verdicts*.txt | head      # the open residues, per address

`progress.py` rewrites and prints `$SP/STATE.md`: fleet counts (parent-aware, so a forked sweep is
not miscounted as a second driver), coverage in BYTES as well as functions, remaining work per size
band, HEAD, staged-but-uncommitted files, selfcheck/regress, last 12 verdicts. `pull_all.sh` also
refreshes it every 120s. Check its `Generated` timestamp if it looks stale.

**Never hand-type a baseline into this file** — `selfcheck.py` fails on a coverage percentage, a
matched/total count or a HEAD hash in any `dqix-*` skill. Bytes, not functions, mean "how much of the
game is decompiled"; they diverge by more than half, so a run of small matches moves the headline
number and little else.

### The standing job

**One function at a time, round-robin through the large tiers.** An agent takes the next address,
works it to a gate verdict, lands it or records the residue, and moves on; the main thread cracks
and automates whatever idiom is blocking. `/dqix-plan` Phase 1 and Phase 4 are the loop. No worker
starts without the user asking in this session.

**Before any paid effort on a function, strip every construct the developers could not have written
— a pragma above all — and work that source**, even when it gates worse. `/dqix-evolve` is a
population search scored by `pad/evo_score.py`, which rejects pragma files.

The tooling that came out of the big-function work is general and worth reaching for on any function:
`pad/casegrid.py` (sweep one region's C against the real function), `pad/findshape.py` /
`pad/findladder.py` / `pad/shapecat.py` (find committed sources whose ROM code already has a shape
you cannot produce — their C is the answer), `pad/probe_cc.py` (compile and disassemble a ten-line
probe), `pad/bytemap.py` (attribute a BYTEDIFF per region), `pad/romdis.py` (disassemble unsplit
code). Three habits paid for themselves: **mine the corpus before guessing**, **bisect from a
known-good function rather than guessing**, and **re-measure every probe answer against the real
function** — a probe that omits a case's tail gives a winner that makes the case worse.

## 5. Repair what the dead session left

In this order:

1. **Emptied `src/`.** If a `finish_wave` was killed mid-flight, `git -C $REPO status` shows mass
   deletions. `git -C $REPO checkout -- src/` before anything else.
2. **Stale wave lock.** `$SP/wave.lock` is a directory holding the owner's pid. `finish_wave` clears
   a lock whose owner is dead, but verify: `cat $SP/wave.lock/pid` and check that pid is alive.
3. **Stale claims.** A killed worker leaves its address claimed. `python $SP/claim.py <mod> --status`;
   release with `--release <addr>`.
4. **Matched-but-uncommitted source.** Anything in `$SP/staging/*/` is stranded work that only a
   commit makes safe. Land it with `bash $SP/integrate_fast.sh` — as a background call, since a
   foreground one is killed at 10 minutes and can empty `src/`.

A half-finished attempt file in `attempts/` is **not** damage — it is the prior the sweeps read.
Leave it.

## 6. Re-enter the plan

Read `/dqix-plan` (`$SP/.claude/skills/dqix-plan/SKILL.md`) and rejoin at the phase the disk says you
are in. The order there is: the big function in this thread, the free sweeps in the background, and
every crack turned into a colorsweep rule. **Paid workers are off unless the user asks for one in
this session.**

Bring the watches back with anything you start — a fleet with no alerting monitor is how a stuck run
survives a whole session unnoticed:

* `$SP/health.sh` — the alerting fleet monitor (stall, zero-yield wave, hung worker, cost per match).
* `bash $SP/leverwatch.sh --once` as a BACKGROUND Bash (`run_in_background`), never under Monitor.
  It blocks until a lever nobody has promoted appears — a worker's `levers.tsv` row, or a LANDED
  evolve board whose address `core.md` does not cite — prints it once, and exits; re-arm it after it
  fires. Under Monitor it only ever idles out every 30 minutes, a wasted turn each time. On a
  firing, add the recipe to `core.md` CITING THE ADDRESS, or record it in
  `wlog/levers_declined.txt` with a reason. The `selfcheck` invariant "every captured lever has
  reached the doc workers actually read" is red until you do.
* a monitor on `$SP/wlog/repairsweep_s*.log` for `^HIT|colorsweep closed|^done:` — free matches.
* a monitor on `$SP/wlog/integrate_fast.log` for `^DONE|^OK |RED|committed` — landings.

## 7. Report, and leave the handoff behind you

One block, terse: where the previous run stopped, what you repaired, coverage now, which phase you
re-entered, and the one decision you need from the user if any.

Before this session ends — whether it hits a limit, gets long, or is simply closed — **update
`$SP/OPEN_WORK.md`**: move every crack you landed into the CRACKED table with the rule that encodes
it, add every residue nobody has solved with the experiments already disproven, and refresh the
spend state. A long compacted session degrades instruction-following; handing off to a fresh one is
cheaper than pushing through, and that file is the only reason the next session does not start from
zero.
