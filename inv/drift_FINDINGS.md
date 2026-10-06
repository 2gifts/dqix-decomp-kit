# drift — link-layout drift: mechanism, fix, detector, proof

**VERDICT: SOLVED, and it was never about the functions.** Drift is a **filename bug**, not a
codegen, delink or config bug. Every drift on record is one object whose `.text` the linker never
placed, because the lcf line that should have placed it did not resolve. Two ways that happens, both
decidable with zero builds. Both are now fixed/guarded, and **all 10 non-`.init` parked "culprits"
land green together in one build** (`e7_parked_all`).

Repo left **GREEN and CLEAN** (`ninja check` OK, `git status` empty).

---

## 1. MECHANISM

`dsd lcf` places each delinked module with exactly one line:

```
<object-basename>.o(<section>)
```

mwldarm resolves `<object-basename>.o` by **basename over the whole of `objects.txt`**, then takes
the named section out of it. If that line yields no section, the linker contributes **0 bytes** at
that point, the overlay comes up `size(function)` short, and every later symbol resolves lower —
`dsd check symbols`: `expected to be at 0xA but is at 0xB`. The dropped object's own symbol is parked
near 0, so it reports `expected 0x0216b7bc but is at 0x0` — **the culprit names itself.**

### M1 — basename collision (the one that was live and unguarded)

`ov_recover.sanitize()` named the placed file from the **first `ARM …name(` token in the file**:

```python
m = re.search(r'\bARM\b[^\n(]*?\b([A-Za-z_]\w*)\s*\(', text)
```

Workers routinely forward-declare an ARM callee above the definition, so this matched a
**declaration**, and the file was written under the **callee's** name:

| held source (its real definition) | written to disk as | collides with |
|---|---|---|
| `AllocateAndScanBits_021567f8.cpp` | `LoadBattleBlock020ac460.cpp` | `src/Combat/Main/LoadBattleBlock020ac460.cpp` |
| `DispatchByType_0216b7bc.cpp` | `SetField1c8True_02184ad8.cpp` | `src/Combat/Overlay_11/SetField1c8True_02184ad8.cpp` |
| `PopStatusSlotAndNotify_0217c094.cpp` | `TestFlag0SetAndFlag1Clear.cpp` | `src/Combat/Main/TestFlag0SetAndFlag1Clear.cpp` |
| `RepositionEntriesInRange_02160794.cpp` | `SetEntryPositionById.cpp` | `src/Combat/Main/SetEntryPositionById.cpp` |
| `InitAllocatorsAndPairTables_021a0c0c.cpp` | `RestorePairTableFromBuffer.cpp` | `src/Combat/Main/RestorePairTableFromBuffer.cpp` |
| `ApplyPendingListEntry_021b5250.cpp` | `CopyHalfwordArrayByCount.cpp` | `src/Combat/Main/CopyHalfwordArrayByCount.cpp` |

`place()` only disambiguated **within the overlay's own directory** (`tracked(bn) or bn in used or
os.path.exists(bn)`), so a clash with `src/Combat/Main/` or another `Overlay_N/` sailed through. Two
objects then share a basename, `X.o(.text)` is ambiguous, **one object is dropped**.

**This is exactly why drift looked like an "interaction that depends on what is COMMITTED."** It is:
the collision only exists while the *other* file is committed. `0216b7bc` gated green against one HEAD
and red against the next because `src/Combat/Overlay_11/SetField1c8True_02184ad8.cpp` landed in
between. Nothing about the function ever changed.

### M2 — the function is not in `.text`

Twelve overlays plus main have a `kind:code` section that is **not** `.text`:

```
main:  .init 0x020e5920-0x020e692c
ov000 .init 0x02183710-0x021838a4   ov001 .init 0x02164a58-0x02164b74   ov003 .init 0x0217fbf0-0x0217fc78
ov004 .init 0x0217031c-0x02170348   ov014 .init 0x02189510-0x02189598   ov017 .init 0x021d6d28-0x021d7310
ov020 .init 0x0218db90-0x0218dba8   ov025 .init 0x021ef200-0x021ef398   ov028 .init 0x021d9a54-0x021d9a80
ov031 .init 0x02249650-0x02249664   ov033 .init 0x022a29b8-0x022a29e4   ov034 .init 0x022a2a04-0x022a2a30
```

`integrate_ov` writes the delink as `.init` (correctly — it is `kind:code`, so its `NON-CODE` guard
does not fire), dsd emits `Foo.o(.init)`, and **mwcc only ever emits `.text`** — 0 bytes placed.
Already guarded: `classify.py` returns `SECTION` for these (verified: `02183710`, `0217fbf0`,
`021d6d28` → `SECTION`), so they never reach a gate now. They are **not bad matches** and must never
be struck.

### The `+8` that looked like a third mechanism

Every historical log also shows a `+0x8` shift. It is the *same* bug seen from the other side: when
the collision drops an object, mwldarm (`-interworking`) emits an **8-byte long-branch veneer** for
the now-unresolved symbol, right after the module that referenced it. Dumped from the built overlay
at the gap:

```
built ov004 @0216ba70: 04 f0 1f e5 00 00 00 00   =  ldr pc,[pc,#-4] ; .word 0
pristine ov004 @0216ba70: f8 4f 2d e9 ...          (the next function, unshifted)
```

So one overlay grows by 8 (veneer) while the other shrinks by the dropped function's size. **No third
mechanism exists** — see §4.

---

## 2. THE FIX (3 edits, all in `SP/ov_recover.py`; nothing in the repo needed changing)

### 2a. `sanitize()` — name the file after its own definition
```diff
 def sanitize(text, addr):
-    m = re.search(r'\bARM\b[^\n(]*?\b([A-Za-z_]\w*)\s*\(', text)
+    m = re.search(rf'// USA: func_ov{OV}_{addr}[^\n]*\n.*?\bARM\b[^\n;{{]*?\b([A-Za-z_]\w*)\s*\(',
+                  text, re.S) or re.search(r'\bARM\b[^\n(]*?\b([A-Za-z_]\w*)\s*\(', text)
     if not m: return None, text
```
Anchored on the `// USA:` tag, i.e. the same anchor `integrate_ov`'s keep-raw already uses, so the
filename and the delinked definition can no longer disagree.

### 2b. `place()` — basename uniqueness must be GLOBAL, not per-directory
```diff
+_FOREIGN_STEMS = set()
+for _l in sh("git", "ls-files", "src/").stdout.split():
+    _l = _l.replace('\\', '/')
+    if _l.endswith(('.c', '.cpp')) and os.path.dirname(_l) != SRCDIR:
+        _FOREIGN_STEMS.add(os.path.basename(_l).rsplit('.', 1)[0])
...
-        if tracked(bn) or bn in used or os.path.exists(bn): bn = f"{SRCDIR}/{name}_{a}.cpp"
+        if tracked(bn) or bn in used or os.path.exists(bn) or name in _FOREIGN_STEMS:
+            bn = f"{SRCDIR}/{name}_{a}.cpp"
+        if bn.rsplit('/', 1)[-1][:-4] in _FOREIGN_STEMS or bn in used:   # still not unique -> force it
+            bn = f"{SRCDIR}/func_ov{OV}_{a}.cpp"
```

### 2c. `gate_culprits()` — stop parking innocent functions
The delta-boundary scan cannot distinguish a culprit from whatever function happens to sit under a
**secondary** boundary (the `.rodata`/`.data` realignment, or the 8-byte veneer). The unplaced object
names itself, so use that first:
```diff
+    direct = sorted({e for e, g in pairs if g < min(starts) & 0xFF000000} & set(starts))
+    if direct: return [f"{a:08x}" for a in direct]
     import bisect          # (boundary scan kept as the fallback)
```

### 2d. `gate()` — a red gate now self-diagnoses
On failure the gate log gets `lcfcheck` output prepended, so a layout fault is a named line instead
of 6000 "expected to be at" errors.

**Nothing in `config/` or `src/` needed a fix.** The `.init` sections, the delink entries and the
symbol sizes were all correct throughout.

---

## 3. DETECTORS (both new, both in `SP/inv/drift/`)

| tool | cost | catches |
|---|---|---|
| `driftcheck.py <OV> <cand.cpp>…` / `--all` | **0 builds**, pure text | M1 + M2, per candidate, before it is ever placed |
| `lcfcheck.py` | ~1 s, after `ninja delink`+lcf, **no link** | M1 + M2 + anything else, generically |

`lcfcheck` asserts three invariants, each of which *is* a drift:
- **[A]** every `NAME.o(.SEC)` line resolves to exactly **one** object in `objects.txt`
- **[B]** that object really **has** section `.SEC`
- **[C]** per segment, `sum(placed section sizes) == the size delinks.txt declares`

Note for anyone extending it: **mwcc emits one `.text` section PER FUNCTION**, so same-named sections
must be summed, not overwritten (a 7-function object has 7 `.text` sections). Getting this wrong
produces convincing false positives on a green tree.

Also shipped: `wire.py` (side-effect-free single-function integrator), `exp.py` (unwire → wire → gate
→ report drift boundaries; ~35 s/iteration on a warm tree), `mass.py`, `postmortem.py`,
`test_fix.py`, `test_culprits.py`, `names.py`, `trace.py`, `landable.txt`.

---

## 4. EVIDENCE

### 4a. Drift turned on and off by changing **one filename** (the whole argument in two runs)
Same source file, same delink entry, same bytes — only the name on disk differs:

```
$ python exp.py e2_567f8    004  hold_ov004/AllocateAndScanBits_021567f8.cpp
[e2_567f8] GREEN  (0 shifted symbols)

$ python exp.py e8_M1control 004 hold_ov004/AllocateAndScanBits_021567f8.cpp=LoadBattleBlock020ac460.cpp
[e8_M1control] RED  (592 shifted symbols)
   lcfcheck: [A] ov004: `LoadBattleBlock020ac460.o(.text)` resolves to 2 objects
             ['build/usa/src/Combat/Main/LoadBattleBlock020ac460.o',
              'build/usa/src/Combat/Overlay_4/LoadBattleBlock020ac460.o'] -- one object goes UNPLACED
             [C] ov004 .text: placed 0x1c194 but the segment is 0x1c39c (off by -0x208)
   ov004: boundary expected 0x021567f8 delta -0x21567f8  func_ov004_021567f8   <- unplaced, lands at 0
          boundary expected 0x02156a00 delta -0x208                            <- == its size, exactly
```
`driftcheck` predicted this statically, before the build, naming the collision.

### 4b. M2 positive control
```
$ python exp.py e9_M2control 017 hold_ov017/AccumulateFloatChain_021d6d28.cpp
[e9_M2control] RED  (5648 shifted symbols)
   lcfcheck: [B] ov017: `AccumulateFloatChain_021d6d28.o(.init)` -> object has no `.init` section
                 (has ['.text']) -- 0 bytes placed here
             [C] ov017 .init: placed 0x550 but the segment is 0x5e8 (off by -0x98)
   ov017: 0x021d6d28 unplaced; -0x98 thereafter (== size), and ov023..ov034 all shift -0x80
```
(Overlays are `ORIGIN = AFTER(...)`, so shrinking one moves every later overlay — that is why one
bad function reddens 25 overlays and 8000 symbols.)

### 4c. Reproduced the historical `gate_ov004` signature byte for byte
```
$ python exp.py e10_ov004plus8 004 hold_ov004/DispatchByType_0216b7bc.cpp=SetField1c8True_02184ad8.cpp
[e10] RED   ov004: boundary 0x0216ba70 delta +0x8
            ov011: 0x02184ad8 unplaced; -0xc thereafter
```
identical to the recorded log. Map confirms `ov004_57.o` starts at `0216BA78` instead of `0216BA70`,
and the 8 intervening bytes are the veneer shown in §1.

### 4d. All 10 non-`.init` parked "culprits" land GREEN together
```
$ python exp.py e7_parked_all --multi 000:… 003:… 004:… 017:…
wired 02170460 02170c94 021600b8 02160794 021567f8 0216b7bc 02199780 021a0c0c 021b498c 021b5250
[e7_parked_all] GREEN  (0 shifted symbols)
   lcfcheck: OK -- 0x459a54 bytes placed across 163 segments, every lcf line resolves
```
Four overlays, one build, zero drift. List: `SP/inv/drift/landable.txt`.

### 4e. Retrospective: 100 % of every drift ever recorded (`postmortem.py`)
Every `SP/wlog/gate_ov*.txt` on record, 16 424 shifted symbols total, **11 unplaced objects**:

| log | unplaced object | verdict |
|---|---|---|
| gate_ov000 | `0217c094` | M1 → `TestFlag0SetAndFlag1Clear` |
| gate_ov000 | `02183710`, `02183794`, `0218381c` | M2 `.init` |
| gate_ov003 | `02160794` | M1 → `SetEntryPositionById` |
| gate_ov003 | `0217fbf0` | M2 `.init` |
| gate_ov004 | `02184ad8` (ov011) | M1 → `SetField1c8True_02184ad8` dropped by the ov004 copy |
| gate_ov017 | `021a0c0c`, `021b498c`, `021b5250` | M1 |
| gate_ov017 | `021d6d28`, `021d7288` | M2 `.init` |

**No drift event is unexplained.** 6 × M1, 5 × M2, 0 × unknown.

### 4f. Counterexamples sought and not found
- **Extra allocated sections in a decompiled object** (a `.rodata` literal pool). Real case:
  `UpdateCombatantBuffs.o` carries an 8-byte `.rodata` no lcf line places. Tree is green — unplaced
  *extra* sections are simply dropped and shift nothing. Not a drift source.
- **Section alignment.** Every segment is `ALIGNALL(4)`, every ARM function is 4-aligned with a
  4-multiple size, and every candidate object's `.text` is `align=4`. No padding is ever inserted.
- **The `mainfix` nested-sized-symbol bug** (the brief's strongest lead). Checked: it produces a hard
  mwldarm *abort* ("sum of all symbol sizes exceed section size"), never a symbol shift, and
  `modsize_check.py --nested` reports 0 nested sized symbols ROM-wide. **Unrelated to drift.**
- **Mass falsification.** Every held candidate that `driftcheck` passes *and* `classify` rates
  TRUSTED, across ov000/003/004/017/031, wired in one build: `GREEN, 0 shifted symbols`,
  `lcfcheck OK`. Nothing drifted that the detector had not named.

### 4g. Fixes validated
```
$ python test_fix.py
test_fix: 724 held candidates -- 0 named after something other than their own definition,
                                 0 still colliding with a foreign basename
$ python test_culprits.py           # old vs new gate_culprits on all four historical logs
ov000  old->5 (1 false positive: 02170460)     new->4   0 false positives, 0 misses
ov003  old->3 (1 false positive: 021600b8)     new->2   0 false positives, 0 misses
ov004  old->1                                  new->1   0 false positives, 0 misses
ov017  old->6 (1 false positive: 02199780)     new->5   0 false positives, 0 misses
```
The old rule parked **3 of 10** perfectly good functions as "culprits". The new rule parks none.

---

## 5. WHEN TO SKIP (proved to have no C form as things stand)

**Only the `.init` functions.** Six held candidates sit in a `kind:code .init` section:
`ov000 02183710 / 02183794 / 0218381c`, `ov003 0217fbf0`, `ov017 021d6d28 / 021d7288`
(plus `ov031 02249650`, already tagged). Their sources may be perfect matches; they cannot be
delinked because `dsd lcf` will ask mwcc's object for a `.init` section it never emits.
`classify.py` already returns `SECTION` for them — **keep their held sources, never strike them.**
Unblocking them needs a dsd/lcf-side change (emit `Foo.o(.text)` for a `.init`-range delink, or
`#pragma section` the source into `.init`), which is out of scope here and worth its own brief.

Everything else previously written off as "drift" is landable. **Do not re-decompile a drift culprit
— the bytes were always right.**

---

## 6. FINAL REPO STATE

```
$ git status --porcelain      (empty)
$ ninja check                 -> GREEN
$ python inv/drift/lcfcheck.py
lcfcheck: OK -- 0x459a54 bytes placed across 163 segments, every lcf line resolves and every
segment size matches
```
No repo file was modified: no `config/`, no `src/`, no `include/`, no git write. All changes are in
`SP/ov_recover.py` (§2, syntax-checked with `py_compile`) and new files under `SP/inv/drift/`.

**ACTION FOR THE ORCHESTRATOR** — the `ov_recover.py` fix is live and takes effect on the next wave.
The 10 addresses in `SP/inv/drift/landable.txt` are proven-good and currently sitting in
`SP/wlog/drift_ov*.txt` park counters; they will be retried automatically as those counters decay, or
you can clear their lines now to get them a few waves sooner. I did not edit those files to avoid
racing a running wave.
