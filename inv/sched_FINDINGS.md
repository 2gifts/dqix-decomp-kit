# SCHED FINDINGS — if-conversion (predication), address CSE, and load/store scheduling
> **Measured on mwccarm 2.0/sp1p5.** The ROM now builds with 2.0/sp2p2 and `-str pool,reuse`
> ([[dqix-sp2p2-rebase]]). The labs under `SP/lab/` and `SP/pad/` follow the build again, so a
> rule here is worth re-measuring before it is cited as settled.

Plain `-O2 -proc arm946e`. Lab: `SP/inv/sched/`.

## VERDICT

| question | verdict |
|---|---|
| **When does mwcc predicate instead of branching, and can C steer it?** | **YES — fully mechanical.** A hard instruction-count threshold. Exact rule + the one pragma lever below. |
| **When does it reuse a mutated register vs recompute an address?** | **YES — fully mechanical.** Decided by one thing: is the address in a *named local* or written inline. |
| **The `0216dad0` 8-byte diff** | **NO lever — SKIP.** It is a load/store *scheduling* difference, not predication. Only 3 schedules are reachable for that block shape and the ROM's is not one of them. ~510 source variants + all 24 compiler builds + all 79 optimizer pragmas. Proof below. |
| **The `0215b7f8` `+0x1f80` recompute** | **SOLVED** by RULE 2. Reproduced the ROM's exact instruction sequence. |

---

# RULE 1 — IF-CONVERSION (predicate vs branch). THE COUNT IS 5.

**mwcc if-converts a block iff the block is ≤ 5 instructions.** Count instructions in the
**TARGET disassembly**, not in your C. Nothing else matters — not statement count, not `if` vs
ternary, not side effects, not store width, not `volatile`.

## Lookup table — read the ROM, pick the row

| shape in the ROM | rule | what to write |
|---|---|---|
| `if (c) { …block… }` **falls through** | predicated **iff block ≤ 5 instrs**; ≥ 6 → real branch | nothing to do — write the natural `if`; the count decides |
| `if (c) { …block… return K; }` **then `return J`** | the two sides are scored **independently**; the `return J` side is only `mov r0,#J ; bx lr` = **2 instrs**, so it is predicated (`movCC r0,#J ; bxCC lr` / `ldmCCia sp!,{…,pc}`) **however big the other side is** | write `if (c) { … return K; } return J;` |
| `if (c) {T} else {E}` | both predicated **iff T ≤ 4 AND E ≤ 5**. If T ≥ 5: T branches, E still predicated if E ≤ 5 | see the matrix below |
| block contains a `bl` | **never** predicated | — |

### `if/else` matrix (verified, T = then-block instrs, E = else-block instrs)

```
        E=2  E=3  E=4  E=5  E=6  E=7
 T=2     P    P    P    P    B    B
 T=3     P    P    P    P    B    B
 T=4     P    P    P    P    B    B
 T=5     B    B    B    B    B    B
 T=6     B    B    B    B    B    B
 T=7     B    B    B    B    B    B
```
(`B` at T=5,E=2 still predicates the *else* side: `ldrne/strne` + `bne` over the then-block.)

**Each side is scored independently against 5** — the `T ≤ 4` column is the only asymmetry (a
then-block of exactly 5 predicates when there is no `else`, but branches when there is one).
Verified on the "other side" too: `return x*3+7;` (3 instrs) predicates; `return GI[7]+GI[8]+GI[9];`
(7 instrs) branches; a 7-instruction fall-through tail branches. A `bl` anywhere in a side forces a
branch at every size (checked k = 1, 2, 3).

## The only lever: `#pragma optimize_for_size off` lowers the threshold 5 → 3

| pragma state | predicated iff block ≤ |
|---|---|
| default (plain `-O2`) | **5** |
| `#pragma optimize_for_size off` | **3** |

Measured directly (block sizes 2…7):

```
default:                  2 P   3 P   4 P   5 P   6 B   7 B
optimize_for_size off:    2 P   3 P   4 B   5 B   6 B   7 B
```

> This corrects worker recipes #3/#12, which say the pragma "forces a branch". It does not —
> it only moves the cutoff from 5 to 3. For a **4- or 5-instruction** ROM block that branches
> while you predicate, the pragma **is** the fix. For a **≤ 3-instruction** block that branches
> in the ROM, there is **no lever** — SKIP.

**Nothing else moves the threshold.** Swept all 79 internal optimizer pragmas × {on,off}: only
`opt_repositioncode on` changed anything, and it reorders whole blocks (breaks everything else).
`-proc` (arm7tdmi/920t/9tdmi/940t/966e) does not move it either.

## RECIPE 1 (mechanical)

1. In the target disasm, find the conditional region. Count the instructions between the `cmp`
   and the merge point — **include** the pool `ldr`, the `mov rN,#K` for a return value, and a
   predicated `pop`/`bx`.
2. Look the count up in the table above.
3. **You get a branch, ROM predicates** → your block is too big. Shrink it: hoist a shared pool
   load out of the `if`, merge duplicated stores, or drop a temp so the block lands ≤ 5.
4. **You predicate, ROM branches** → block is 4–5 → add `#pragma optimize_for_size off` directly
   above the function. Block is ≤ 3 → **SKIP**, no C form.
5. `bl` inside the block ⇒ always a branch; if the ROM predicates there, your call is spurious.

### Worked example — `func_ov004_0216dad0`, every conditional explained

```
cmp r5,#1 / cmpne r0,#1 / ldreq / moveq / streqb / moveq / ldmeqia    block = 5  -> PREDICATED  (<=5)
cmp r0,#4 / bne .L_0216db48   [ldr,mov,strb,cmp,moveq,streqb,mov,pop] block = 8  -> BRANCH      (>=6)
cmp r0,#2 / ldreq / moveq / streqb / moveq / ldmeqia                  block = 5  -> PREDICATED
cmp r5,#2 / ldreq / moveq / streqb / moveq / ldmeqia                  block = 5  -> PREDICATED
cmp r5,#0 / ldreq / moveq / streqb + beq  (else-if, own cmp)          T = 3, E = 3 -> BOTH PREDICATED
cmp r1,#1 / bne  (block contains bl func_020ab5d4)                    call        -> BRANCH
```
6 for 6. Write the natural `if`s; the counts fall out.

---

# RULE 2 — ADDRESS CONSTANTS: how they split, and which half is CSE'd

This is the `0215b7f8` phenomenon (`add r4,r4,#0x1f80` once, `add rX,r4,#0x4000` at each use).

## 2a. How mwcc splits `base + K` when K is not a single ARM immediate

```
lo = the 8-bit-rotated chunk containing the LOWEST set bit of K      hi = K - lo
                                     ->   add t, base, #lo
                                          add d, t,    #hi
```
Python: `low = (K & -K).bit_length()-1 ; r = low - low%2 ; lo = K & (0xFF << r)`

Verified on 38/38 constants that fit two rotated immediates (0x104…0x3a489, hand-picked +
30 random). If K needs **three** immediates mwcc uses a **pool `ldr`** instead — 12/50 sampled.
Examples: `0x5f80 -> 0x1f80 + 0x4000` · `0x6180 -> 0x2180 + 0x4000` · `0x5f6c -> 0x36c + 0x5c00`
· `0xabc -> 0x2bc + 0x800` · `0xf770 -> 0x770 + 0xf000`.
(For a **load/store** the low part goes in the 12-bit offset instead: `0x5f7c` → `add #0x5000` +
`ldr [rX,#0xf7c]`. That is recipe #4.)

## 2b. Which half survives across a `bl` — THE LEVER

| how you wrote the address | what mwcc emits |
|---|---|
| **named local**, `T* p = base + K;` live across a `bl` | `add p,base,#lo` **ONCE** (callee-saved reg) + `add rX,p,#hi` **at every use** |
| **inline** `base + K` at each use | `add #lo` **and** `add #hi` **at every use** |
| K fits **one** immediate, named local | the single `add` is **rematerialised at every use** — a named local does NOT help |

So a two-instruction offset in a named local gives you exactly one hoisted add and one
recomputed add. That is the ROM's "mutated register reused across strlen+memcpy" — it is not a
special idiom, it is the default output of a named pointer local.

## RECIPE 2 (mechanical)

Read the target: if you see `add rC, base, #A` **once** (rC callee-saved) and `add rX, rC, #B`
**repeated**, then `A + B` is one address constant `K` and the source is a **named local**:

```c
/* target: add r4,r4,#0x1f80 ; add r0,r4,#0x4000 ; bl strlen ; add r1,r4,#0x4000 ; bl memcpy */

char* name = (char*)battle + 0x5f80;      /* <-- ONE named local, FULL offset 0x1f80+0x4000 */
int   len  = strlen(name);
memcpy(dst, name, len);
```
✔ emits `add r4,r0,#0x1f80` / `add r0,r4,#0x4000` / `bl` / `add r1,r4,#0x4000` — ROM sequence.

```c
/* WRONG (what the symptom looks like): both adds duplicated at each use */
int len = strlen(battle + 0x5f80);
memcpy(dst, battle + 0x5f80, len);        /* -> add #0x1f80 ; add #0x4000  TWICE */
```
Writing the low split point explicitly (`char* p = battle + 0x1f80;` then `p + 0x4000`) gives the
**same** code as the full-offset named local — either is fine, prefer the full offset (readable).

Both directions confirmed in `SP/inv/sched/a1.cpp` (`a1`…`a6`) and `a2.cpp` (`b1`/`b2`/`b3`, the
real `0215b7f8` fragment including the `strcpy(buf+len, …)` tail).

---

# RULE 3 — LOAD/STORE INTERLEAVING (the real `0216dad0` problem). PARTIAL LEVER.

For a straight-line run of `dst.field = src.field;` copies, mwcc chooses how far each load is
hoisted above the preceding store. The lever is **the declared type and SIZE of the destination
object** — pure declaration change, zero other codegen effect.

## What controls it

| destination object | schedule |
|---|---|
| `extern T X[];` (**incomplete** array — the default a worker writes for a pool symbol) | **fully interleaved**: `L1 P L2 S1 L3 S2 S3` |
| `extern StructT X;` — **struct object**, all copies **byte-width** | fully interleaved (same as array) |
| `extern StructT X;` — mixed widths | **depends on `sizeof(StructT)`** (table below) |

Measured for the `strh@8 / strb@7 / strb@6` shape by padding the struct declaration
(`unsigned char pad[n];` at the end), size sweep 10…58:

```
sizeof(dest) <= 14   ->  L1 P S1 L2 S2 L3 S3      (strictly serial)
sizeof(dest) == 16   ->  L1 P S1 L2 L3 S2 S3
sizeof(dest) >= 18   ->  L1 P L2 S1 L3 S2 S3      (fully interleaved)
```
Source-object size is irrelevant (72-cell src-size × dst-size cross product: 3 distinct schedules,
all determined by the destination alone).

## RECIPE 3

1. Write the ld/st order of the target block as `L`/`S`.
2. If yours is interleaved and the ROM's is serial (or vice versa): declare the destination pool
   symbol as a **struct** instead of `extern unsigned char X[];`, then **pad it** to move across the
   14 / 16 / 18 thresholds. Padding an `extern` declaration emits nothing — it is free.
3. If the ROM's `L`/`S` string is none of the three reachable ones, **SKIP** (see below).

---

# WHEN TO SKIP

* **`func_ov004_0216dad0` — SKIP.** Its predication is already correct under RULE 1; the residual
  5-instruction diff is the tail block `ldrh[sp,0] / ldr pool / ldrb[sp,2] / strh[8] / strb[7] /
  ldrb[sp,3] / strb[6]` = `L P L S S L S`. That string is **not reachable**:
  * 3 reachable schedules for `(strh@8, strb@7, strb@6)` into any destination — `LPSLSLS`,
    `LPSLLSS`, `LPLSLSS` — selected by destination size (14/16/18 thresholds). `LPLSSLS` is not
    among them for **descending** store offsets. It IS reachable when the byte stores sit at
    offsets **above** the halfword store (28 hits: `h@8, b@10, b@11`) — the ROM's are at 7 and 6,
    which is fixed by the data layout, so there is no source that produces it.
  * Ruled out: ~510 source variants (dest form ×  source form × temps × statement order ×
    grouping (comma/block/do-while/if(1)) × signedness × array-vs-member × union × anonymous union
    × C++ reference × pointer-of-unknown-provenance × `volatile` on either side × 20 struct layouts
    × 49 struct sizes × 72 size cross-products); all 79 optimizer pragmas × {on,off}; all 24
    compiler builds in `tools/mwccarm/` × {c++, c99, c} × {-opt speed/space/level=4/noschedule/
    schedule} × 7 `-proc` values. Global minimum stays at 5 differing instructions.
* **A ≤ 3-instruction ROM block that branches where you predicate** — no lever (the pragma floor
  is 3). SKIP.
* **Mixed-width copy runs whose `L`/`S` string is outside the 3 reachable schedules** — SKIP.

---

# EVIDENCE / REPRODUCING (`SP/inv/sched/`)

| file | what |
|---|---|
| `d.py <src.cpp> [target.txt]` | build + reloc-masked aligned diff vs a ROM asm dump |
| `m.py <src.cpp>` | build + dump **every** `.text` (multi-function labs) |
| `p1.py p2.py p3.py` | RULE 1 — threshold, if/else matrix, pragma, return-shaped ifs |
| `r20.py`, `a1.cpp`, `a2.cpp` | RULE 2 — 50-constant split enumeration; named-local vs inline |
| `r13.py r15.py`, `lab_sz.cpp`, `lab_off.cpp` | RULE 3 — width patterns, size thresholds, offset order |
| `r1…r12.py`, `r16.py`, `sweep.py`, `ver.py` | the `0216dad0` negative (variants, pragmas, compilers) |
| `t_dad0.txt`, `t_b7f8.txt` | extracted ROM asm for the two reproducers |
| `base_dad0.cpp` + `r2.py` | tail-block variants inside the **real** function |

Counts: RULE 1 threshold — exact cutoff located by exhaustive block sizes 2…12 in four block
shapes, plus a 6×6 if/else matrix and a 79×2 pragma sweep. RULE 2 — 50 constants × 3 source
shapes, 38/38 agreement on the split formula and on the named-local/inline split (the 4
"exceptions" are single-immediate constants and are stated as their own row). RULE 3 — 108-case
width×dest-kind enumeration, 49-point size sweep, 72-cell src×dst cross, 45-point layout×offset
sweep.
