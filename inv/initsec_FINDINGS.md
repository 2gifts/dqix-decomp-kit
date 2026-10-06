# INVESTIGATOR `initsec` — `.init`-SECTION FUNCTIONS

## VERDICT: **YES — a controllable lever exists, and it is a one-line pragma.**

`.init` functions ARE emittable from readable C++ with this exact toolchain. The blocker was never
the delink config, the lcf, or `integrate_ov.py` — all three were already correct. The single missing
piece was that mwccarm put the code in `.text`. **`#pragma define_section` + `__declspec` fixes it.**

**6 functions matched (6/6 first-or-second try), across 6 modules, covering all 4 families.
2 of them integrated for real: `ninja check` green, `ninja sha1` → `dqix_usa.nds: OK`.**

**No shared script needs to change to make these link.** `integrate_ov.py` is already correct.
(Two *gating* scripts need a 1-line section parameter so workers can gate them — see §6.)

---

## 1. MECHANISM (what was actually wrong)

`.init` holds CodeWarrior's **static-initializer functions** (`__sinit_<TU>`); `.ctor` is a
null-terminated table of pointers to them. ov031's `.ctor` is literally `50 96 24 02 | 00 00 00 00`
= `&func_ov031_02249650`, terminator.

The chain, and where it broke:

| stage | state | verdict |
|---|---|---|
| `delinks.txt` says `.init start:0x02249650 end:0x02249664` | correct | ✅ |
| `integrate_ov.py` `section_for()` writes that `.init` line | correct | ✅ |
| `dsd lcf` emits `File.o(.init)` between `OV031_INIT_START/END` | correct | ✅ |
| lcf declares `KEEP_SECTION { .init, .ctor }` | correct | ✅ |
| **the compiled object contains only `.text`, never `.init`** | **BROKEN** | ❌ |

So the lcf's `(.init)` line placed **0 bytes**. mwldarm said
`warning: func_ov031_02249650(.text) in file probe.o is referenced but has not been written`,
the symbol resolved to `0x0`, and everything after the hole slid down.

**The −0x20 drift is arithmetic, and it confirms the diagnosis exactly.** With `.init` contributing
0 instead of 0x14: `.ctor` starts at 0x02249650, is 8 bytes → ends 0x02249658; `.data` is
`align:32` → 0x02249660. Observed: `data_ov031_02249680 … is at 0x02249660`. Exact.

---

## 2. THE FIX

```c
#pragma define_section initcode ".init" RX     // once per file, near the top
extern "C" __declspec(initcode) ARM <ret> func_ovNNN_<addr>(<args>) { ... }
```

Grammar (recovered from the compiler's own diagnostic, which is the authority here):

```
#pragma define_section <tag> "<section>" [abs32|pcrel32|sbrel32|sbrel12] [R|RX|RW|RWX]
```

Proof it produces a real ELF section — not a cosmetic attribute:

```
== ordA.o
  [17] .init                    size=0x14
  [18] .rela.init               size=0x18
  func_ov031_02249650   FUNC  GLOBAL  val=0x0  size=0x14  sec=.init
```

### ⚠️ LANDMINE — `__declspec` MUST come before `ARM`, and the function MUST already be
### `extern "C"` and already named `func_ovNNN_<addr>`.

`integrate_ov.py` locates the definition with
`(\bARM\b[^\n;{]*?\b)(\w+)(\s*\()`. That is non-greedy, so if `__declspec(...)` sits *after* `ARM`
it captures **`__declspec`** as the function name and renames *that*. Simulated on my first draft:

```
t_02249650:  captured-name='__declspec'           has_extern_C=False -> WOULD REWRITE
ordA:        captured-name='func_ov031_02249650'  has_extern_C=True  -> NO REWRITE (stable)
```

The bad rewrite emits `extern "C" ARM func_ov031_02249650(initcode) void* Semantic_...(void)`.
That is a **compile error**, so it fails loudly rather than mislinking — but it burns a wave.
Writing the definition already in canonical form (`extern "C"` + raw name) makes the integrator a
**no-op** on the file, which is what the two landed files did.

Compile-tested orderings:

| form | result |
|---|---|
| `extern "C" __declspec(initcode) ARM void* f(void)` | ✅ **use this** — `.init`, integrator-stable |
| `extern "C" ARM __declspec(initcode) void* f(void)` | ✅ compiles, but integrator renames `__declspec` |
| `__declspec(initcode) extern "C" ARM void* f(void)` | ❌ `declaration syntax error` |
| `extern "C" ARM void* __declspec(initcode) f(void)` | ❌ `declaration syntax error` |

---

## 3. EVIDENCE

Baseline before touching anything: `ninja check` → **exit 0** (green).

### Gate results (`wgate_sec.py` = my private copy of `wgate.py` with the section parametrised; the shared `wgate.py` is untouched)

| module | addr | family | size | result |
|---|---|---|---|---|
| ov031 | `02249650` | F1 tail-call thunk | 0x14 | **MATCH** (1st try) |
| ov020 | `0218db90` | F4 misc | 0x18 | **MATCH** (1st try) |
| ov034 | `022a2a04` | F2 init+register | 0x2c | **MATCH** (1st try) |
| ov004 | `0217031c` | F2 init+register | 0x2c | **MATCH** (1st try) |
| ov003 | `0217fbf0` | F3 `_fadd` chain | 0x88 | **MATCH** (2nd try) |
| ov000 | `02183710` | F3 `_fadd` chain | 0x84 | **MATCH** (2nd try) |

### Real link, ov031 alone (whole-segment fill)

```
Check ARM9 main: OK
Check overlay 31: OK
=== EXIT: 0 ===
lcfcheck: OK -- 0x459a54 bytes placed across 163 segments, every lcf line resolves
               and every segment size matches
dqix_usa.nds: OK
```
The `func_ov031_02249650 … has not been written` warning is **gone**.

### Real link, ov031 + ov000 (ov000 is a **PARTIAL** fill: 1 of 3 `.init` funcs, 0x84 of 0x194)

```
Check ARM9 main: OK
Check overlay 0: OK
Check overlay 31: OK
=== check exit 0 ===
lcfcheck: OK ...
dqix_usa.nds: OK
```

**Partial `.init` fill works** — dsd splits the residual `.init` into the asm blob exactly as it does
for `.text`. This was the main open risk; it is closed. Multi-function `.init` segments
(main ×42, ov017 ×11) can be attacked incrementally, one function per wave.

### Counterexamples sought and not found
- Does the object still emit a stray `.ctor` that would double-place? **No** — `__declspec` places
  only the function; `.ctor` stays with the asm blob. Confirmed by `secs.py` (only `.init` +
  `.rela.init`) and by the byte-exact ROM.
- Does `-w off` hide a rejected pragma? Re-ran everything with `-w on -warn pragmas`; the accepted
  forms emit **zero** diagnostics. The rejected forms are the ones listed above.

---

## 4. THE RULE (worker-applicable lookup)

**Trigger:** the addr falls in a module's `.init` range (`grep '\.init' <module>/delinks.txt`).
Do **not** skip it. `.init` is now an ordinary matching job.

```
1. Read the target:  grep -rhA40 "^func_ovNNN_<addr>:" build/usa/asm/ovNNN/
2. Add ONE line near the top of the file:
       #pragma define_section initcode ".init" RX
3. Write the definition in EXACTLY this shape (order is load-bearing):
       extern "C" __declspec(initcode) ARM <ret> func_ovNNN_<addr>(<args>) { ... }
   - `__declspec(initcode)` BEFORE `ARM`
   - already `extern "C"`, already named `func_ovNNN_<addr>`  (so the integrator is a no-op)
   - keep the `// USA: func_ovNNN_<addr>` tag directly above
4. Body: match it like any .text function — every existing recipe applies unchanged.
5. Gate with the section argument:  python wgate.py <OV> <addr> <file> .init   (needs §6 patch)
6. Integrate: NOTHING SPECIAL. integrate_ov.py already writes `.init` into delinks.txt.
```

### Family table — all 69, every family has a proven representative

| family | shape | count | modules | status |
|---|---|---|---|---|
| **F1** tail-call thunk | `ldr ip,=fn; ldr r0,=data; bx ip` (20 B) | 8 | main 7, ov031 1 | ✅ proven (ov031, linked) |
| **F2** init + register-dtor | `bl Init(&obj); bl func_0200efd0(&obj,&dtor,&region)` (44 B) | 13 | main 9, ov004/028/033/034 | ✅ proven (ov004, ov034) |
| **F3** float `_fadd` chain | accumulate-and-store, 132–184 B | 39 | main 18, ov017 11, ov000 3, ov025 3, ov001 2, ov003 1, ov014 1 | ✅ proven (ov003, ov000, linked) |
| **F4** misc / one-off | 24–356 B | 9 | main 8, ov020 1 | ✅ proven (ov020) |

> **Correction to the brief:** the count is **69**, not 18 (42 in main + 27 in overlays, 13 modules).
> Verified structurally: per module, Σ(function sizes) == the declared `.init` segment size, **EXACT
> for all 13**. The "18" in `classify.py`'s comment counted only the `.init` addrs present in that
> run's candidate pool, not the ROM.

### Worked before/after — F1, ov031 `0x02249650`

Target:
```
ldr ip, .L_0224965c
ldr r0, .L_02249660
bx ip
.L_0224965c: .word _Z23ResetAndReturnAllocatorP13SafeAllocator
.L_02249660: .word data_ov031_02291e3c
```
BEFORE (`.text` — links to 0x0, drifts the whole overlay, 771 symbol errors):
```cpp
ARM void* ResetGlobalAllocator_02249650(void) {
    return ResetAndReturnAllocator(&data_ov031_02291e3c);
}
```
AFTER (`.init` — MATCH, ROM byte-identical):
```cpp
#pragma define_section initcode ".init" RX

class SafeAllocator;
extern SafeAllocator data_ov031_02291e3c;
void* ResetAndReturnAllocator(SafeAllocator* allocator);

// USA: func_ov031_02249650  (semantic: ResetGlobalAllocator)
extern "C" __declspec(initcode) ARM void* func_ov031_02249650(void) {
    return ResetAndReturnAllocator(&data_ov031_02291e3c);
}
```

### Bonus rule for F3 (39 functions — the biggest family)

The `_fadd` chains are `acc`-and-store sequences. The only thing that ever went wrong was the
ldr/str order inside a block, and it has a mechanical fix:

> **Between the accumulate and the store, hoist the NEXT source member into a named local.**

```cpp
t = t + s->a;
float sc = s->c;   // <-- hoist: forces  ldr &S, ldr &D, ldr S[c], str D[..]
d->a = t;          //     without it mwcc emits  ldr &D, ldr &S, str D[..], ldr S[c]
t = t + sc;
```
This is recipe #15 (scratch regs follow *use* order) applied to the pool pointers. It took ov003
from `BYTEDIFF: 10 bytes @0x60` → **MATCH**, and ov000 from `BYTEDIFF: 10 bytes @0x48` → **MATCH**.
Also note F3 heads often start `ldmia r1,{r0,r1}` — that is just two **adjacent** floats
(`s->a + s->b`), recipe #20; non-adjacent members give two separate `ldr`s.

---

## 5. WHEN TO SKIP

Nothing in `.init` is skip-on-sight any more. The old blanket rule
*"Addrs ≥ 0x020e5920 = .init → SKIP"* is **obsolete and should be deleted** from `worker_ov_all.md`
— it was blocking all 42 main-module `.init` functions.

Skip only for ordinary body reasons (the existing PROVEN-NO-C-FORM list). Two genuine cautions:
- **F4 `main 0x020e6790` (356 B) / `0x020e6420` (220 B)** — large one-offs into `func_0200ee94`
  and `func_01ff96fc/9718`; treat as normal large functions, not as a family.
- The `.ctor` pointer table is **not** worker territory. It stays in the asm blob. Never try to
  emit `.ctor` from C++ (that needs a real static object, which drags in ctor/vtable symbols this
  project does not have — see recipe #16).

---

## 6. `classify.py` SECTION GUARD — **RELAX IT, do not keep it, do not just delete it**

Current behaviour (`classify.py:70-74`) returns verdict `SECTION` for any addr outside `.text`,
parking all 69 permanently. That is now wrong.

But a bare deletion **won't work**: two shared scripts measure `.text` by name and would report
`SIZE/OVERGEN: 0 .text total=0x0` for a correct `.init` function.

Required (1 line each, both mechanical — **I did not make these edits**; I proved them out in a
private copy, `inv/initsec/wgate_sec.py`):

- **`wgate.py`** — take an optional 4th arg `SEC` (default `.text`) and substitute it in the three
  places that hardcode `'.text'` / `'.rel.text'` / `'.rela.text'`. Exact diff is in
  `inv/initsec/wgate_sec.py`; it is otherwise byte-identical to `wgate.py`.
- **`classify.py`** — replace the `SECTION` early-return with: look up the section via the same
  `section_for(addr)` logic `integrate_ov.py` already has, measure *that* section, and emit verdict
  `SECTION` only for a **non-code** section (`rodata`/`data`/`bss`). `.init`/`.text` both proceed.

`integrate_ov.py` — **NO CHANGE**. It was right all along.

---

## 7. FINAL REPO STATE — **GREEN AND CLEAN**

Everything I placed was reverted by restoring byte-identical backups (not `git checkout`, so no
other investigator's work could be touched). Verified:

```
md5 config/usa/arm9/overlays/ov031/delinks.txt = 5608779afbdd80f362f655ebb798026e  (== original)
git status --short   -> (no output)
ninja check          -> exit 0, 0 errors
ninja sha1           -> dqix_usa.nds: OK
```

I ran only the permitted commands (`tools/configure.py usa --no-extract`, `ninja check`,
`ninja sha1`). No `git commit`/`push`/`reset`. No shared script edited.

### Ready to land (already proven green together, in one build)

The two verified files are preserved at `inv/initsec/landable/`:

| file | delinks line to append to `config/usa/arm9/overlays/ovNNN/delinks.txt` |
|---|---|
| `ResetGlobalAllocator_02249650.cpp` → `src/Combat/Overlay_31/` | `.init start:0x02249650 end:0x02249664` |
| `SumVectorIntoTotals_02183710.cpp` → `src/Combat/Overlay_0/` | `.init start:0x02183710 end:0x02183794` |

`symbols.txt` needs **no** edit (keep-raw names). The other four matched sources
(`g_ov003.cpp` h2 variant, `g_ov004.cpp`, `g_ov020.cpp`, `g_ov034.cpp`) are in `inv/initsec/`,
each gating MATCH but not yet link-tested.

### Files I created (all outside `src/`, per the shared rules)
`inv/initsec/` — `secs.py` (ELF section/symbol dump), `enum_init.py` (the 69-function census),
`wgate_sec.py` (section-aware gate), `odis_sec.py`, `cc.sh`/`ccw.sh`, the candidate `.cpp`s,
`backup/` (pristine delinks), `landable/`.
