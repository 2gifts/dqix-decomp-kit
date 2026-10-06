# mainsup — EXTENDING THE PIPELINE TO arm9 MAIN

## 1. VERDICT: **YES — the pipeline extends to main.**

Main is not structurally different from an overlay. Same compiler flags, same ARM ABI, same reloc
model, same recipes. Every difference is a **path / prefix / section-map** difference, and all of them
are now handled. The per-function gate, the wave generator and the integrator all work on main today.

Proof: **12 previously-unmatched main functions matched byte-exactly**, and the gate itself validated
against **150/150 already-committed main functions**.

---

## 2. WHAT I BUILT (all NEW files — no shared script was edited)

| file | status |
|---|---|
| `SP/wgate_main.py` | **verified** — `python SP/wgate_main.py <addr> <file>` → `MATCH` or exact failure |
| `SP/genwave_main.py` | **verified** — `python SP/genwave_main.py [per=12] [maxnb=32]` → `SP/wave_main/bN.txt` |
| `SP/integrate_main.py` | **verified via `--dry`** — `python SP/integrate_main.py [--dry] [--srcdir=DIR]` |
| `SP/worker_main.md` | worker doc — a **delta** over `worker_ov_all.md`, not a copy (so recipe updates propagate) |
| `SP/run_main.sh` | **UNVERIFIED, never executed** — needs exclusive `ninja check`; see §6 |
| `SP/inv/mainsup/*.cpp` | the 12 matched functions (in my dir, NOT in `src/`) |
| `SP/inv/mainsup/selftest.py` | the 150-function gate correctness proof |
| `SP/inv/mainsup/dis.py` | `python dis.py <addr>…` — dumps a main function's target disasm |

I ran **no** `ninja check`, `ov_recover.py`, `integrate_ov.py`, `tools/configure.py`, or git write.
`git status --porcelain` in the repo is **empty** after all of the above.

---

## 3. EVIDENCE

### 3a. The gate is correct — 150/150 committed main functions
`python SP/inv/mainsup/selftest.py 150` samples committed **single-function** main `.cpp` files from
`config/usa/arm9/delinks.txt` (seeded shuffle, 4193 eligible), copies each out of `src/`, and gates it.
These are in the shipped ROM and byte-exact by construction, so anything but `MATCH` is a gate bug.

```
eligible single-func committed main files: 4193 delinks -> sampled 150
MATCH 150 / 150
```

### 3b. The gate is not a rubber stamp — every failure path fires
Deliberately-wrong sources in `SP/inv/mainsup/neg/`:

| injected defect | gate output |
|---|---|
| right bytes, **wrong data symbol** (`data_020fefec` for `data_020fb3f0`) | `RELOC-WRONG … first [('0x8','data_020fefec','orig->0x20fb3f0')]` |
| offset `0x954`→`0x958` | `BYTEDIFF: 1 bytes differ at ['0x0']` |
| nonexistent callee, size-exact | `UNDEF-SYM: ['data_deadbeef']` |
| static helper defined alongside | `SIZE/OVERGEN: 2 .text total=0x10 slot=0x8` |
| function name containing `0x` | `BAD-NAME: … breaks dsd delinking` |
| addr `0x020e5920` (`.init`) | `NON-TEXT-SECTION: … cannot be delinked. SKIP` |

`RELOC-WRONG` firing matters most: the byte-compare **masks** `bl`/pool words, so without it a
function that calls the wrong callee looks matched and then fails `dsd check modules` at link.

### 3c. 12 real unmatched main functions matched (task asked for 3)
All re-gated in one final pass; all `MATCH`. Chosen smallest-first, then deliberately extended to
cover each reloc type so the whole path — not just the trivial tail — is exercised.

| addr | size | file | what it proves |
|---|---|---|---|
| `02000b98` `02000b9c` `020017bc` `02009668` `0200d890` `0203af44` | 0x4 | `BlankFunction*.cpp` | plain path, no relocs |
| `020c7dc4` | 0x8 | `ReturnZero020c7dc4.cpp` | — |
| `020457e0` | 0x8 | `GetFieldValue020457e0.cpp` | field load |
| `020017a4` | 0xc | `AbsInt020017a4.cpp` | if-conversion (`rsblt`) |
| `02012fe4` | 0xc | `GetGlobalStruct02012fe4.cpp` | **R_ARM_ABS32** pool `.word` → `.bss`, reloc target verified |
| `02001728` | 0x14 | `ForwardIfNonNull02001728.cpp` | **R_ARM_CALL** `bl func_0200ab10`, reloc target verified |
| `0200edb4` | 0x14 | `ForwardIfNonNull0200edb4.cpp` | **R_ARM_CALL** `bl func_02001728` |

Those 6 blanks are **the entire `bx lr` family in main** — the census in §4 finds exactly 6. That
seam is now closed.

One honest SKIP, worked to the worker's 5-variant limit: `func_02001578` (0x24). Target allocates
`ldr r1,<pool>; mov r0,#1; str r0,[r1,#0xc]`; mwcc gives me the two scratch registers swapped. Five
forms (`char*` arithmetic, `int[]` index, struct member, named pointer local, value shared with the
preceding call argument) all produce the **identical** 4-byte diff at `0xd/0x11/0x15/0x16`. That is
exactly recipe #11's "scratch-register decl order is a no-op" — no C lever. SKIP.

### 3d. The integrator writes exactly the right thing
`python SP/integrate_main.py --srcdir=SP/inv/mainsup` (a non-default `--srcdir` forces `--dry`) over
all 24 `.cpp` in my directory — 12 good, 1 honest BYTEDIFF, 4 failed variants of it, 7 negative controls:

```
[main] base=0x2000000  24 pending files  (DRY RUN)
BYTEDIFF InitAndNotify02001578.cpp @0x2001578: 4 bytes differ at ['0xd','0x11','0x15','0x16'] — SKIP
DUP-ADDR neg/badname.cpp @0x020c7dc4: already claimed by ReturnZero020c7dc4.cpp — SKIP
NON-TEXT neg/initsec.cpp @0x020e5920: .init — SKIP
… (4 more try/ variants BYTEDIFF, 5 more neg/ DUP-ADDR)
integrated 12; 12 failed
this-pass verdicts 17: {'MATCHED': 12, 'BYTEDIFF': 5}
```
Exactly the 12 good files were accepted and every bad one rejected. The `neg/` controls trip
`DUP-ADDR` here rather than their individual defect, because the good file for the same address sorts
first — each of those defects was proven separately at the gate level in §3b.

Replaying the wire-up code against the **real** config files, writing to my own directory:

```
detected line endings: symbols.txt='\n'  delinks.txt='\r\n'
symbols.txt  orig 555235B -> new 555235B (delta +0)  CRLF-preserved=True
delinks.txt  orig 451594B -> new 451912B (delta +318)  CRLF-preserved=True
```
`diff` on symbols.txt: **empty** (universal keep-raw means the symbol name never changes — same as
overlays). `diff` on delinks.txt: **exactly** the intended new entries, nothing else.

---

## 4. THE POOL — corrected numbers

The brief's figures (6208 / 4197 delinked / 2011 unmatched / 1243 ≤256B) are **stale**; ~500 more
functions have landed since. Measured now, by `genwave_main.py`:

```
main: 1452 unmatched (745 <=256B)
excluded: 7 delinked, 0 skiplisted, 0 held-verified, 42 outside .text
```
Full picture: 6208 total function symbols · 4699 already named · 1509 raw `func_` (1501 ARM + 8 thumb)
· minus 42 outside `.text` · minus 7 already inside a delinked range = **1452 servable**.

Size bands and the known-dead families inside them:

| band | count |
|---|---|
| `<32B` | 60 |
| `32–63B` | 55 |
| `64–127B` | 129 |
| `128–256B` | 501 |
| **≤256B total** | **745** |
| `>256B` | 707 |

Dead families already inside that 745 (worker_main.md tells workers to skip on sight):
- **6** pure `bx lr` blanks — **all 12 matched here include these 6, so this family is now empty**
- **3** lone `ldmia sp!, {…, pc}` — mis-split function tails, no C form
- **13** `ldr ip,<pool>; bx ip` tail-call thunks — unreachable under `-inline noauto`

So the realistic servable small band is ~723. Still comparable to all 35 overlays combined (2137 by
the brief's count), and it is **the largest single untapped module**.

---

## 5. WHAT `run_overlay.sh` / `run_all.sh` NEED TO TREAT "main" AS A TARGET

Main is not an overlay-shaped target: it has no `OV` number, no `DEC`, no `Overlay_<DEC>` dir, no
`overlays/ovNN/` config, and no per-overlay `.bin`. Threading it through `run_overlay.sh` as `OV=main`
would need edits in ~14 places. `SP/run_main.sh` (new file, unexecuted) is the concrete answer; the
exact substitutions it makes are:

| run_overlay.sh | main equivalent |
|---|---|
| `DEC=$((10#$OV))`, `src/Combat/Overlay_$DEC/` | drop `DEC`; `src/Combat/Main/` |
| `config/usa/arm9/overlays/ov$OV/delinks.txt` | `config/usa/arm9/delinks.txt` |
| `genwave_direct.py $OV 12 $NB` | `genwave_main.py 12 $NB` (**no OV arg**) |
| `$SP/wave_ov$OV/b*.txt` | `$SP/wave_main/b*.txt` |
| worker prompt → `worker_ov_all.md` w/ OV substitution | `worker_main.md` (delta doc) then `worker_ov_all.md` |
| `wgate.py $OV <addr> <file>` | `wgate_main.py <addr> <file>` (**no OV arg**) |
| `finish_wave.sh $OV` → `ov_recover.py $OV` | `integrate_main.py` + one `ninja check` (**no bisection** — §7) |
| `grep -c '\.text'` on the overlay delinks | `grep -c '\.text start'` (main's delinks header lines also contain `.text`, so the bare pattern over-counts by 1 and, worse, its `align:` header line makes the BEFORE/AFTER delta wrong) |
| skiplist `skiplist_ov.txt`, strike files `strike_${OV}_${a}` | `skiplist_main.txt`, `strike_main_${a}` |
| `wlog/{gen,integ,served,done,trusted}_${OV}.txt` | same names with `main` |
| `ov${OV}_stage`, `hold_ov${OV}` | `main_stage`, `hold_main` |
| quarantine "other overlay dirs" preflight | quarantine untracked `.cpp` **outside** `src/Combat/Main/` |

For `run_all.sh` the change is smaller: its ranking block globs
`config/usa/arm9/overlays/ov*/symbols.txt`, which is precisely why main was never processed. Give main
a row of its own — its unmatched-≤256B count (745) would put it **first** in the ranking, ahead of
every overlay — and dispatch `run_main.sh` instead of `run_overlay.sh` when that row wins. Main needs
its own cooldown file (`cooldown_main`); everything else in `run_all.sh` (lockfile, coverage probe,
usage-limit sentinel, dead-pass counter) is module-agnostic and needs no change.

---

## 6. GOTCHAS WHERE MAIN GENUINELY DIFFERS FROM AN OVERLAY

These are the things that will silently produce wrong results if ported naively.

1. **`config/usa/arm9/delinks.txt` is CRLF; `config/usa/arm9/symbols.txt` is LF.** Overlay configs are
   uniformly CRLF. `integrate_ov.py` rewrites in Python text mode, which on Windows would convert
   main's symbols.txt wholesale to CRLF — a 7928-line phantom diff on a file other agents share.
   `integrate_main.py` detects and preserves each file's own ending (verified byte-exact, §3d).
2. **Main's delinks mix UPPER- and lowercase hex** (`.text start:0x0200FE68 end:0x0200FEA4` sits right
   next to lowercase entries). Every `[0-9a-f]` regex ported from the overlay scripts must become
   `[0-9a-fA-F]`. This is not cosmetic: `BASE=min(...)` computed with a lowercase-only regex parses
   `0x0200FE68` as `0x0200` and yields **BASE=512**, which silently offsets every byte compare.
3. **`BASE` must come from the section table, not from `min()` over all `start:` values.** The overlay
   gate's `min(STARTS)` idiom happens to work only because overlay file entries all sit above the
   section base. Both `wgate_main.py` and `integrate_main.py` parse the header lines (the ones with
   `kind:`) instead — per-file delink entries have no `kind:` field, which makes the two trivially
   separable.
4. **`.init` is real and sizeable in main** — `0x020e5920–0x020e692c`, ~4KB of genuine ARM code
   holding **42** raw `func_` symbols. In overlays this was an 18-function project-wide curiosity.
   Both the gate and the wave generator drop these explicitly.
5. **Main has legacy multi-function files.** One delink entry can cover several functions, so 7 raw
   `func_` addrs already sit *inside* a delinked range. `genwave_direct.py` excludes only range
   *starts*; `genwave_main.py` excludes anything inside a *range*. Serving one of these would collide
   at integration.
6. **Main has 8 raw THUMB functions** and ~500 already-named-but-never-decompiled SDK symbols
   (`memset`, `GetCRC16`, `_fls`, `Entry`, …). The `kind:function(arm,…)` filter handles both; don't
   loosen it.
7. **Legacy committed main names contain `0x`** (`Clear0x20BytesAt0x74de`, `GetScaledBits12To17At0x2fc`).
   The `BAD-NAME` guard correctly rejects these — it is a *new-work* rule, and those files predate it.
   They are harmless in place because universal keep-raw renames the definition to `func_<addr>` at
   integration time anyway. Don't "fix" them, and don't be alarmed when the gate rejects one.
8. **Address spaces are disjoint** — main is `0x02000000–0x021536e0`, every overlay starts at
   `≥0x021536e0`. So sharing `skiplist_ov.txt` is *safe* (no main addr can appear in it), but main
   strikes belong in `skiplist_main.txt` because overlay addrs collide with **each other**.
9. **`grep -c '\.text'` over main's delinks over-counts.** Main's section header block contains
   `.text` and `.init` lines with `align:`; use `grep -c '\.text start'`.
10. **Wave files must be written LF.** Python text mode emits CRLF on Windows; any consumer doing
    `for a in $(cat bN.txt)` then `grep "^func_$a:"` then fails silently on the trailing `\r`. I hit
    this and `genwave_main.py` now passes `newline='\n'` explicitly.
11. **`wgate.py` requires an absolute source path** (it `chdir`s to the repo first). `wgate_main.py`
    resolves the path before `chdir`, so workers can gate from their own directory.
12. **Two files claiming the same address silently overwrote each other** in `integrate_ov.py`'s
    `mangled` dict — one byte-exact match dropped with no signal, the loser left untracked in `src/`.
    `integrate_main.py` adds a `DUP-ADDR` guard. **This bug is also present in `integrate_ov.py`** and
    is worth porting back there; I did not touch it, per the shared-file rule.

Everything else is genuinely identical: `dsd check modules` validates main's module checksum exactly
as it does an overlay's, so the reloc-false-match hazard (byte-exact but wrong callee → passes the
masked per-func gate, fails at link) applies to main **identically**, and `RELOC-WRONG` in both
`wgate_main.py` and `integrate_main.py` is the same defence.

---

## 7. REMAINING GAP — the one thing not ported

**There is no `ov_recover.py` port for main.** That script provides preserve-before-git-touch,
classify (TRUSTED/RISKY/BAD), bisection over a red gate, per-chunk commits, drift parking, and the
`trusted_ovNN.txt` held-and-verified ledger. Without it, `run_main.sh` integrates **all-or-nothing**:
one reloc-false-match reds the wave and the whole wave rolls back.

That is survivable but wasteful, and it is the highest-value next task. It is not a hard port — the
overlay-specific surface is small and localised (`OV`/`DEC`, `CFG`, `SRCDIR`, the `func_ov{OV}_`
regex in `gather()`/`prune_unwired()`, the `hold_ov{OV}`/`trusted_ov{OV}`/`gate_ov{OV}` filenames,
and the commit message) — but validating it **requires** full `ninja check` runs, which are exclusive
and would have collided with the overlay fleet. I deliberately did not attempt it.

Until it exists, run `run_main.sh` with `PUSH` unset and inspect the verdict.

---

## 8. WHEN TO SKIP (main-specific — tell workers, saves whole slots)

- **addr ≥ 0x020e5920** → `.init`/rodata/data. Not delinkable. 42 such addrs. The gate refuses them.
- **lone `ldmia sp!, {…, pc}`** (size 0x4) → not a function, it is the mis-split tail of the previous
  one. No C form. 3 in the pool, and they sort to the very front of a smallest-first wave.
- **`ldr ip,<pool>; bx ip`** (size 0xc) → tail-call thunk; unreachable under `-inline noauto`
  (`return f()` emits `bl` + `bx lr`). 13 in the pool, likewise near the front.
- **scratch-register address/value swaps** (recipe #11), e.g. target `ldr r1,<pool>; mov r0,#K;
  str r0,[r1,#N]` where you emit r0/r1 swapped. Verified on `func_02001578` across 5 source forms:
  identical output every time. No C lever — SKIP on sight.
- Everything in `worker_ov_all.md`'s PROVEN-NO-C-FORM list applies unchanged.
