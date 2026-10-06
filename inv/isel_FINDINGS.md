# ISEL FINDINGS — address folding / offset splitting / load-form selection
> **Measured on mwccarm 2.0/sp1p5.** The ROM now builds with 2.0/sp2p2 and `-str pool,reuse`
> ([[dqix-sp2p2-rebase]]). The labs under `SP/lab/` and `SP/pad/` follow the build again, so a
> rule here is worth re-measuring before it is cited as settled.

plain -O2, `-char signed`, `-lang=c++`.
Lab: `SP/inv/isel/` (`lab.sh`, `gen.py`, `dall.py`, `a1..a5.cpp`, `b1,b2.cpp`, `v788.cpp`).

---

## 1. VERDICT

| sub-problem | lever? | note |
|---|---|---|
| **which constants appear in the `add` chain** | **YES — fully mechanical** | you do *not* choose them. They are `canon(total)`. To get a *different* chain you insert **cast barriers**. |
| **whether the low bits land in the load's immediate or in a full `add`** | **YES** | the pointer must be a value that **crosses a basic-block boundary**. |
| **`ldrsh`+`lsl16`/`lsr16` staying separate instead of folding to `ldrh`** | **partial → in practice NO** | the only lever is a *second, sign-significant use* of the loaded value. The 2 ROM sites that show this pattern have no second use → **SKIP**. Pattern occurs 2× ROM-wide; worthless family. |

End-to-end proof: **`func_ov004_0216f788` → `MATCH`** (`SP/inv/isel/v788.cpp`), written from the rule below on the first try, no iteration. It exercises the canon rule *and* the cast barrier.

Scale proof: of **2432** multi-`add` address chains in the whole ROM disassembly, **2285 are exactly `canon(total)`**, **144 are clean group-splits** (cast barriers), **3 are `add r,r,#0`** in hand-written `itcm`. **0 unexplained.**
Of **10186** `add`+load pairs (add imm ≥ 0x100), **9658 (95%)** are the canonical split and **528** are the cross-block full-materialisation case.

---

## 2. THE RULE

### 2a. `canon(N)` — the only decomposition mwcc ever emits

```python
def canon(n):                     # n >= 0, n < 0x100000
    out = []
    while n:
        lb = (n & -n).bit_length() - 1   # lowest set bit
        lb -= lb & 1                     # round DOWN to an even bit position
        c = ((n >> lb) & 0xFF) << lb     # 8-bit window starting there
        out.append(c); n -= c
    return out                    # emitted in this order, lowest chunk FIRST
```
(ARM immediates are 8 bits rotated by an **even** amount — hence the `lb &= ~1`.)

Examples (all verified against the ROM):
`0x6380→[0x2380,0x4000]` · `0x5f6c→[0x36c,0x5c00]` · `0x5f80→[0x1f80,0x4000]` · `0x23ec→[0x3ec,0x2000]`
`0x71fc→[0x1fc,0x7000]` · `0x74de→[0xde,0x7400]` · `0x408→[0x8,0x400]` · `0x300→[0x300]`

> **Consequence: the constants you write in C are irrelevant.** `p+0x2380+0x4000`, `p+0x6380`,
> `p+(0x2380+0x4000)`, and `char* a=p+0x2380; a+0x4000` all emit **identical** code.
> Recipe #4's "write it as two adds to get two adds" is **false** — it only appeared to work when the
> source constants happened to already be `canon(total)` (which is common, because the ROM's own
> source was written that way).

### 2b. Memory-instruction immediate width (the `mask`)

| instruction | mask | C type that produces it |
|---|---|---|
| `ldr` `str` `ldrb` `strb` | **0xFFF** | `int`/`void*` ; `unsigned char` |
| `ldrh` `strh` `ldrsh` `ldrsb` | **0xFF** | `unsigned short` ; `short` ; **`char`** (`-char signed`) |
| no memory op (address is a call arg / stored pointer) | **0** | — |

*Bonus discriminator:* `ldrb rD,[rX,#>0xFF]` ⇒ the C type is **`unsigned char`**, never `char`
(`char` is signed here → `ldrsb`, 8-bit imm). Same for `ldrh` vs `ldrsh`.

### 2c. The default (straight-line) form

For a total byte offset `TOT` from base pointer `B`:
```
adds      = canon(TOT & ~mask)      # lowest chunk first
mem_imm   = TOT &  mask
```
**Test for "is this the default form": `sum(adds) & mask == 0`.**

### 2d. The two levers

| you need | lever | C form |
|---|---|---|
| the add chain split at a place `canon` would not split | **cast barrier** — every explicit pointer cast closes a group | `(char*)( (char*)B + G1 ) + G2` |
| a full-width `add` + `[r,#0]` (or `[r,#small_member]`) instead of the low bits sinking into the load | **pointer value must cross a basic-block boundary** | define `T* q = ...;` **before** an `if`/loop and dereference it **inside** |

Each group's own constants are still `canon(sum of that group)`. Groups compose left to right.

Verified barrier forms (all give `[0x2380, 0x4000, 4]` for base+0x6384):
```c
(char*)((char*)p + 0x6380) + 4                          // c03
(char*)((char*)p + 0x2380 + 0x4000) + 4                 // c16
char* b = (char*)((char*)p + 0x6380); ... b + 4         // c05
struct S* s = (struct S*)((char*)p + 0x6380); s->buf    // c09  <-- most readable
```
NOT a barrier (folds to `[0x384,0x6000]`): `char* b = (char*)p + 0x6380; b + 4` (c06),
`((struct SC*)p)->sub.buf` where the struct declares the whole nesting (c13).

Verified cross-block forms (all give `add rX,rB,#0x5d00; ldr rD,[rX]`):
```c
T* q = (T*)((char*)p + 0x5d00);  if (cnd()) snk(q->a);      // f01/e04-family
T* q = (T*)((char*)p + 0x5d00);  if (q != other()) ...      // f02
T* q = (T*)((char*)p + 0x5d00);  if (cnd()) q = other(); q->a;  // f03 (phi)
snk2(q->a); if (cnd()) snk2(q->a);   // f06 — 2nd (guarded) access uses the full pointer
```
Straight-line always folds instead (`f05`, `f07`, `f08`, `f13`, `g01`, `g07`, `g08`).
Caveat: `if (q)` (null test) fuses the add with the compare → **`adds`**, not `add` — different bytes.
Use a test on something else (`if (cnd())`) when the ROM shows plain `add`.

---

## 3. RECIPE — read the target, write the C

Target shape: `add r,B,#a1 ; add r,r,#a2 ; … ; add r,r,#ak ; <MEM> rD,[r,#L]`
(`L` absent ⇒ 0; the chain may end at a `bl` argument instead of a MEM — then `mask = 0`).

1. **`mask`** ← §2b from the MEM mnemonic (0 if there is no MEM).
2. **Group the adds.** Left to right, take the **longest** prefix `[a1..aj]` with `[a1..aj] == canon(a1+…+aj)`. That is `G1`. Repeat on the rest → `G1..Gm`.
3. **`m-1` cast barriers.** Write
   `(char*)( … (char*)( (char*)B + sum(G1) ) + sum(G2) … ) + sum(Gm)`
   — or, more readably, make the innermost groups named/cast pointers (`(struct X*)((char*)B + sum(G1))`).
4. **Last group / load form.**
   * `sum(Gm) & mask == 0` → **straight-line is fine.** Collapse: `*(T*)((char*)B + sum(G1..Gm) + L)`; you may drop barriers whose groups are already `canon` of the running total.
   * `sum(Gm) & mask != 0` → the pointer **must cross a BB boundary**: declare it before an `if`/loop, use it inside. `L` becomes the struct-member offset from that pointer.
5. Gate.

### Worked example — `func_ov004_0216f788` (MATCHED)

```
    bl _Z15GetBattleStructv
    add r0, r0, #0x2380
    add r1, r0, #0x4000
    add r0, sp, #0x0
    add r1, r1, #0x4
    mov r2, #0x40
    bl memcpy
```
* MEM = none (call arg) ⇒ `mask = 0`, `L = 0`.
* Group: `canon(0x2380)=[0x2380]` ✓, `canon(0x6380)=[0x2380,0x4000]` ✓, `canon(0x6384)=[0x384,0x6000]` ✗
  ⇒ `G1=[0x2380,0x4000]` (sum 0x6380), `G2=[4]`. `m=2` ⇒ **one cast barrier.**
* `sum(G2)&0 == 0` ⇒ straight-line OK.

```c
// BEFORE (worker's form — 1 instruction SHORT, SIZE 0x98 vs 0x9c)
memcpy(header, (char*)bs + 0x6384, 0x40);      // -> add #0x384 ; add #0x6000

// AFTER (matches)
memcpy(header, (char*)((char*)bs + 0x2380 + 0x4000) + 4, 0x40);
                                                //-> add #0x2380 ; add #0x4000 ; add #4
```
Full file: `SP/inv/isel/v788.cpp` → `python SP/wgate.py 004 0216f788 …` prints **MATCH**.

### Worked example — `0215b7f8` (case C), both levers

```
    add r6, r4, #0x26c
    …two bl, one conditional branch…
    add r2, r6, #0x5d00
    ldr r0, [r2, #0x0]
```
* `mask = 0xFFF`, `L = 0`. Chain `[0x26c, 0x5d00]`; `canon(0x5f6c)=[0x36c,0x5c00]` ✗ ⇒ `G1=[0x26c]`, `G2=[0x5d00]` ⇒ **1 cast barrier**.
* `sum(G2) & 0xFFF = 0xd00 != 0` ⇒ **cross-block pointer** required.

```c
struct S4* q = (struct S4*)((char*)((char*)bs + 0x26c) + 0x5d00);   /* barrier */
SomeCall();
if (Cond()) { Use(q->ptr); }                                        /* cross-block */
```
→ `add r0,r0,#0x26c ; add r4,r0,#0x5d00 ; ldr r0,[r4]`  (lab case `g04`; `g05` is the
named-intermediate variant when the ROM also uses `B+0x26c` elsewhere).

---

## 4. EVIDENCE

* **2-addend sweep, 29 (A,B) pairs** taken from real ROM chains: every single result is exactly
  `canon(A+B)`. The 2 that looked "re-split" (`0x2380+4`, `0x26c+0x5d00`) and the 3 that "merged
  to one add" are all `canon`. Source constants have **zero** influence.
* **Barrier matrix, 18 forms** (`c01..c18`): cast ⇒ group boundary; named local, parenthesisation,
  operand order, `void*`/`unsigned char*` round-trip ⇒ no effect. `(char*)(int)ptr` also a barrier.
* **Load-immediate sweep**, 12 offsets × {`int*`,`short*`,`char*`, cast-wrapped}: `mem_imm = TOT & mask`
  with mask 0xFFF / 0xFF exactly as tabulated; the cast barrier does **not** change load absorption.
  Negative offsets → `sub` + negative `L`, same masks. Offsets ≥ ~0x100000 → pool load + register add.
* **Cross-block hunt, 42 forms** (`d01..d16`, `e01..e13`, `f01..f13`, `g01..g08`): straight-line always
  folds; a BB boundary always materialises. `volatile` pointer produces stack traffic (never matches).
* **ROM-wide check**: 2285/2432 chains `== canon(total)`, 144 group-splits, 3 hand-asm `add #0`.
  9658/10186 add+load pairs canonical.
* **Counterexamples sought and not found**: no source form makes mwcc emit a non-`canon` chunking
  *within* a group; no form makes a straight-line load skip the immediate absorption; no optimizer
  pragma (78 `opt_*`/`peephole`/`scheduling`/`reg_class_allocs`, on **and** off) changes any of it.

### Case A (`ldrsh` + `lsl#16`/`lsr#16` vs `ldrh`) — the negative

`(unsigned short)` of a `short` **load** folds to `ldrh` under every one of 30 syntactic variants
tried: direct, via `int`/`unsigned` temps, `&0xffff`, double cast, `+0`, unary `+`, C++ reference,
`const` reference, union, `unsigned short*` cast, `volatile short*`, `static inline` accessor
(short- and int-returning), global-pointer double indirection, prototype narrowing, ternary,
2-way and 3-way branch merges, value live across a call, 16-bit signed bitfield (gives 4 instrs, not 3).
No `opt_*` pragma affects it.

**The one thing that works:** the loaded value must have a **second use that needs the sign**
(a signed compare, a signed store, or passing the signed value on). Verified in `a1v8`, `a2v24`,
`a2v25`, `a2v26`, `a2v27`, `a4w14`, `a5x3`, `a5x5`:
```c
short t = gp->f;                       /* ldrsh   */
unsigned short v = t;                  /* lsl #16 ; lsr #16 */
useIt(t, v);                           /* <-- the signed use is what keeps ldrsh alive */
```
Both ROM sites (`ov004:02155d54`, `ov004:02154b7c`) have **no** second use — the loaded value dies
into the shift. Not reproducible. The pattern occurs **2×** in the entire ROM.

---

## 5. WHEN TO SKIP

* **`ldrsh rX,[..] ; lsl rX,#16 ; lsr rY,#16` where `rX` has no other use.** No C form. 2 sites ROM-wide
  (`02155d54`, `02154b7c`) — skip that branch or the function. Do **not** grind it.
* **`adds` (flag-setting) in an address chain** — that is the null-test fusion (`if (q)`); if the ROM
  shows plain `add` you cannot use a null test, and vice-versa.
* **A `volatile` pointer** never matches an `add`+load chain (it forces a stack slot).
* **`ldr rN, <pool>` where the pool word is a small ARM-encodable literal** (e.g. `.word 0x1f`,
  `.word 0x20` at `0216f780/0216f784` in `func_ov004_0216f634`). mwcc emits `mov rN,#0x1f` for every
  form tried (plain int, `(void*)`, `(char*)`, `unsigned`, `static const`, file-scope `const`,
  struct-by-value, bitfield-struct-by-value; `float` pools the *float* bits, not the integer).
  **This is a separate, uncracked lever.** 78 such sites ROM-wide (1.4% of pool words) — rare.
  `func_ov004_0216f634` is blocked on *this*, not on the offset rule: apply §3 to its
  `add #0x2380/#0x4000/#0x4` chain (identical to the matched `0216f788`), and if the two
  `ldr r0,<literal>` remain the only diff, **SKIP**.

---

## 6. PATCH TO `worker_ov_all.md` RECIPE #4

Replace recipe #4 with:

> **4. IMMEDIATE/OFFSET SPLIT — the constants are NOT yours to choose.**
> `mem_imm = TOT & mask` (`mask` = 0xFFF for `ldr/str/ldrb/strb`, 0xFF for `ldrh/strh/ldrsh/ldrsb`,
> 0 for a call argument); the `add`s are `canon(TOT & ~mask)` = greedy 8-bit windows from the lowest
> set bit rounded down to an even bit, lowest chunk emitted first. Any C expression with the same
> total gives the same code.
> * chain ≠ `canon(total)` → **cast barrier** per group: `(char*)((char*)B + G1) + G2`.
> * `sum(last group) & mask != 0` (e.g. `add #0x5d00 ; ldr [r,#0]`) → the pointer must **cross a
>   basic-block boundary**: declare `T* q = …;` before an `if`/loop, dereference inside.
> Full rule + worked examples: `SP/inv/isel_FINDINGS.md`.
