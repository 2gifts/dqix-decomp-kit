# mainfix — arm9 MAIN integration: root cause, fix, proof

**VERDICT: FIXED.** `ninja check` green, `ninja sha1` OK, main delinks **4199 -> 4211** with the 12
matched main functions wired. The fix is **two lines in `config/usa/arm9/symbols.txt`**.

---

## 1. ROOT CAUSE

Not delinks.txt, not `main_N` module entries, not dsd renumbering. The failing link is a
**double-counted symbol size** in one dsd-generated gap module.

`dsd delink` emits, for each gap module, the enclosing **function** symbol *and* any `kind:data`
symbol that lies **inside** that function's address range, each carrying its own ELF `st_size`.
Those bytes are counted twice. `mwldarm` sums every symbol size per section and hard-errors when the
total exceeds the section size — with a message that names neither the symbol nor the real cause:

```
mwldarm: In section .text in file main_5.o , the sum of all symbol sizes exceed section size.
mwldarm: This is most likely cause by code generation bug in compiler/assembler.
```

Exactly **two** such nested symbols exist ROM-wide (verified across all 2610 delinked objects and all
36 `symbols.txt` files) — the 256-byte reciprocal tables compiled *inline* inside the hand-written
soft-float routines:

| symbol | size | lives inside | function range |
|---|---|---|---|
| `.L_0200c274 kind:data(byte[256])` | 0x100 | `_fdiv` | 0x0200c1c0 + 0x3b8 |
| `.L_0200d484 kind:data(byte[256])` | 0x100 | `_ddiv` | 0x0200d34c + 0x544 |

**Why it never fired before.** At HEAD the whole low `.text` is a *single* gap module,
`main_828.o` = `0x02000000 .. 0x0200f398`, and it contains the secure-area blob plus SWI stubs, i.e.
a large run of bytes that belong to **no** symbol. Measured at HEAD:

```
main_828.o .text size=0xf398  symsum=0xed9a  slack=+0x5fe   (203 symbols, GetCRC16 .. func_0200f374)
```

0x5fe of slack absorbs the 0x200 double-count, so the sum stays under the section size.

**Why the 12 functions broke it.** Six of the twelve live in that blob. Delinking
`func_02009668` (0x4) and `func_0200d890` (0x4) split it, and the middle piece —

```
main_5.o .text size=0x4224  symsum=0x43dc  slack=-0x1b8   (0x0200966c .. 0x0200d890)
```

— is 100% attributed code (only 0x48 of alignment padding), so the 0x200 double-count has nowhere to
hide. Note `0x0200d890` is exactly `_ddiv`'s end and `0x0200966c` is exactly `func_02009668`'s end:
the new module is bounded by two of the new delink entries, and it swallowed both tables.

Also note **it is `main_5` by coincidence** — module numbers are assigned in address order over the
current gap set, so they renumber on every delink change. `main_5.o` before the change was
`func_020103c8` (0x20 bytes); after, it is the 0x4224-byte block above. Chasing the number, as the
brief's evidence did, leads nowhere; the module's *address range* is the identifying fact.

### The brief's delinks.txt hypothesis was a red herring
`grep ov004_ config/usa/arm9/overlays/ov004/delinks.txt` hits 50 lines, but they are ordinary
per-source delink entries for files named `src/Combat/Overlay_4/func_ov004_<addr>.cpp` (real,
committed, decompiled C++ that kept its raw symbol name). They are **not** `ov004_N` raw-module
entries. `main_N` / `ov0NN_N` gap modules never appear in any delinks.txt — dsd synthesises them from
the gaps. Main needs no equivalent. Structurally main and an overlay are identical here.

---

## 2. THE FIX — `config/usa/arm9/symbols.txt`, 2 lines

```diff
 _fdiv kind:function(arm,size=0x3b8) addr:0x0200c1c0
-.L_0200c274 kind:data(byte[256]) addr:0x0200c274
+.L_0200c274 kind:label(arm) addr:0x0200c274
 ...
 _ddiv kind:function(arm,size=0x544) addr:0x0200d34c
-.L_0200d484 kind:data(byte[256]) addr:0x0200d484
+.L_0200d484 kind:label(arm) addr:0x0200d484
```

`kind:label(arm)` makes dsd emit the symbol with `st_size = 0`, so the bytes are counted once (by the
enclosing function) instead of twice. It is not an invented kind — `.L_0200d158 kind:label(arm)`
already sits inside `_u32_div_f` two lines above, and `label` is used 6x across the configs.

**This is a general fix, not a patch for these 12 functions.** After it, **no symbol anywhere in the
ROM is nested inside another sized symbol**, so `sum(symbol sizes) <= section size` now holds for
*every* dsd module under *any* future delink split. The whole low-`.text` region is unblocked.

**Cost:** if anyone re-runs `dsd rom extract`/disassembly, those 512 bytes render as garbage ARM
instructions instead of `.byte` tables in `build/usa/asm/main/*.s`. Nothing in `build.ninja` produces
those `.s` files (they are a one-off July-16 artifact; no ninja rule targets `build/usa/asm/`), the
build never reads them, and `_fdiv`/`_ddiv` are already-named libc soft-float nobody will decompile.
Verified byte-neutral: `dsd check modules`, `dsd check symbols` and `sha1` all pass unchanged.

---

## 3. PROOF

### 3a. Reproduced the failure exactly
12 `.cpp` copied into `src/Combat/Main/`, then:
```
$ python SP/integrate_main.py
[main] base=0x2000000  12 pending files
integrated 12; 0 failed
this-pass verdicts 12: {'MATCHED': 12}
$ python tools/configure.py usa --no-extract && ninja delink && ninja check
mwldarm: In section .text in file main_5.o , the sum of all symbol sizes exceed section size.
Errors caused tool to abort.
ninja: build stopped: subcommand failed.
ninja check EXIT=1
```

### 3b. Falsified the obvious fix first
**Deleting** the two `.L_` lines from symbols.txt does **not** work — dsd re-derives them from its own
inline-table analysis and emits `data`/0x100 anyway (`main_5.o` still `symsum=0x43dc`). Only an
explicit `label(arm)` entry overrides the analysis. Worth knowing: symbols.txt overrides dsd's
analysis, absence does not.

### 3c. Green with the fix
```
$ ninja delink && python SP/modsize_check.py
modsize_check: OK -- 2610 delinked objects, 0 sum-of-symbol-sizes violations
$ ninja check
[INFO ds_decomp_cli::cmd::check::modules] Check ARM9 main: OK
[INFO ds_decomp_cli::cmd::check::modules] Check overlay 0..34: OK
[4/4] .\dsd.exe check symbols --config-path config\usa\arm9\config.yaml --elf-path build\usa\arm9.o --fail
$ ninja rom && ninja sha1
dqix_usa.nds: OK
```
```
$ git show HEAD:config/usa/arm9/delinks.txt | grep -c '\.text'   -> 4199
$ grep -c '\.text' config/usa/arm9/delinks.txt                    -> 4211      (+12)
```

### 3d. Negative control — the guard and the fix are both real
Reverting **only** the 2-line symbols.txt fix while keeping the 12 delink entries:
```
$ git checkout HEAD -- config/usa/arm9/symbols.txt && ninja delink && python SP/modsize_check.py
modsize_check: 1 VIOLATION(S) -- mwldarm will reject these at link:
  build/usa/delinks\main_5.o .text: size=0x4224 sum=0x43dc over by 0x1b8
    double-counted (nested) symbols: ['.L_0200c274', '.L_0200d484']
modsize_check EXIT=1
$ ninja check   -> mwldarm: ... sum of all symbol sizes exceed section size.  (EXIT=1)
```
So the fix is load-bearing (removing it re-reds the tree) and the guard predicts the link failure
exactly, naming the module and both culprits.

### 3e. ROM-wide sweep
`python SP/modsize_check.py --nested` over all 2610 delinked objects: **0 nested sized symbols,
0 violations.** A scan of all 36 `symbols.txt` for `kind:data`/`kind:label` entries falling inside a
`kind:function` range finds 6 total, of which only these 2 ever had a nonzero size. The class of
failure is closed, not merely dodged.

---

## 4. HARDENING (all new/own files — no shared overlay script touched)

| file | change |
|---|---|
| `SP/modsize_check.py` | **NEW.** Pure-ELF preflight: scans `build/usa/delinks/*.o`, exits 1 on any `sum(symbol sizes) > section size`, printing the module, the overage and the nested symbols. `--nested` lists latent mines. Turns the cryptic mwldarm abort into a one-line diagnosis. |
| `SP/integrate_main.py` | Re-applies the 2-line fix **idempotently** on every run, with the full explanation in-comment. Necessary because `run_main.sh` does `git checkout HEAD -- config/` immediately before calling it, which would revert an uncommitted fix. Verified: re-ran on an already-wired tree, printed `inline-table fix re-applied`, 0 pending files, tree stayed green. |
| `SP/run_main.sh` | Inserted `ninja delink` + `modsize_check.py` preflight between `configure.py` and `ninja check`, logging to `SP/wlog/modsize_main.log`; plus a note at the `git checkout HEAD -- config/` line. (Script remains otherwise unexecuted, as before.) |

**ACTION FOR THE ORCHESTRATOR:** I am not permitted to commit. The symbols.txt fix **must be committed
together with (or before) main's delinks.txt entries** — delinks without the fix is a red tree. Until
it is committed, `integrate_main.py` re-applies it on every wave, and `run_main.sh` commits
`config/usa/arm9/symbols.txt`, so the first main wave will carry it in.

---

## 5. WHAT I REJECTED, AND WHY IT MATTERS

**Delink-placement workaround (keep the module unsplit).** Feasible but crippling, and worth stating
because it is what a future investigator would otherwise land on. Measured unattributed ("slack")
bytes in main `.text` total 0x90e, and they are almost all in one place:

| region | slack |
|---|---|
| `0x02000000-0x02000800` secure-area blob interleaved with SWI thumb stubs | 0x7b6 |
| `0x02000ba0-0x02000c9c` | 0xfc |
| alignment padding at `0x0200b5f0`/`0x0200c1b4`/`0x0200ca98`/`0x0200d334` | 0x48 total |
| everything else in main `.text` | ~0 |

`_fdiv` and `_ddiv` need 0x200 of slack in whatever module holds them, and every slack source is
*below* `_fdiv`. Available above 0x02000800 is only 0x144 — not enough even for one table. So without
the symbols.txt fix the rule would have been: **no delink entry may begin anywhere in
`0x02000000 .. 0x0200d88f`**, killing 6 of the 12 matched functions and permanently fencing off the
low 55KB of `.text`. The 2-line fix removes the constraint entirely.

**Other rejected options:** deleting the symbols (dsd re-derives them, §3b); `kind:data(byte[N])` with
a smaller N (still a lie about the size *and* makes the disassembler render the rest as code — strictly
worse than `label`); splitting `_fdiv`/`_ddiv` into two function symbols each so everything tiles
(honest, but invasive: it redefines two libc symbols and risks `check modules`); upgrading dsd (v0.6.0
is pinned in `build.ninja` and shared with the whole overlay fleet).

---

## 6. FINAL REPO STATE — GREEN, 12 LEFT WIRED

`ninja check` OK · `ninja sha1` `dqix_usa.nds: OK` · `modsize_check` OK · main delinks 4211.

```
 M config/usa/arm9/delinks.txt      (+60 lines: 12 delink entries)
 M config/usa/arm9/symbols.txt      (2 lines: the inline-table fix)
?? src/Combat/Main/AbsInt020017a4.cpp
?? src/Combat/Main/BlankFunction02000b98.cpp
?? src/Combat/Main/BlankFunction02000b9c.cpp
?? src/Combat/Main/BlankFunction020017bc.cpp
?? src/Combat/Main/BlankFunction02009668.cpp
?? src/Combat/Main/BlankFunction0200d890.cpp
?? src/Combat/Main/BlankFunction0203af44.cpp
?? src/Combat/Main/ForwardIfNonNull02001728.cpp
?? src/Combat/Main/ForwardIfNonNull0200edb4.cpp
?? src/Combat/Main/GetFieldValue020457e0.cpp
?? src/Combat/Main/GetGlobalStruct02012fe4.cpp
?? src/Combat/Main/ReturnZero020c7dc4.cpp
```

Uncommitted (per the no-git-write rule) but **green and coherent**: the 12 sources, their delink
entries and the enabling symbols.txt fix are all present together. No git write, no overlay config
touched, no shared script edited. `git stash`-ing or committing this set as one unit is safe; keeping
delinks.txt while dropping symbols.txt is **not** — that is exactly the red state of §3d.
