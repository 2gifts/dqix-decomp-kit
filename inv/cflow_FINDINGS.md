# CONTROL FLOW FINDINGS — switches, jump tables, loops
> **Measured on mwccarm 2.0/sp1p5.** The ROM now builds with 2.0/sp2p2 and `-str pool,reuse`
> ([[dqix-sp2p2-rebase]]). The labs under `SP/lab/` and `SP/pad/` follow the build again, so a
> rule here is worth re-measuring before it is cited as settled.

plain -O2, `-lang=c++ -enum int -char signed -proc arm946e`

## VERDICT

**YES — a fully controllable lever exists, and it is the strongest one found so far: for both
switches and loops the emitted shape is a PURE FUNCTION of the C source. There is nothing
stochastic to fight.**

* **SWITCH**: the dispatch shape is decided by the *set of case values alone* (count + span).
  Nothing else touches it — not the index type, not signed/unsigned, not `default:` presence,
  not any pragma, not `-Os`. You can **read the exact case-value set off the target's jump table**
  and the switch you write back is byte-identical. Verified end-to-end: **4 real unmatched
  functions taken to `MATCH`** (§EVIDENCE), one of them a 28-case / 64-entry split dispatch.
* **LOOP**: mwcc emits exactly one loop skeleton (`b <test>` at entry, body, bottom test). It
  **never rotates, never peels the guard, never unrolls, never strength-reduces an index into a
  pointer.** Every ARM loop variation a worker sees is therefore a direct 1:1 image of a C
  choice: `for`-vs-`do`, `<`-vs-`!=`, signed-vs-unsigned, index-vs-pointer.
* Worker SKIP reasons citing "14-way jump table" / "8-way+4-way nested switch" / "nested loops"
  are **not real blockers** — they are the most mechanically reproducible constructs in the ROM.

### The measurement the brief asked for (the "0 jump tables" detector was wrong)

`ldr pc` / bare `add pc` returns ~0 because **mwccarm never emits a data-word address table in
ARM mode**. It emits a *branch* table: an `addCC pc, pc, rX, lsl #2` followed by a run of `b`
instructions living inside the function's own `.text` (and inside its size slot — `wgate`'s size
check covers it, no separate `.rodata`).

| scope | number |
|---|---|
| ARM dispatch sites (`addCC pc, pc, rX, lsl #2`) in `build/usa/asm` | **442** |
| Thumb dispatch sites (`add pc, rX` + `.short` table) | 18 |
| Unconditional `add pc, pc, rX, lsl #2` (hand-asm, see SKIP) | 2 |
| **still-unmatched functions containing ≥1 jump table** | **346** (411 tables, 4269 case labels) |
| of the 2631 unmatched >256B funcs, how many have a table | 276 |
| backward branches (loops) in still-unmatched ARM funcs | **5818** |

---

## THE RULE — SWITCH

### R1. Table-vs-chain (the only threshold that exists)

Let `V` = sorted distinct case values, `k = |V|`, `span = max(V) - min(V) + 1`.

> **mwcc emits ONE dense jump table iff `k >= 4` AND `span <= floor(5*k/2)` AND V has no large
> internal hole that makes a split cheaper. Otherwise: compare chain (small k) or a binary
> partition whose leaves are tables/chains.**

`floor(5k/2)` is exact, verified for every `k` in 4..16 crossed with every `lo` in
{0,1,3,4,5,9,40,200} — **104/104 boundaries hit exactly, contiguous, no exceptions**:

| k | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| max span for one table | 10 | 12 | 15 | 17 | 20 | 22 | 25 | 27 | 30 | 32 | 35 | 37 | 40 |

`k` counts **case LABELS, not distinct bodies** — `case 0: case 1: case 2: case 3: body;` is k=4
and gets a 4-entry table with four identical entries.

### R2. The four dispatch encodings — read the target, know the case set

| target instruction sequence | meaning | C |
|---|---|---|
| `cmp rX,#N` · `addls pc,pc,rX,lsl#2` | one table, `lo` is 0..3, cases `0..N` | plain `switch`, min case ≤ 3 |
| `sub rT,rX,#LO` · `cmp rT,#N` · `addls pc,pc,rT,lsl#2` | one table, biased, cases `LO..LO+N` | plain `switch`, min case `LO >= 4` |
| `cmp rX,#HI` · `bgt L` · `cmp rX,#0` · `addge pc,pc,rX,lsl#2` | **split**: table covers `0..HI`, **and the switch HAS more cases above HI** handled at `L` | one `switch`, extra high case values |
| `cmp rX,#HI` · `bgt L` · `subs rT,rX,#LO` · `addpl pc,pc,rT,lsl#2` | **split**, biased (`LO >= 4`), more cases above `HI` at `L` | one `switch`, extra high case values |

`bias LO >= 4` is the exact threshold: for `LO` in 0..3 mwcc extends the table down to index 0
(filling `0..LO-1` with default entries) instead of paying for the `sub`. ROM-wide: **all 56
biased `addls` sites and all 9 `addpl` sites have `LO >= 4`; zero counterexamples.**

The two-compare forms (`addge`/`addpl`) exist **only** because out-of-range-high must reach a
*different* place than the default, so the one-compare unsigned `ls` trick is unavailable. If
you see `addge`/`addpl`, the switch positively has case values above the table.

### R3. Table geometry (how to read the case set off the target)

```
A:      addls pc, pc, rX, lsl #2      <- pc reads as A+8
A+4:    <default>                     <- ONE instruction: `b default`, or the epilogue inlined
A+8:    b case(LO+0)                  <- entry 0
A+12:   b case(LO+1)
...     entry i  <=>  case value  LO + i
```
* `LO` = the `sub` immediate, else 0. `N` = the `cmp` immediate; the table has `N+1` entries.
* A slot that contains `b <the default label>` (or the inlined return) is a **hole** — that case
  value is *not* in your switch.
* Two slots with the same target = `case A: case B:` sharing one body.
* `A+4` holding `ldmia sp!,{...,pc}` / `bx lr` instead of `b` just means the default is a bare
  return; those return instructions also appear inline in hole slots. Same thing.

### R4. Case-clause ORDER in the source

**Table entries are emitted in numeric order; case BODIES are emitted in source order.**
Read the body addresses off the target: sort them ascending, and that is the order your `case`
clauses must appear in the C source. If your table entries are right but 2 entries point at
swapped bodies, swap those two `case` clauses. (This was the only diff on `func_ov015_0218f1c4`;
swapping `case 2` and `case 3` in the source turned BYTEDIFF into MATCH.)

### R5. Non-levers (stop trying these)

Verified no effect on switch shape whatsoever:
`int` / `unsigned int` / `short` / `unsigned short` / `char` / `unsigned char` / `long` index
(all byte-identical) · presence or absence of `default:` · `#pragma optimize_for_size on|off` ·
`#pragma opt_switch_tables off` · `#pragma opt_loop_invariants` · `#pragma opt_unroll_loops`.

### R6. Thumb (`#pragma thumb on`, 18 sites, all ov031)

Same k>=4 threshold, half-word offset table:
```
cmp r0,#N · bhi default · adds r0,r0,r0 · add r0,pc · ldrh r0,[r0,#6]
lsls r0,r0,#0x10 · asrs r0,r0,#0x10 · add pc,r0 · <.short (target-tablebase-2) x (N+1)>
```

---

## THE RULE — LOOPS

### L1. The one skeleton

mwcc -O2 emits exactly one loop form and **never rotates it, never peels/hoists the guard**:

```
        <init>
        b   .Ltest          <-- top-tested loops ALWAYS have this entry branch
.Lbody: <body>
        <increment>
.Ltest: cmp ...
        bCC .Lbody
```

`do { } while` is the same minus the entry `b`. **So: entry `b` present ⇒ `for`/`while`;
entry `b` absent ⇒ `do{}while`.** That single bit is the whole `for`-vs-`do` decision.

### L2. Lookup table — target shape → C form

Counts are backward branches in the still-unmatched ARM asm (5818 total); the top 8 rows cover 85%.

| target | share | C form |
|---|---|---|
| `b test` · body · `add i,#1` · `cmp i,n` · **`blt`** | 52.7% | `for (int i=0; i<n; i++)` (identical to `int i=0; while(i<n){…;i++;}`) |
| body · `subs r,r,#1` · **`bne`**, **no entry b** | 9.5% | `do { … } while (--n);` — pre-decrement in a do-while is the ONLY thing that merges the decrement and the test into `subs` |
| `b test` · body · `cmp x,#0`/`cmp x,y` · **`bne`** | 7.7% | `for (p = head; p != 0; p = p->next)` / `while (x != k)` |
| `b test` · body · `cmp p,end` · **`blo`** | 6.8% | **unsigned** counter, or a pointer compare `p < end` |
| body · `cmp i,n` · **`blt`**, **no entry b** | 6.4% | `do { … } while (i < n);` |
| body · `cmp x,#0` · **`bne`**, no entry b | 2.5% | `do { … } while (p);` |
| body · `cmp p,e` · **`blo`**, no entry b | 2.0% | `do { … } while (p < e);` |
| body … · unconditional **`b`** backwards | 2.7% | `for (;;) { … }` / `while (1)` with `break`/`return` inside |
| `cmp` · `ble`/`bge`/`bgt` | 2.7% | `<=` / `>=` / `>` written literally; `for (i=n; i>0; i--)` → `sub i,#1; cmp i,#0; bgt` |

### L3. Sub-levers inside the loop body (each is a direct 1:1 image of C)

| target | C |
|---|---|
| `str r,[rP],#4` post-indexed + `cmp rP,rEnd` | **pointer** induction: `for (T* q=p; q<e; q++) *q=…` |
| `str r,[rBase,rI,lsl #2]` scaled + separate `add rI,#1` | **index** induction: `for (i…) p[i]=…` — mwcc does **NOT** strength-reduce, so index in C ⇒ index in asm |
| `lsl rT,rI,#1` then `ldrsh rX,[rBase,rT]` | index into a **short/byte** array (`ldrsh`/`ldrb` cannot take a scaled register offset, so the scale becomes its own `lsl`) |
| `mul rT,rI,rK` with `mov rK,#S` before the loop | index into a struct array with **non-power-of-2 element size S** |
| `cmp p,end` → **`blo`** vs **`bne`** | `q < p+n` gives `blo`; `q != p+n` gives `bne` |
| **`add rE, base, rN, lsl #k`** hoisted before the loop | the bound was written as `p + n` in the condition (`q < p+n`) |
| bound **re-`ldr`'d inside** the loop each iteration | the bound was written as a memory expression: `for (i=0; i < b->count; i++)` — mwcc does **not** hoist it. A hoisted bound means the C used a **local** |
| `and rI,rT,#0xff` after `add rT,rI,#1` | the induction variable is `unsigned char` |
| `cmp rN,#0` · `sub rN,rN,#1` · `bne` (cmp BEFORE sub) | `while (n--) …` (post-decrement: test old value, then decrement) |
| `cmp n,#0` · `b<le>` skip **before** the loop | an explicit `if (n > 0) { for(…) }` in the C — mwcc never generates this on its own |
| `moveq r0,#1; bxeq lr` inside the body | early `return` from inside the loop, ≤5 instrs so it predicates (recipe #3) |
| inner loop starting `mov rJ, rZero` where rZero holds 0 | nested loop: mwcc parks the inner initial value in a callee-saved reg and copies it per outer iteration |

### L4. Relational operand order (minor lever, use when only the `cmp` operands differ)

* Plain locals/params: **source order is preserved** — `a<b` → `cmp a,b; blt`; `b>a` → `cmp b,a; bgt`.
  Both directions are reachable; pick the C form whose left operand is the target's first `cmp` operand.
* `==`/`!=` against an **inline memory load** gets canonicalised (operands come out reversed).
  Hoisting the loaded side into a **named local of the field's exact type** restores source order:
  `if (p[i].id == k)` → `cmp k,field` · `short v = p[i].id; if (v == k)` → `cmp field,k`.
  (This is exactly the trick the committed `FindEntryByShort2_021550c8.cpp` uses.)

---

## EVIDENCE

**Lab** (`SP/inv/cflow/`, all reproducible): `e1.py` case-count/density/no-default sweep ·
`e2.py` density × k=4..16 · `e3.py` split shapes, all 8 index types, shared targets, multi-cluster,
switch-on-expression · `e4.py`/`e5.py` 45 loop forms · `e6.py` 220 randomised switches ·
`e7.py` 104-point density × bias sweep · `e9.cpp` pragma sweep · `tt.cpp` thumb.

* **Density law `span <= floor(5k/2)`: 104/104 exact** across k=4..16 × lo∈{0,1,3,4,5,9,40,200},
  boundaries contiguous. Independent 220-case randomised run: 216/220 agree; **all 4 misses are
  the same known refinement** (sets with a large *internal* hole get binary-partitioned even when
  (k,span) permits a table — e.g. `{5,6,12,15,16}`, k=5 span=12) — never the reverse, i.e. the law
  is never violated in the direction that would make a worker write a wrong table.
* **ROM-wide validation of the dispatch encodings: 442 ARM sites, 0 violations.**
  416/417 `addls` sites satisfy `table_entries == cmp_imm + 1` (1 has the `cmp` outside the
  8-instruction scan window); 56/56 biased `addls` have `LO>=4`; 14/14 `addge` are preceded by
  `cmp #0` *and* a `bgt`; 9/9 `addpl` have a `subs` with `LO>=4` *and* a `bgt`. (`verify_rom.py`)
* **Counterexamples sought and NOT found**: no `ldr pc,[pc,rX,lsl#2]` data-word table anywhere in
  the ROM or from any C I could write; no unrolled loop at -O2 (constant trip counts 2/4/8 all
  stay rolled); no compiler-generated loop guard; no index→pointer strength reduction; no pragma
  or `-Os` that moves the k>=4 threshold.

**Real functions taken to `MATCH` with `wgate.py` using only these rules** (sources left in
`SP/inv/cflow/`, ready to integrate):

| addr | file | shape exercised | tries |
|---|---|---|---|
| `func_ov001_02164194` | `t1.cpp` | 6-entry table, k=4 (`case 0,1,4,5`), holes → default, predicated early return | **1** |
| `func_ov015_0218f1c4` | `t2.cpp` | 7-case `ldrb` dispatch, tail calls, table entries non-monotonic vs bodies | 2 (R4 reorder) |
| `func_ov017_021a3b40` | `t4.cpp` | **28 cases / 64-entry table + `bgt`+`cmp #0`+`addge` SPLIT** (k=28 span=75 > 70 ⇒ split; low cluster k=27 span=63 ≤ 67 ⇒ table; lo=1 < 4 ⇒ extend to 0) | 1 (after fixing 4 callee names) |
| `func_ov025_021e24a8` | `t5.cpp` | top-tested `cmp+bne` list walk with predicated early return | **1** |

Two further reconstructions (`func_ov004_021550c8` counted `unsigned char` loop with `mul` stride;
`func_ov017_021ce2fc` 4-iteration indexed byte copy) came out instruction-identical but their
slots are already decompiled, so `wgate` reports `NO-SLOT`; the committed sources confirm the
derived shapes independently.

---

## RECIPE — SWITCH (mechanical)

1. **Find the dispatch.** `grep -n "add\(ls\|ge\|pl\) pc, pc" <the function's asm>`.
2. **Read `LO`**: the `sub`/`subs` immediate immediately above, else `0`.
   **Read `N`**: the `cmp` immediate. The table has `N+1` slots and starts **2 instructions after**
   the `addCC pc` (the instruction in between is the default).
3. **Read the case set.** Walk the slots; slot `i` is case value `LO + i`. A slot branching to the
   default label (or holding the epilogue) is a hole → omit that case.
4. **If you saw `addge`/`addpl`**: the switch has MORE cases. Follow the `bgt` target and read the
   compare chain there (`cmp x,#V; beq …`) — each `beq` is one more `case V:`. Add them to the
   same `switch`.
5. **Order the clauses.** List each case's body address; sort ascending; write the `case` clauses
   in that order (R4). Numeric order is irrelevant — mwcc re-sorts the table for you.
6. Write the plain `switch`. **Do not** try to force the table with casts/masks/pragmas.
7. Sanity-check before compiling: `k >= 4` and `span <= (5*k)/2` must hold for the cluster you
   read, and `LO >= 4` iff the target had a `sub`. If they do not, you mis-read the table.
8. `wgate`. A remaining diff **inside the table** ⇒ your case set or clause order is wrong (go to 3);
   a diff only in the bodies ⇒ ordinary codegen, route to the usual recipes.

### Worked example — `func_ov001_02164194`

```
    cmp r3, #0x0
    moveq r0, #0x0
    bxeq lr
    str r1, [r0, #0x0]
    cmp r1, #0x5                  <- N = 5, no `sub` => LO = 0, 6 slots
    addls pc, pc, r1, lsl #0x2
    b .L_021641d8                 <- default
.L_021641b0: ; jump table
    b .L_021641c8 ; case 0        }
    b .L_021641c8 ; case 1        }  -> same body: `case 0: case 1:`
    b .L_021641d8 ; case 2           -> == default => HOLE, not a case
    b .L_021641d8 ; case 3           -> HOLE
    b .L_021641c8 ; case 4        }
    b .L_021641c8 ; case 5        }
```
case set = {0,1,4,5}: k=4 ✓, span=6 ≤ 10 ✓, LO=0 (<4 ⇒ no `sub`) ✓ — all consistent.

```c
struct Rec02164194 { int kind; int a; int pad; int b; };

// USA: func_ov001_02164194
ARM int StoreRecordIfKindAllowed_02164194(Rec02164194* out, int kind, int a, int b) {
    if (b == 0) return 0;
    out->kind = kind;
    switch (kind) {
    case 0: case 1: case 4: case 5:
        out->a = a;
        out->b = b;
        return 1;
    }
    return 0;
}
```
→ `MATCH`, first try.

## RECIPE — LOOPS (mechanical)

1. **Locate the loop head** = target of the backward branch. **Is the instruction just above the
   head an unconditional `b` forward into the test?** yes ⇒ `for`/`while`; no ⇒ `do { } while`.
2. **Read the terminator** (`cmp`+`bCC` / `subs`+`bne`) and pick the row from the L2 table.
   `blt`→signed `<` · `blo`→unsigned `<` or pointer `<` · `bne`→`!=` or `p` truthiness ·
   `ble`/`bge`/`bgt` → write `<=`/`>=`/`>` literally.
3. **Read the induction form from the body's addressing**, per L3: post-indexed `[rP],#S` ⇒ write a
   **pointer** loop; scaled `[rB,rI,lsl#k]` ⇒ write an **index** loop. Do not convert between them —
   mwcc will not.
4. **Bound**: `cmp rI, rReg` with `rReg` set before the loop ⇒ a local/param; `ldr` of the bound
   *inside* the loop ⇒ write the memory expression directly in the condition.
5. If the target has a `cmp n,#0` + conditional skip **before** the `b test`, add an explicit
   `if (n > 0) { … }` around the loop — mwcc will never produce that guard on its own.
6. `wgate`.

### Worked example — the `subs`+`bne` copy idiom (`func_02085d14`, 555 sites ROM-wide)

```
    mov r1, #0x4
.L: ldrh r0, [r3], #0x2
    subs r1, r1, #0x1
    strh r0, [r2], #0x2
    bne .L
```
no entry `b` ⇒ `do{}while`; `subs`+`bne` ⇒ pre-decrement; post-indexed ⇒ pointer form:
```c
int i = 4;
do { *d++ = *s++; } while (--i);          /* unsigned short* s  =>  ldrh; short* => ldrsh */
```
The near-miss forms, for contrast (all verified):
`while (n--) *d++=*s++;` → `b test` + `cmp r,#0; sub r,r,#1; bne` (3 instrs, entry branch) ·
`for (i=0;i<4;i++) *d++=*s++;` → `b test` + `add r,#1; cmp r,#4; blt`.

---

## WHEN TO SKIP (proven no C form / not worth grinding)

* **Unconditional `add pc, pc, rX, lsl #2`** (2 sites, both in `main_828`, preceded by
  `add r2,r2,r2,lsl#1` inside the runtime divide). This is the hand-written unrolled-division
  computed branch. No C form. SKIP on sight.
* **`ldr pc, [pc, rX, lsl #2]` / any `.word` address table.** Does not exist in this build. If you
  think you see one, it is hand-asm — SKIP.
* **`ldr pc, [sp], #4`** (4 sites) and `subs pc, lr, #4` — hand-written prologue/exception returns.
  SKIP.
* **A jump table whose case set violates R1** (`k < 4`, or `span > floor(5k/2)` with no internal
  hole to justify it): you mis-read the table. Re-read it — do **not** start trying source variants.
* **Thumb tables outside ov031** — the 18 thumb sites are all in ov031; needs `#pragma thumb on`
  per worker note 11, and everything else in this document still applies.
* A loop whose shape you cannot place in the L2 table after reading L3 **is not a loop-shape
  problem** — it is scheduling or register allocation inside the body. Route to recipes
  #9 / #11 / sched, or SKIP per the existing "load/store SCHEDULING differences" rule.

## SUGGESTED ADDITION TO `worker_ov_all.md`

> **15. SWITCH / JUMP TABLE — read it, don't guess (SP/inv/cflow_FINDINGS.md).** `addCC pc,pc,rX,lsl#2`
> + a run of `b` = a switch, and the table is inside the function's size slot. `LO` = the `sub`
> immediate (0 if none), `N` = the `cmp` immediate, table starts 2 instructions after the `addCC pc`,
> slot *i* = case `LO+i`, a slot pointing at the default is a HOLE. `addls` = plain switch;
> `bgt`+`cmp #0`+`addge` or `bgt`+`subs`+`addpl` = the switch ALSO has cases above the table —
> follow the `bgt` and read the `cmp/beq` chain. Table exists iff k>=4 and span<=floor(5k/2);
> `sub` bias iff min case >= 4. Case BODIES emit in SOURCE order, table entries in NUMERIC order —
> if two entries point at swapped bodies, swap those two `case` clauses. Index type, `default:`,
> and every pragma are NO-OPS. Never force it with casts or masks.
>
> **16. LOOPS — the entry `b` is the whole `for`-vs-`do` bit.** mwcc never rotates, peels, unrolls,
> or strength-reduces. `b <test>` above the loop head ⇒ `for`/`while`; none ⇒ `do{}while`.
> `blt`=signed `<`, `blo`=unsigned/pointer `<`, `bne`=`!=`. `subs r,r,#1; bne` with no entry branch
> ⇒ `do{…}while(--n);` (555 sites). `cmp r,#0; sub r,r,#1; bne` ⇒ `while(n--)`. Post-indexed
> `[rP],#S` ⇒ POINTER loop; scaled `[rB,rI,lsl#k]` ⇒ INDEX loop — they never interconvert. A bound
> re-loaded each iteration means the C wrote the memory expression in the condition; a `cmp n,#0`
> guard before the loop means the C had an explicit `if (n>0)`.
