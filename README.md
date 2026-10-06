# dqix-decomp-kit

Tools for matching Dragon Quest IX (Nintendo DS, USA) functions to byte-exact C++ in
[ZevyaDev/dqix-decomp](https://github.com/ZevyaDev/dqix-decomp), branch `decomp-matching`. Usable by
hand, from a single AI session, or as an autonomous fleet of Claude Code workers.

The kit compiles a candidate with the build's own `mwccarm` and flags, compares it against the
original ROM bytes, names the class of whatever still differs, applies meaning-preserving rewrites,
and lands matches through one serialized, gated path that commits to the decomp.

## Platform

Tested on Windows 11, Git Bash, Python 3.10. The per-function tools are plain Python but run the
decomp's `mwccarm.exe` directly; untested on any other system. The fleet scripts list processes with
PowerShell `Get-CimInstance` and run only on Windows.

## Quickstart

Clone both repositories side by side. The kit finds the decomp at `../dqix-decomp`, or at
`$DQIX_REPO`.

    git clone -b decomp-matching https://github.com/<you>/dqix-decomp.git   # your fork of ZevyaDev/dqix-decomp
    git clone https://github.com/ZevyaDev/dqix-decomp-kit.git

Build the decomp once. Supply your own base ROM and place it as the decomp README says
(`extract/baserom_dqix_usa.nds`).

    cd dqix-decomp
    python -m pip install -r tools/requirements.txt ninja
    python tools/configure.py usa
    ninja min

Initialise the kit:

    cd ../dqix-decomp-kit
    python -m pip install capstone pyelftools
    python kit_init.py
    python selfcheck.py

Match one function. Module is `main` or a 3-digit overlay; addresses are 8 hex digits without `0x`.

    export SP="$(pwd -W 2>/dev/null || pwd)"
    python claim.py 017 --peek 5                    # next candidates in overlay 017; claims nothing
    A=021bb1a4                                      # one of them
    F="$SP/wip/ov017/$A.cpp"; mkdir -p "$SP/wip/ov017" "$SP/staging/ov017"
    python scaffold.py 017 $A "$F"                  # callees and data resolved, a stub to fill in
    python wlist.py 017 $A                          # the target, decoded from the ROM
    python wgate.py 017 $A "$F"                     # MATCH, or RESIDUE <CLASS> <metric> <detail>
    python wdiff.py 017 $A "$F"                     # only the differing instructions
    python colorsweep.py 017 $A "$F" --apply        # mechanical rewrites, stops on MATCH
    cp "$F" "$SP/staging/ov017/$A.cpp"
    bash finish_wave.sh 017                         # integrate, ninja check, commit, push

`wgate.py` and `wdiff.py` change into the decomp directory, so source paths must be absolute.
`finish_wave.sh` reverts uncommitted edits to tracked files under the decomp's `include/`, `config/`
and `src/` before it starts; commit your own changes first. [docs/WORKFLOW.md](docs/WORKFLOW.md)
explains every step.

## Layout

| path | contents |
|---|---|
| `wgate.py` `wdiff.py` `wlist.py` `scaffold.py` `residue.py` | the per-function gate, diff, listing and starting file |
| `colorsweep.py` `presweep.py` `vtry.py` `symfix.py` `fixundef.py` `autorepair.py` | mechanical rewrites and repairs |
| `claim.py` `poolsize.py` `nearmiss.py` `resumable.py` `sdkident.py` `dqtool.py` | choosing work |
| `finish_wave.sh` `integrate_fast.sh` `ov_recover.py` `integrate.py` `classify.py` `dataown.py` `countfix.py` | landing |
| `pull_all.sh` `pull_worker.sh` `supervise.sh` `health.sh` `fullstop.sh` `killfleet.sh` | the fleet |
| `selfcheck.py` `regress.py` `pipetest.py` | invariants, regression and gate behaviour tests |
| `worker_src/core.md` `worker_src/deadends.md` | the matching guide and per-address dead ends |
| `pad/` | diff, probe and search tools; `pad/renum/` compiler-numbering tools |
| `frida/` | Frida forcing of the compiler's colouring and scheduling decisions |
| `priors/` | the closest saved attempt per unmatched address, indexed in `priors/INDEX.tsv` |
| `refs/VERIFIED.txt` | reference DS decompilations to clone into `refs/` |
| `naming/` | the naming pass for matched functions; start at `naming/HANDOFF.md` |
| `merge_human.sh` `merge_port/` | upstream merges and API ports |
| `archive/` | finished experiments cited as evidence; see `archive/README.md` |
| `INVENTORY.md` | one line per script |
| `OPEN_RESIDUES.md` `REGALLOC_FINDINGS.md` `inv/*_FINDINGS.md` | recorded findings |
| `.claude/skills/` `.claude/workflows/` | Claude Code skills and workflows |

`kit_init.py` creates the state directories: `wip/`, `staging/`, `handwork/`, `wlog/`, `gated/`,
`clsbest/`, `claims/`, `scaffold/`, `doc_cache/`, `attempts/`, `quarantine/`, `refs/`.

## Docs

- [docs/SETUP.md](docs/SETUP.md) — prerequisites, environment, reference decomps, Frida
- [docs/WORKFLOW.md](docs/WORKFLOW.md) — one function from address to commit
- [docs/FLEET.md](docs/FLEET.md) — the autonomous pipeline, knobs, stopping, cost
- [docs/LESSONS.md](docs/LESSONS.md) — compiler facts and pipeline rules learned the hard way
- [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md) — sending matches and tool fixes back

## Claude Code

Opening Claude Code in the kit root loads [CLAUDE.md](CLAUDE.md), the skills `/dqix-plan`,
`/dqix-continue`, `/dqix-status`, `/dqix-stop`, `/dqix-hand-match`, and the workflows `dqix-crack` and
`dqix-evolve`.
