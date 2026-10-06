# THUMB FINDINGS — project flags
> **Measured on mwccarm 2.0/sp1p5.** The ROM now builds with 2.0/sp2p2 and `-str pool,reuse`
> ([[dqix-sp2p2-rebase]]). The labs under `SP/lab/` and `SP/pad/` follow the build again, so a
> rule here is worth re-measuring before it is cited as settled.


## 1. VERDICT

**YES — a controllable lever exists, and thumb is one of the CHEAPEST families in the ROM.**

* `#pragma thumb on` works. The macro already exists: **`THUMB`** in `include/globaldefs.h`
  (`#define THUMB _Pragma("thumb on")`). Put it exactly where `ARM` goes. Recipe #11's claim is
  correct but was never exercised.
* **33 thumb functions matched byte-exact** in this investigation (31 in ov031, 2 in arm9-main),
  from every shape present in the family: getters/setters, bool tests, byte-swaps, byte loops,
  strlen, guarded do-while loops, tail-call thunks, `blx` interworking calls, indirect calls
  through function-pointer tables, stack-frame functions, and 5-arg/6-arg stack-argument wrappers.
* **The blocker was never the compiler — it was the tooling.** `wgate.py` NO-SLOTs every thumb
  address, and `classify.py` / all four `genwave*.py` / `integrate_ov.py` / `integrate_main.py`
  filter on `kind:function(arm,…)`, so no worker was ever handed a thumb address and none could
  have gated one. Exact one-line diffs in §6. **I did not edit any shared script** — I used a
  private copy, `SP/inv/thumb/tgate.py`.
* Scale is exactly **166** (158 ov031 + 8 arm9-main; the other 18 thumb symbols are named SVC stubs).

**The single most important new fact: the ov031 thumb block was compiled for SPEED, not size.**
`#pragma optimize_for_size off` (== `-O2,p`) at the top of the file. It is **required** for leaf
functions that use callee-saved registers and a **verified no-op everywhere else** (all 33 matches
re-gated with it inserted: 33/33 still MATCH), so write it unconditionally.

---

## 2. THE RULE

### 2a. Mode selection

| you want | write | notes |
|---|---|---|
| an ARM function | `ARM <ret> Name(...)` | unchanged |
| a **thumb** function | `THUMB <ret> Name(...)` | `THUMB` is already in `globaldefs.h` |

`ARM`/`THUMB` are **state pragmas**, not attributes. They apply to every function *defined* after
them. Put the macro **directly on the definition**; never rely on one that appeared on a
declaration higher in the file.

### 2b. The mandatory boilerplate for ov031 thumb

```c
#include <globaldefs.h>

#pragma optimize_for_size off        // ov031 thumb block == -O2,p.  Harmless when it changes nothing.

// USA: func_ov031_<addr>
THUMB <ret> Name_<addr>(<args>) { ... }
```

Detecting whether you need it, from the target disasm — one symptom only:

| target prologue/epilogue | meaning | action |
|---|---|---|
| `push {r3,r4…}` … `pop {r3,r4…}` + **`bx lr`** (lr never pushed) | LEAF that saves callee-saved regs, built `-O2,p` | **you need `#pragma optimize_for_size off`** |
| `push {r4,lr}` … `pop {r4,pc}` | size-optimised leaf | plain `-O2` |
| `push {r3,lr}` … `pop {r3,pc}` | non-leaf | either; pragma is a no-op |
| no push at all | leaf, no callee-saved regs | either; pragma is a no-op |

Verified: **33/33** matching files still MATCH with the pragma added. Only leaf functions that
touch r4+ can tell the difference. **Just always write it for ov031/thumb.**

### 2c. Thumb-specific codegen rules (these DIFFER from the ARM recipes)

| construct | ARM recipe | **THUMB reality** |
|---|---|---|
| **if-conversion / predication (recipe #3, "the count is 5")** | ≤5 instrs → predicated | **DOES NOT EXIST.** Thumb has no predication. Every `if` is a real branch. **Never apply recipe #3 to a thumb function.** |
| **bool return** (recipe #8) | `return f()!=0;` | `return (x&M)!=0;` gives **split returns** (`movs r0,#1; bx lr` / `movs r0,#0; bx lr`). The ROM's merged form (`movs r2,#0 … movs r2,#1 … adds r0,r2,#0`) comes from **`bool r=false; if(c) r=true; return r;`** — see §4 recipe R3. |
| **register move** | `mov rD,rS` | `adds rD,rS,#0` (`0x1Cxx`) for low regs; `mov r8,r8` (`0x46C0`) is the 2-byte **alignment pad**, not code. |
| **tail call** | `ldr ip,<pool>; bx ip` | `ldr r3,<pool>; bx r3`. A void/valued call in tail position **always** becomes this. Huge family (12 pure thunks). |
| **loop skeleton (recipe #18)** | one skeleton, `b <test>` at entry | **TWO** shapes, both source-selectable: `while/for` → `b <test>` at entry; **guarded do-while** (`cmp; bCC skip;` then bottom-test body) → write it literally as `if (i < n) { do { … } while (i < n); }`. The ROM's loops are overwhelmingly the guarded form. |
| **`bl` vs `blx`** | you pick nothing | you pick nothing here either — mwcc always emits `bl` and the **linker** converts it to `blx` when the callee is ARM. The mode macro on a *declaration* is irrelevant to call encoding (verified, `lab_mode.cpp`). |
| **thumb function pointer in a pool word** | n/a | the ROM word has **bit0 set** (`…fa59` for a function at `…fa58`). Reference the plain symbol; the linker sets the bit. The existing delinked `.s` also writes `.word func_ov031_0221fa58` with no `+1`, so this is already proven at link. |
| **struct copy (recipe #20/OVERGEN)** | same | same — 3 separate members → out-of-line `operator=`. Use `struct { int v[3]; }` (1 copy unit) to keep it inline. |
| **stack frame (recipe #19)**, **switch tables (#17)**, **CSE/recompute (#2)**, **scratch regs (#15)** | — | unchanged, they behave the same in thumb. |

### 2d. Slot size arithmetic (this is what makes SIZE checks look wrong)

A thumb function's ROM slot is 4-byte aligned; mwcc emits the trailing 2-byte pad **only when a
literal pool follows**. So:

```
slot = code_bytes  (pool present, mwcc emits its own `mov r8,r8` pad)
slot = code_bytes + 2  (no pool, function ends on a halfword — the LINKER pads)
```
A correct thumb match therefore produces a `.text` of **`slot` or `slot-2`** bytes. `wgate.py`
demands exactly `slot` → false `SIZE/OVERGEN` on every odd-halfword thumb function.

---

## 3. EVIDENCE

### 33 verified `MATCH` (each = `tgate.py` MATCH: masked ROM byte-compare + size + UNDEF + reloc-target)

ov031 (31): `0221c49c 0221d060 0221d06c 0221d270 0221dc58 0221e458 0221e470 0221e51c 0221e52c
0221e5a0 0221e5b0 0221e5b4 0221e5d0 0221e5f8 0221e610 0221e638 0221e650 0221e6d4 0221e6e0
0221e798 0221ecac 0221ecb4 0221fa88 02220178 022201cc 022205d8 0222127c 02222818 02222830
022228b0 02222af0`

arm9-main (2): `020c111c 020c1264`

Sources: `SP/inv/thumb/w<addr>.cpp` (ov031) and `SP/inv/thumb/m<addr>.cpp` (main). They are
integration-ready except that `integrate_*.py` cannot see them yet (§6).

### The gate is not a rubber stamp — 4 negative controls, all correctly REJECTED

| injected fault | verdict |
|---|---|
| `blx` callee swapped `func_ov031_022073a4` → `…072d0` | `RELOC-WRONG want callee@0x22073a4` |
| pool tail-call callee swapped (thumb target) | `RELOC-WRONG want data@0x221fa59` |
| pool data symbol swapped | `RELOC-WRONG want data@0x224e6e0` |
| pool tail-call to a different thumb func | `RELOC-WRONG want data@0x221e6e9` |

### Enumerations run

* **Compiler builds**: all 10 `2.0/*` and all 5 `1.2/*` builds on the leaf-prologue shape. Every
  `2.0` build is byte-identical; `1.2/base…sp2p3` differ only in the epilogue (`pop{r4};pop{r3};bx r3`).
  **Every `2.0` build agrees, the project's `2.0/sp2p2` included — the compiler is not the
  variable here.**
* **Flag sweep** (15 variants: `-O0/1/2/4`, `-O2,p`, `-O4,p`, `-O4,s`, `-opt speed|space`,
  `-interworking` off, `-proc arm7tdmi`, `-lang=c99`, `-char unsigned`, `-ipa file`): the leaf
  `push {r3,r4}` form is produced by **exactly** `-opt speed` / `-O2,p` / `-O4,p`, and reproduced
  in-source by `#pragma optimize_for_size off`.
* **Pragma-invariance**: all 33 matching files re-gated with `#pragma optimize_for_size off`
  inserted → **33/33 still MATCH**.
* **Reloc types actually emitted** (dumped from 6 objects): thumb `bl`/`blx` = **type 10
  (R_ARM_THM_PC22)**; pool words = type 2 (R_ARM_ABS32). mwcc marks thumb code **only** with the
  `$t`/`$a` ELF mapping symbols — `st_value` bit0 is **not** set, so any thumb-detection code that
  looks at `st_value & 1` will silently mis-decode.
* **Counterexample hunt for the `-O2,p` claim**: scanned all 158 ov031 thumb functions. 3 leaves
  with callee-saved regs all use the lr-free `push {r3,r4}` / `push {r4,r5}` form (consistent);
  2 large leaves (`0222170c`, `02221ac8`) use `push {r4-r7,lr}` / `pop {r4-r7,pc}` — that is mwcc's
  ≥4-callee-saved-reg form and is emitted under `-O2,p` too, so it is not a counterexample.

---

## 4. RECIPE (mechanical)

**Step 0 — gate.** `wgate.py` cannot gate thumb. Use
`python SP/inv/thumb/tgate.py <OV|main> <addr> <file.cpp>` until the shared gate is patched.
`tgate.py` is a line-for-line copy of `wgate.py` plus the five thumb fixes and also supports
`main` (pass `main` instead of an overlay number).

**Step 1 — confirm it is thumb.**
`grep 'func_ov031_<addr> kind:' config/usa/arm9/overlays/ov031/symbols.txt` → `function(thumb,size=…)`.
Read the target with `python SP/inv/thumb/rom.py 031 <addr>` (real thumb disassembly of the
pristine ROM; the delinked `.s` prints `mov r0,r1` for what is really `adds r0,r1,#0`, and prints
the alignment pad as an instruction).

**Step 2 — write the skeleton.**

```c
#include <globaldefs.h>

#pragma optimize_for_size off

// USA: func_ov031_<addr>
THUMB <ret> Name_<addr>(<args>) { … }
```

**Step 3 — pick the body from the shape table.**

| target shape | write |
|---|---|
| `ldr rX,<pool>; ldr rX,[rX]; blx rX` | `typedef R (*Fn)(A); extern Fn data_ov031_XXXX; … data_ov031_XXXX(a);` — the temp register tells you the **arity**: pointer in `r1` ⇒ `r0` is a live pass-through param, pointer in `r0` ⇒ no params. |
| `ldr r3,<pool>; bx r3` (± arg shuffle) | plain tail call: `Callee(args…);` (or `return Callee(...);`) — nothing else. |
| `blx #imm` | ordinary call to an ARM function; grep symbols.txt for the decoded target address. |
| `movs rN,#0; … cmp/tst; bCC; movs rN,#1; adds r0,rN,#0` | `bool r = false; if (cond) r = true; return r;` (**not** `return cond;`) |
| `cmp rN,#0; bls/ble <after>` then a bottom-tested body | `unsigned i = 0; if (i < n) { do { … } while (i < n); }` — write the guard with the **same relation as the loop test** (`i<n` → `bls`; a literal `if (n != 0)` gives `beq` instead). |
| `b <test>` at entry, bottom test | plain `for`/`while` |
| a `lsls #24; asrs #24` hoisted **above** the loop | the value is an `int` param narrowed by an explicit cast in the body: `*d = (char)value;` (a `char` param produces no narrowing at all) |
| `push {r3,r4}` / `pop {r3,r4}; bx lr` | you forgot `#pragma optimize_for_size off` |
| `ldr rX,<pool>; str/ldr rX,[rX,#K]` | `extern T data_ov031_YYYY; *(int*)((char*)&data_ov031_YYYY + K) = v;` |

**Step 4 — gate; on BYTEDIFF route to §2c, not to the ARM recipes.**

### Worked before/after

Target `func_ov031_02222818` (`size=0x18`):

```
push {r3, r4}
movs r4, #0
cmp  r2, #0
bls  .end
.loop:  ldrb r3,[r1,r4] ; strb r3,[r0,r4] ; adds r4,r4,#1 ; cmp r4,r2 ; blo .loop
.end:   pop {r3, r4}
        bx lr
```

BEFORE — the natural ARM-worker translation → `SIZE/OVERGEN 0x12 vs 0x18`:
```c
THUMB void CopyBytes(char* dst, const char* src, unsigned int count) {
    unsigned int i;
    for (i = 0; i < count; i++) dst[i] = src[i];        // -> `b <test>` skeleton, push {r4,lr}
}
```

AFTER — **MATCH**:
```c
#include <globaldefs.h>

#pragma optimize_for_size off                            // -> push {r3,r4} / pop {r3,r4}; bx lr

// USA: func_ov031_02222818
THUMB void CopyBytes_02222818(unsigned char* dst, const unsigned char* src, unsigned int count) {
    unsigned int i = 0;
    if (i < count) {                                     // -> cmp r2,#0 ; bls .end
        do {
            dst[i] = src[i];                             // unsigned char -> ldrb (signed char -> ldrsb)
            i++;
        } while (i < count);
    }
}
```

---

## 5. WHEN TO SKIP

* **`kind:function(thumb,size=0x4)` in `config/usa/arm9/symbols.txt` named `Div`, `Sqrt`, `CpuSet`,
  `SoftReset`, … (18 of them)** — BIOS SVC stubs, already named, not in the 166. Never touch.
* **Block-layout mismatch inside an otherwise-identical loop.** Proven on `func_ov031_0221e4fc`
  (byte memcmp): the ROM lays the loop out `[latch][test][body]`; mwcc emits `[body][latch][test]`
  or `[test][body][latch]` and nothing reaches the third. Enumerated: `for(;n-->0;a++,b++)`,
  `while(n-- >0){…break;}`, `…{…continue;}`, `for(;;){if(n--<=0)break;…}`, `while(1){…else break;}`,
  `#pragma opt_repositioncode on`, `#pragma optimize_for_size off/on`. All produce one of the two
  reachable layouts. **Same class as the existing "load/store SCHEDULING → no C form" entry. SKIP.**
* **Scratch-register/def-placement mismatch when the store order already matches.** Proven on
  `func_020c1280` (`MTX_RotY33_`): store order, instruction count and slot size all match exactly;
  the ROM computes `-sinVal` and `FX32_ONE` *before* the `_20` store, mwcc sinks both defs to their
  uses. This is recipe #15 ("decl order is a no-op for scratch regs") — no lever. SKIP.
  *(Useful sub-finding anyway: for the `func_020c111c/1264/1280/129c/197c/199c/19b8` matrix family
  the ROM's store order is literally the NitroSDK source order —
  `_00, _22, _01, _10, _12, _21, _20, _02, _11` for RotY — so write the assignments in the exact
  order the `str` offsets appear.)*
* **`stm rD!,{r1,r2,r3}` of constants** (`func_020ca7d0`, zero a 3×3 matrix). A `struct{int v[3];}`
  assignment is 1 copy unit and does inline, but mwcc materialises it as `ldm`+`ldr` from a stack
  temp, not as three constant-register `stm`s. Not cracked here; treat as SKIP until someone finds
  the form.
* **`func_ov031_0221c5ac` (0xa18), `02221f78` (0x848), `02220af0` (0x3e4), `0222170c`/`02221ac8`
  (0x3bc each)** — not proven unmatchable, just large. Normal size triage applies; 9 of the 166 are
  >0x200 and 46 are ≤0x20.

---

## 6. TOOLING — EXACTLY WHAT MUST BE WIDENED (report only; nothing was edited)

### 6a. Discovery — thumb addresses are never handed to a worker

| file:line | current | change to |
|---|---|---|
| `classify.py:54` | `func_ov%s_([0-9a-f]{8}) kind:function\(arm,size=0x([0-9a-f]+)\)` | `…kind:function\((?:arm\|thumb),size=…` |
| `genwave.py:25` | `func_([0-9a-f]{8}) kind:function\(arm,size=…\) addr:…` | same widening |
| `genwave_ov.py:15` | `{PREFIX}([0-9a-f]+) kind:function\(arm,size=…\)` | same widening |
| `genwave_main.py:46` | `^func_([0-9a-fA-F]{8}) kind:function\(arm,size=…\)` | same widening |
| `genwave_direct.py:33` | `func_ov%s_([0-9a-f]{8}) kind:function\(arm,size=…\)` | same widening |

Each also has to tell the worker the ISA, otherwise the worker writes `ARM` and gets a 2× size
mismatch. Cheapest: emit the addr as `<addr>:thumb` or set `THUMB` in the per-addr brief.

### 6b. `wgate.py` — 5 fixes (all present and exercised in `SP/inv/thumb/tgate.py`)

1. **line 29** `kind:function\(arm,size=0x([0-9a-f]+)\)` → `kind:function\((arm|thumb),size=0x([0-9a-f]+)\)`
   (capture the ISA; adjust the group index). *Without this every thumb addr is `NO-SLOT`.*
2. **line 36** `r'\bARM\b[^\n(;{]*?\b([A-Za-z_]\w*)\s*\('` → `r'\b(?:ARM|THUMB)\b…'`
   (the BAD-NAME 0x-token check silently skips thumb defs otherwise).
3. **line 49** `total != slot` → `total not in ((slot, slot-2) if isa=="thumb" else (slot,))`
   — see §2d. *Without this, every thumb function with no literal pool reports `SIZE/OVERGEN`.*
4. **lines 52–55, reloc masking** `o = rr['r_offset'] & ~3` is wrong for thumb branches: a thumb
   `bl`/`blx` pair sits at a **halfword** offset and covers `[off, off+4)`. Rounding down to a word
   leaves 2 of its 4 bytes unmasked → **false BYTEDIFF**. Use:
   ```python
   THM_BR = {10, 25, 30, 31}     # R_ARM_THM_PC22 / THM_CALL / THM_JUMP*
   o = rr['r_offset']
   reloc.update(range(o, o+4) if rr['r_info_type'] in THM_BR else range(o & ~3, (o & ~3)+4))
   ```
5. **line 87 ff., RELOC-TARGET VERIFY** — answers the brief's `blx` question directly:
   * The current `typ in (1,28,29)` branch handles **ARM** branches only, and its
     `if (pinstr>>24)&0xFE == 0xFA: continue` skips ARM→thumb `blx`. For a **thumb** function none
     of that applies: every call is **type 10**, which the loop ignores entirely — so today a thumb
     function calling the *wrong* callee would pass the gate and poison the overlay checksum.
   * Add:
     ```python
     elif typ in THM_BR:
         hi = pinstr & 0xFFFF; lo = (pinstr >> 16) & 0xFFFF
         if (hi & 0xF800) != 0xF000: continue
         off23 = sign(((hi & 0x7FF) << 12) | ((lo & 0x7FF) << 1), 23)
         tgt = (P + 4 + off23) & 0xFFFFFFFF
         if (lo & 0xF800) == 0xE800: tgt &= ~3        # BLX -> ARM target, word aligned
         if S is None or (S & ~1) != tgt: wrong.append(...)
     ```
   * **ABS32 (`typ == 2`)**: a pool word pointing at a *thumb* function has **bit0 set** in the ROM,
     so `(S+A) != pinstr` fires a false `RELOC-WRONG`. Accept `S+A+1` when the symbol is
     `kind:function(thumb`.

### 6c. `integrate_ov.py` / `integrate_main.py` — thumb files cannot be integrated at all today

| file:line | problem |
|---|---|
| `integrate_ov.py:102`, `integrate_main.py:128` | the keep-raw definition regex requires `\bARM\b`: `(\bARM\b[^\n;{]*?\b)(\w+)(\s*\()`. A `THUMB` definition never matches → `NO-DEF … cannot locate def to keep-raw — SKIP`. **Widen to `\b(?:ARM\|THUMB)\b`.** (Same landmine as recipe #21's `__declspec`-before-`ARM` rule: the *first* `ARM`-or-`THUMB` token before the name is what gets captured.) |
| `integrate_ov.py:63`, `integrate_main.py:105` | `size_of()` matches `kind:function\(arm,size=…\)` → returns `None` → `NO-ARM-SLOT … SKIP`. Widen the same way. |
| `integrate_ov.py:123`, `integrate_main.py:158` | `total != slot` — needs the `slot-2` tolerance of §2d. |
| `integrate_ov.py:~127`, `integrate_main.py` equivalent | `_o = _rr['r_offset'] & ~3` — same halfword masking bug as wgate fix 4. |
| `integrate_ov.py:154–160`, `integrate_main.py:~190–196` | same missing type-10 branch and same ABS32 thumb-bit issue as wgate fix 5. |

Good news: the symbol-rename pass (`integrate_ov.py:171`, `integrate_main.py:212`) uses
`(kind:function.*)` and therefore **preserves `(thumb,size=…)` correctly** — no change needed there.

### 6d. Two facts any thumb-aware tooling needs

* mwcc marks thumb code with the ELF **mapping symbols `$t` / `$a` / `$d`** (STT_FUNC, STB_LOCAL,
  at the section offset where the mode changes). It does **not** set `st_value & 1`. Disassembler
  helpers must use the mapping symbols; `SP/odis.py` needs its `thumb` argument passed explicitly
  and `try.sh` never passes it, so `try.sh` disassembles thumb output as ARM garbage. Use
  `SP/inv/thumb/tdis.py` instead (auto-detects per section, prints *all* `.text` sections).
* mwcc always emits a plain thumb `bl` placeholder; the **linker** turns it into `blx` when the
  callee is ARM. So the compiled bytes legitimately differ from the ROM at every call site — they
  are reloc-masked, which is exactly why fix 4 (correct masking) and fix 5 (real target decoding)
  have to land together.

---

## 7. FILES

* `SP/inv/thumb/tgate.py` — thumb-capable gate (`<OV|main> <addr> <src> [section]`). Use this.
* `SP/inv/thumb/rom.py` — pristine-ROM thumb disassembly of one function.
* `SP/inv/thumb/tdis.py` — compile + disassemble all `.text`, ARM/thumb auto-detected.
* `SP/inv/thumb/sweep.py`, `scan.py`, `census.py` — the flag sweep, the leaf-prologue ROM scan, the 166-function census.
* `SP/inv/thumb/w<addr>.cpp` (31, ov031) and `m<addr>.cpp` (2, main) — the verified matches.
* `SP/inv/thumb/lab_*.cpp` — the enumeration labs (tail calls, bool shapes, loop shapes, fill
  narrowing, byteswap operand order, leaf prologues, call-mode encoding).
* `*.cpp.nomatch` — the three proven-negative cases from §5.
