# METHOD FINDINGS — a repeatable process for VERY LARGE functions (>600B)

Scope: 968 still-undecompiled functions are ≥600B; measured worker success on >1024B was 0/20.

## 1. VERDICT — **PARTIAL, and the split is predictable**

A working process exists, and it is *not* "block-by-block convergence". The lever is not a codegen
idiom, it is a **triage rule plus an aligned-diff tool**:

* **`ov004/02168b44` (1536B, 384 words) — MATCHED byte-exact.** `wgate.py 004 02168b44` → `MATCH`.
  4 source revisions. The FIRST compile was already 301/377 instructions correct.
* **`ov000/02166e5c` (1248B) — 97.2% byte-exact** (35/1248 bytes differ, 8 offsets).
  Blocked ONLY by a 2-register permutation. 22 source variants tried, no lever.
* **`ov004/0215f0c0` (1588B) — structurally reproduced, exact instruction count (397/397),
  register-blind match 319/397 — but 814/1588 bytes differ.** Blocked by a whole-function
  spill decision. 21 source variants tried, no lever.

**The thing that decides it is the number of callee-saved registers the target pushes.**
≤6 → the allocator has slack and the assignment falls out of the value structure → winnable.
=8 (r4–r11 all live, plus stack spills) → the allocator is saturated, the spill choice is
internal, and **no declaration-level source rewrite changes it** (43 variants tested, 0 effective).

Big functions are NOT uniformly hopeless. They are two populations, and current workers fail
because they attempt the wrong one. Of the 968 functions ≥600B, **594 (61%) push all 8
callee-saved registers** — that is the 0/20 population. **65 are GREEN** by the rule below.

---

## 2. THE RULE — triage BEFORE you write any C

Run `python SP/inv/method/triage.py [minbytes]` (defaults to 0x258). It emits one line per
undecompiled function ≥ minbytes with the two decisive numbers. Or read them off the target
disasm by hand:

| read from target | GREEN (attempt) | AMBER (attempt only if the other axis is good) | RED (**SKIP ON SIGHT**) |
|---|---|---|---|
| **`sav` = # of r4–r11 in the opening `stmdb sp!`** | ≤ 6 | 7 | **8** |
| **`big%` = largest basic block ÷ total instructions** | ≥ 0.35 | — | < 0.35 |
| verdict | both GREEN | exactly one | neither |

Counts over the 968 undecompiled functions ≥600B: **GREEN 65 · AMBER 330 · RED 573.**
(Overlay-only, i.e. gateable with `wgate.py`: 43 GREEN.)

### Why these two numbers

* **`sav`=8 means the register allocator is saturated *and spilling*.** Every long-lived value
  competes; which one loses is decided inside mwcc, and a single different spill choice rotates
  the whole assignment (see §5). `sav`≤6 means at least two callee-saved registers are spare, the
  allocation is forced by the value structure, and recipes #9/#11 apply as written.
* **`big%`≥0.35 means "long but simple"** — a few enormous straight-line blocks (bitfield packing,
  long call chains). You transcribe those; there is no control-flow puzzle. `big%`<0.35 means
  30–50 tiny blocks — a control-flow *reconstruction* problem on top of everything else.

**Validation on the brief's own candidate list** — every one of the 5 suggested targets except
`02168b44` is RED, and RED is exactly where I stalled:

```
GREEN ov004 func_ov004_02168b44 1536 377 blk=13 big%=0.58 sav=6   <- MATCHED
GREEN ov000 func_ov000_02166e5c 1248 302 blk=11 big%=0.54 sav=6   <- 97.2% (2-reg permutation)
RED   ov004 func_ov004_0215f0c0 1588 389 blk=27 big%=0.17 sav=8   <- stalled on global spill
RED   ov004 func_ov004_02161514 1796 443 blk=40 big%=0.15 sav=8
RED   ov004 func_ov004_0215f770 1676 401 blk=34 big%=0.20 sav=8
RED   ov004 func_ov004_02162108 1636 400 blk=39 big%=0.14 sav=8
```

---

## 3. THE RECIPE — what actually worked (follow in order)

Answer to the brief's first question up front: **block-by-block convergence is NOT viable and must
not be attempted.** Frame size, the callee-saved push set, and register assignment are all
*whole-function* properties. A partially-written function compiles to a different prologue, a
different frame, and different registers, so a per-block byte-diff never converges and its
"progress" is noise. What works is **whole-function decode → whole-function C → aligned diff**.
And the second question: yes — split the target into labelled blocks and account for **every**
instruction *before* writing a line of C. On 02168b44 that made the first compile 80% correct.

### Step 0 — triage (§2). RED → SKIP, write the addr off, move on.

### Step 1 — block map (2 min)
```
grep -rhA400 "^func_ovNNN_ADDR:" REPO/build/usa/asm/ovNNN/ > T_ADDR.txt   # trim at arm_func_end
python SP/inv/method/blocks.py T_ADDR.txt
```
Prints per block: instruction count, `PRED≤5 / BRANCH≥6 / CALL-never-pred` (recipe #3 decided for
you), successors, and the callees. This is the control-flow skeleton you must reproduce.

### Step 2 — resolve every callee BEFORE writing C
`grep -o "bl [_a-zA-Z0-9]*" T_ADDR.txt | sort -u`, then for each:
* `_Z…` mangled → already decompiled. `grep -rhoE "ARM [^;{]*\bNAME *\([^)]*\)" src/ include/`
  gives the exact signature — **copy it verbatim**, do not re-derive types.
* `func_ovNNN_xxxx` → `grep '^func_ovNNN_xxxx ' config/usa/arm9/**/symbols.txt`.
  Present → `extern "C"` the raw name. **Absent → it is decompiled+renamed**; find the real name
  and call that (this bit me on `func_ov004_02168ad4` → `GetNodeIfType11_02168ad4`).

### Step 3 — decode the whole body on paper first
Walk the disasm top to bottom and write down, per instruction, the C it came from. Do not skip the
"hard" middle. Specifically resolve:
* **frame layout** — every distinct `[sp,#N]` and `add rX, sp, #N`. Sum them; they must equal
  `sub sp,#K` minus the outgoing-arg area. On 02168b44 that was
  `4 (outgoing) + 4 + 12 + 0xb0 = 0xc4` and it confirmed the buffer size before I compiled once.
* **bitfield structs** — `ldr; bic #M; orr rX, rY, lsl/lsr` is a bitfield insert. Recover the
  *width and bit position* of both source and destination fields; that is what generates the
  shift amounts. mwcc materialises one mask constant and derives the others by shifting it
  (`r8 = -0x4000`, then `r8 lsr#18` = 0x3fff, `r8 lsr#22` = 0x3ff …) — you get that for free once
  the field widths are right; do not try to write the masks.
* **address offsets** — write the TOTAL offset; recipe #4's `canon()` reproduces the split. Verified
  again here: `bs+0x74c0` → `add #0x34c0; add #0x4000`; `r4+0x3b84` → `add #0x3000; ldr [,#0xb84]`.
* **repeated global loads** — if the target re-loads `ldr rX,pool; ldr rY,[rX,#0]` before *every*
  use, the source really does repeat the expression. Write it repeated. (02168b44 re-reads
  `data_ov004_02171030->f194` 14 times in a row.)

### Step 4 — write the WHOLE function, then diff
```
python SP/inv/method/adiff.py <abs path to.cpp> <abs path to T_ADDR.txt> [maxhunks] [--blind]
```
Prints `TGT=n MINE=n MATCHED=n FIRSTDIFF=i` and every differing hunk side-by-side with target
indices. It normalises same-encoding aliases, masks relocs/pool loads, and resolves branch targets
to instruction indices so branch *structure* is compared.

**Use two metrics, they mean different things:**
* `MATCHED` (exact) — everything, including register names.
* `MATCHED` with `--blind` (register names erased) — instruction *shape* only.

| exact | blind | diagnosis | what to do |
|---|---|---|---|
| low | low | your C is wrong | fix the semantics — read the hunks |
| low | ~100% | only register allocation is wrong | §5 — usually terminal |
| ~100% | ~100% | you are 1–2 idioms away | fix them, then `wgate` |

`MINE` vs `TGT` instruction count is the sharpest single signal: being **one instruction short**
almost always means a missing type conversion (see §4).

### Step 5 — isolate every disagreement into a 5-line lab
Do **not** iterate on the 400-instruction file. Extract the disagreeing statement into
`lab.cpp` with 4–6 one-line variants and `bash SP/try.sh lab.cpp`
(or `python REPO/ra/odis2.py lab.o` to see *all* functions in one object). Two of my three
blockers fell in one lab each. Then re-apply to the real file.

### Step 6 — bulk-test hypotheses with `sweep.py`
```
python SP/inv/method/sweep.py <base.cpp> <target.txt> <variants.py>
```
`variants.py` is a list of `(name, [(find, replace), …])`. Prints per variant:
exact match, blind match, frame size, prologue. Use it the moment you have >2 hypotheses —
7 variants cost one command instead of seven edit/compile/read cycles.

### Step 7 — `python SP/wgate.py OV ADDR file.cpp` → `MATCH`. Nothing else counts.

### Worked before/after (02168b44, the two fixes that finished it)

```c
/* BEFORE — 365/384, and one instruction SHORT */          /* AFTER — MATCH */
extern "C" void* func_ov011_021845f8(void* ctx);            extern "C" void* func_ov011_021845f8(void* ctx, int v);
void* holder = func_ov011_021845f8(a);                      void* holder = func_ov011_021845f8(a, 0);
unsigned char bit = 0;                                      int bit = 0;
if (c) bit = c->f150->bit0;                                 if (c) bit = c->f150->bit0;
…->f194->f16 = bit;                                         …->f194->f16 = bit;
```
* Arg-count tell: target had `ldr r2,pool / mov r1,#0 / str r1,[r2]` where I had the pool in r1 and
  the zero in r2. r1 was **pinned as an ABI argument register** — the callee takes 2 args and the
  `0` serves double duty as the stored value. Scratch-register *inversion* around a call is an
  arg-count signal (recipe #11), not a decl-order problem.
* One-instruction-short tell: target `and r0,r5,#0xff` then `and r0,r0,#0x1`; I emitted the folded
  `and r0,r5,#1`. **Storing an `int` into an `unsigned char`-based bitfield emits the u8 conversion
  AND the field truncation as two separate `and`s; storing an already-`unsigned char` value folds
  them into one.** So the ROM's variable was `int`.

---

## 4. NEW IDIOMS FOUND (lab-verified, reusable)

**A. `unsigned` narrowing is defeated by an `int` temp — `moveq` vs `movle`.**
```c
if (p->f130->count <= 0) idle = 1;              // -> cmp; MOVEQ   (mwcc narrows unsigned <=0 to ==0)
int n = p->f130->count; if (n <= 0) idle = 1;   // -> cmp; MOVLE   (matches ROM)
```
Verified `lab2/lab3/lab4.cpp`. Applies to any `<=0` / `>1` on an `unsigned short`/`unsigned char`
field. If the ROM has a **signed** condition code on a value loaded with `ldrb`/`ldrh`, route it
through an `int` local. (Direct one-level access sometimes keeps `movle`; a chained
`a->b->c` access always narrows. Use the `int` temp — it is correct either way.)

**B. int→narrow-bitfield stores emit TWO `and`s; narrow→narrow emits one.** (§3 step 4 above.)
Use it in both directions: two masks in the target ⇒ the source variable is `int`; one mask ⇒ it is
already the bitfield's base type. Confirmed twice (`02168b44` bit0/`and #0xff`+`and #0x1`;
`02166e5c` 3-bit→4-bit/`and #0xff`+`and #0xf`).

**C. `int`→`short` at a call site is ALWAYS truncated (`lsl#16; asr#16`) — no exceptions.**
Labs `lab5/lab6/lab7.cpp`: 9 forms tried (direct, via `int` temp, via `short` temp, 7-bit-bitfield
source, `|` instead of `+`, tail vs non-tail call, `unsigned short` param). Only declaring the
parameter `int` removes it. So if the ROM passes `ldrb; add #K` straight into an `s`-mangled
parameter with no `lsl/asr`, the call site's prototype was **not** `short`. Fix with a verbatim
mangled extern (recipe #12): `extern "C" void _Z27…Pvis(void* a, int key, int value);`

**D. Pointer + index + constant: association order is source-visible.**
```c
tab[0xf78 + n] = 1;      // add r2,r5,#0x378; add r2,r2,#0xc00; strb r3,[r0,r2]   (3 instrs)
(tab + n)[0xf78] = 1;    // add r0,r0,r5; strb r2,[r0,#0xf78]                     (2 instrs, = ROM)
```
Constant-folds with the *index* if you write it inside the brackets; lands in the load/store
immediate if you write `(base + index)[constant]`. This is the index-register companion to
recipe #4 and is worth an instruction every time.

**E. A pending ABI argument pins its register through an inlined block copy.**
`struct B4 { char b[4]; } tmp = g;` compiles to `ldrb rS,[rSRC],#1 / subs rN,rN,#1 /
strb rS,[rDST],#1 / bne`. The three registers are taken from r0,r1,r2,r3,**r4**… *skipping any
register already holding a set-up call argument*. Target used r2/r3/**r4** with `mov r1,#2`
hoisted above the loop; I used r1/r2/r3. Proven by substituting a live variable for the literal
`2` — the loop instantly moved to r2/r3/r4 (`variants7.py` `livearg`). Diagnostic value: if a
block copy's registers are one lower than the ROM's, an argument is materialised earlier in the
ROM than in your source.

---

## 5. WHERE IT STOPS — global register allocation (the real wall)

Both near-misses died in the same place, and it is **not** a recipe-#9 band problem.

**`0215f0c0` (RED, sav=8).** Exact instruction count reproduced (397/397), blind 319/397. Residual:
the ROM spills `self`+`frame` and keeps `info` in r11; mine spills `info` and keeps `self`+`frame`
in registers. That one different spill decision shifts every callee-saved register by one, changes
the frame from 0x8c to 0x90, and adds the alignment `r3` to the push list.
**21 source variants**: declaration order, declaration scope (block vs function-top), pointer vs
`int` types, `void*` + cast-at-use, splitting a live range through a second named pointer,
CSE-vs-recompute for each pointer, hoisting the array initialiser, reordering the temp.
**Every single one produced byte-identical output** (198 exact / 319 blind, unchanged).
Only *diagnostics that add a genuine extra long-lived value* moved it — extending `info`'s live
range to the end of the function immediately produced `sub sp,#0x90`, the `r3` push, `mov r4,r0`,
and 231 exact / 335 blind. That proves the mechanism (the ROM has one more value competing) and
simultaneously proves there is no legal lever: the ROM's `info` is provably used only twice
(`grep -n r11` → 3 lines), so I cannot lengthen it honestly.

**`02166e5c` (GREEN, sav=6) — 35/1248 bytes.** Only `bs` (call-defined, r7 in ROM) and
`sub = base+0x2cc` (computed, r6 in ROM) are swapped. Recipe #9 predicts call-below-computed,
which is what I get and what the ROM contradicts — i.e. this is recipe #9's documented
"mixed pair, target has COMPUTED lower → no C form" case. **22 variants** tried: decl swap,
forward declaration, deriving each pointer from the other, `void*`/`int`/`unsigned char*`/reference
types, inlining either pointer, shortening either live range, re-fetching `bs` at its last use,
reordering the two first uses. All 295/312. Two class-flip diagnostics (making `sub`
call-defined; making `bs` computed) **also failed to move it**, which means the assignment here is
not decided by def class at all — so recipe #9's rule does not generalise to functions this size.

**Consequence for recipes #9/#11:** they were derived on small labs and hold there. At ≥300
instructions with ≥6 live callee-saved values, register assignment is decided by mwcc's global
allocator and is **not a function of anything a decompiler can write**. Do not grind it.

---

## 6. ATTEMPT BUDGET AND POINT OF NO RETURN

Measured, per GREEN function:

| phase | cost | leaves you at |
|---|---|---|
| triage + block map | ~5 min | go/no-go |
| callee signature resolution | ~10 min, 1–2 greps per callee | — |
| full decode + first draft | the bulk of the work | **75–90% of instructions correct** |
| adiff-driven fixes | 3–6 revisions | MATCH or a named blocker |

* **The first compile is the checkpoint.** 02168b44 → 301/377 (80%). 02166e5c → 284/312 (91%).
  0215f0c0 → blind 305/397 (77%) but exact 198 (50%).
  **If the first compile is below ~70% blind, your decode is wrong — re-read the disasm, do not
  start permuting source.** If it is above 85% blind, you will almost certainly finish.
* **Point of no return: when `--blind` is ≥97% and exact is not.** That is pure register
  allocation. Give it **at most 6 variants via `sweep.py`, in one batch**. If none of them changes
  the output *at all* (identical exact+blind score), stop — you have hit §5 and no further variant
  will help. Both of my near-misses showed 5+ consecutive zero-effect variants before I stopped;
  that signature ("N variants, byte-identical output") is the reliable abort condition.
* Never exceed ~10 revisions on one large function. The cost of a wrong SKIP is one address;
  the cost of grinding a RED function is a whole worker session.

---

## 7. WHEN TO SKIP (proven, add to the worker prompt)

1. **`sav`=8 (target pushes all of r4–r11).** 594 of the 968 ≥600B functions. The allocator is
   saturated and spilling; the spill choice is not source-controllable (43 variants, 0 effective).
   **SKIP ON SIGHT** regardless of how simple the code looks.
2. **`big%`<0.35 AND `sav`≥7.** 573 functions. Control-flow reconstruction *and* a saturated
   allocator.
3. **Block-by-block / incremental construction of a large function.** Not a target class but a
   method: it cannot work, because the prologue, frame size and register assignment are
   whole-function properties. Never partially write a big function "to check the first block".
4. Everything already in the worker prompt's PROVEN-NO-C-FORM list still applies.

---

## 8. ARTIFACTS

In `SP/inv/method/`:

| file | what |
|---|---|
| `triage.py` | ranks every undecompiled function ≥N bytes GREEN/AMBER/RED. `python triage.py [minbytes]` |
| `triage_all.txt` | the full 968-row ranking, ≥600B |
| `blocks.py` | basic-block map of a target: sizes, predicate/branch verdict, successors, callees |
| `adiff.py` | aligned instruction diff, exact + `--blind`. The core tool |
| `sweep.py` | batch-compile N source variants, report exact/blind/frame/prologue |
| `lab1..lab7.cpp` | the idiom labs behind §4 |
| `variants*.py` | the 43 register-allocation variants behind §5 |

**Deliverable functions** (left in this directory per the no-writes-to-`src/` rule):

* **`InitBattleSceneContext_02168b44.cpp` — MATCHES.** `python SP/wgate.py 004 02168b44 <file>` →
  `MATCH`. Ready to move to `REPO/src/Combat/Overlay_4/`. Carries `// USA: func_ov004_02168b44`.
* `NEARMISS_ov000_02166e5c.cpp` — 35/1248 bytes, blocked on the r6/r7 permutation (§5). Correct C
  otherwise; keep it in case a future finding cracks mixed-band allocation.
* `NEARMISS_ov004_0215f0c0.cpp` — structurally complete, blocked on the spill choice (§5).

## 9. RECOMMENDED WORKER-PROMPT PATCH

> **Before attempting any function ≥600B, run `python SP/inv/method/triage.py` and find your
> address.** RED → SKIP immediately, no exceptions. GREEN/AMBER → follow
> `SP/inv/method_FINDINGS.md` §3: block-map first, resolve every callee signature second, decode
> the entire body third, write the whole function fourth, and converge with
> `adiff.py … --blind`. Never write a large function block by block. If `--blind` reaches ≥97%
> and exact does not move across 6 `sweep.py` variants, SKIP.
