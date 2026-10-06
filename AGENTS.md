# DQIX decomp kit

This directory is `$SP`, the kit root. The decomp checkout is `$DQIX_REPO` (default
`../dqix-decomp`, branch `decomp-matching`). Every script resolves both through `kitpaths.py`. In
Git Bash, `export SP="$(pwd -W 2>/dev/null || pwd)"` from this directory.

## Before any work: update the kit

The kit is maintained continuously; fixes, levers and rules land in it all the time. At the start of
every session, and again before resuming work after any pause, run:

    python kit_update.py

| exit | meaning | do |
|---|---|---|
| 0 | up to date, or fast-forwarded and `kit_init.py` re-run | re-read every file it prints as `RE-READ`, then work |
| 1 | the fetch failed (offline), or `kit_init.py` failed after an update | tell the user the line it printed; fix a `kit_init.py` FAIL before working |
| 2 | the fleet or an integration is running, so nothing was pulled | tell the user an update is waiting; never pull under a running script |
| 3 | local changes or local commits block the fast-forward | tell the user; never stash, reset or discard them yourself |

## Where things are

| need | read |
|---|---|
| state of a fresh session's work | `OPEN_WORK.md` first, then `STATE.md` (`python progress.py --print`) |
| one function, start to commit | `docs/WORKFLOW.md` |
| the fleet, knobs, stopping | `docs/FLEET.md` |
| compiler facts and pipeline rules | `docs/LESSONS.md` |
| every codegen lever | `worker_src/core.md` (1400 lines: grep a heading, never read whole) |
| forms already ruled out per address | `worker_src/deadends.md` |
| what each script does | `INVENTORY.md` |
| named residues nobody cracked | `OPEN_RESIDUES.md` (grep an address) |
| closest saved attempt per address | `priors/INDEX.tsv`, `python resumable.py <addr>` |
| naming already-matched functions | `naming/HANDOFF.md` |
| finished experiments cited as evidence | `archive/README.md` (nothing runs them) |

## Skills

`.claude/skills/` is the source of every skill. `python kit_init.py` copies the agent-neutral ones
into `.agents/skills/`, where Codex and other Agent Skills readers load them; edit the source, never
the copy.

| skill | use | agents |
|---|---|---|
| `dqix-hand-match <addr>` | close one function in this session | all |
| `dqix-status` | where things are: fleet, coverage, what moved, what needs a decision | all |
| `dqix-stop` | stop everything now and verify nothing survived | all |
| `dqix-plan` | run the standing plan: one function at a time, gate it, land it or record why | Claude Code |
| `dqix-continue` | a fresh session after a limit, a crash or a long session | Claude Code |

The fleet's workers are Claude Code sessions (`claude -p`), so running the fleet needs the Claude Code
CLI whichever agent operates the kit. Do not start the fleet, a workflow or any paid worker unless
the user asks.

## Hard rules

1. Source being matched lives in `$SP/wip/<main|ovNNN>/`; scratch in `$SP/handwork/`. Never write
   into `$DQIX_REPO/src/`. Never put scratch in `staging/`, `hold_*/`, `gated/` or `attempts/`:
   those are pipeline inputs.
2. Create and change files with your file-editing tool. Never with a shell heredoc or `sed -i`.
3. Pass absolute source paths to `wgate.py` and `wdiff.py`; they change into the decomp directory.
4. A match lands only through `staging/<main|ovNNN>/` and `finish_wave.sh <main|NNN>` (or
   `integrate_fast.sh`). Only a commit that passes `ninja check` proves a match.
5. One integration at a time. Never run `finish_wave.sh`, `integrate_fast.sh` or `integrate.py`
   (even `--dry`) while `$SP/wave.lock` exists or `claims/INTEGRATING` names a module.
6. Launch integrations as a background task with a long timeout and the command passed plain: no
   `nohup`, no trailing `&`. They run 10 minutes or more.
7. `finish_wave.sh` reverts uncommitted changes to tracked files under the decomp's `include/`,
   `config/` and `src/`. Commit what a match needs first.
8. No hand assembly except addresses in `asm_allow.txt`. No codegen `#pragma`. Nothing in
   `tools/cc_overrides.txt` or `tools/cc_flag_overrides.txt`. A match that needs one of these is a
   diagnosis: find what the source has that the ROM's did not.
9. Route on the `RESIDUE <CLASS>` line `wgate.py` prints. Run `colorsweep.py` before calling a
   register or scheduling residue stuck. Never write a function off as unmatchable.
10. Key on the address, never the `func_` name. A curated name in `symbols.txt` is binding; define
    exactly that symbol.
11. Preserve before deleting. An untracked worker file is the only copy of that attempt.
12. Never edit a running bash script; edit a copy and `mv` it over. Quiesce the fleet before editing
    any pipeline script (`touch STOP_PULL FLEET_STOPPED`, then `fullstop.sh`).
13. Never kill a DQIX process by hand. `bash fullstop.sh` (`--dry` to look, `--hard` for CPU jobs
    too), `bash killfleet.sh` to escalate. Count jobs with `bash psq.sh`, dispatchers only by
    `pull_all.pid`.
14. Verify the effect, not the patch. After any script edit: `python selfcheck.py` and
    `python regress.py`; `regress.py --slow` after touching `colorsweep.py`, `wdiff.py` or
    `wgate.py`; `python pipetest.py` after touching the gate.
15. Never fork a code path; parameterise it. A new script gets a line in `INVENTORY.md`.
16. After editing `worker_src/core.md`, run `python build_worker_docs.py` and grep the built doc for
    the new text.
17. Record work in progress in `OPEN_WORK.md` while working, not at the end. A fresh session starts
    from it.
