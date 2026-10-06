# FRAME FINDINGS — plain -O2 (stack frame layout / local ordering / spills)
> **Measured on mwccarm 2.0/sp1p5.** The ROM now builds with 2.0/sp2p2 and `-str pool,reuse`
> ([[dqix-sp2p2-rebase]]). The labs under `SP/lab/` and `SP/pad/` follow the build again, so a
> rule here is worth re-measuring before it is cited as settled.


## 1. VERDICT — **YES. Fully controllable, and it is deterministic.**

The sp offset of every stack-resident local is a **pure function of (object size, declaration
order)**. Nothing else. Not use order, not type, not alignment, not initialisation, not scope
nesting, not address-taken-ness (beyond deciding whether the object is on the stack at all).

The whole thing collapses to one worker-facing sentence:

> ### **DECLARE YOUR LOCALS IN DESCENDING ORDER OF THEIR TARGET `[sp,#N]` OFFSET.**
> Highest offset declared first. This is a fixed point of the allocator (500/500 verified) and
> reproduces any layout the compiler can produce.

The only constraint on *which* layouts exist is the band order in §2 — and even that has a lever
(§7: pad an object into a different band).

---

## 2. THE RULE (lookup table)

Frame grows **upward from `sp+0`**. Everything is packed bottom-up in this fixed sequence:

| # | region | contents | ordering inside the region |
|---|---|---|---|
| 0 | **outgoing args** | `4 * max(0, maxargs_over_all_bl - 4)` bytes | ABI: arg5 at `[sp,#0]`, arg6 at `[sp,#4]`, … |
| 1 | **spill slots** | 4 bytes per spilled value | ascending, in order of first spill. Not source-controllable |
| 2 | **band 0** | locals with `sizeof == 1` | **reverse declaration order** (last declared = lowest) |
| 3 | **band 1** | locals with `sizeof == 2` | reverse declaration order |
| 4 | **band 2** | locals with `3 <= sizeof <= 8` | reverse declaration order |
| 5 | **band 3** | locals with `sizeof >= 9` | reverse declaration order |

Within a region the allocator is a plain bump allocator, bottom-up, **aligning each object up to
its own alignment**. There is **no hole filling** — alignment padding is dead space forever.

`sub sp, sp, #K` where **`K = align_up(end_of_band3, 4)`**.
* Special case: if that would be exactly `4`, mwcc emits **no `sub sp`** and puts the 4 bytes in
  the dummy `r3` slot of `push {r3, lr}` (frame reads as 0).
* mwcc then makes `push_bytes + K` a multiple of 8 by pushing a **dummy register** (`r3`) —
  it never pads the frame for alignment. So `push {lr}` ⟺ `K % 8 == 4`, `push {r3,lr}` ⟺ `K % 8 == 0`.

### Equivalent phrasing (same thing, top-down)
Address order from the TOP of the frame down: band3 objects in declaration order, then band2 in
declaration order, then band1, then band0, then spills, then outgoing args at `sp+0`.

### The corollaries a worker needs
* **Two locals in DIFFERENT bands: swapping their declarations is a NO-OP.** Byte-identical
  output. Stop permuting them.
* **Two locals in the SAME band: swapping their declarations swaps their slots** and changes
  nothing else. This is the lever.
* **`sizeof` decides the band, and band order is fixed** — a `short` can never sit above an `int`.
  To move an object across a band boundary you must change its *size* (§7).

---

## 3. EVIDENCE

| test | scale | result |
|---|---|---|
| Randomized layouts (2–10 locals, 40 type shapes: scalars, `char/short/int` arrays sz 1–24, 9 structs) | **1050 functions** | 1050/1050 exact (offsets + frame size) |
| Same, plus a call forcing a 8/20-byte outgoing-arg area | **600 functions** | 600/600 exact |
| Byte-equivalence classes over **all** permutations of random multisets (4 and 5 locals) | 55 multisets = **2760 compiled permutations** | class partition == model band-order partition, 55/55 |
| "declare in descending target offset" fixed-point test | **500 functions** | 500/500 |
| Real matched ROM functions, predicted from source | 4 hand-verified (below) | 4/4 exact |
| ROM-wide `sub sp,sp,#K` sanity | **2953 functions** (main + all overlays) | `K % 4 == 0`: **2953/2953**. `push_bytes+K` 8-aligned: 2934/2953; **all 19 exceptions are varargs prologues** (`stmdb sp!,{r0,r1,r2,r3}` before the real push) **or hand-asm** — zero genuine counterexamples |

### Real-function cross-checks (these are byte-exact matched sources, so the ROM *is* the answer)

**`src/Combat/Main/FinalizeRecordListDisplay02020cb8.cpp`** — frame `0x54`, 7 stack objects, 2 bands.
Decl order: `recListHead`(4) `val2Ignored`(4) `dummyPtr`(4) | `arrayC`(12) `arrayB`(12) `s`(32) `struct2`(16).
Model: band2 reversed → `dummyPtr@0, val2Ignored@4, recListHead@8`; band3 reversed →
`struct2@0xc, s@0x1c, arrayB@0x3c, arrayC@0x48`; end `0x54`.
ROM: `add r7,sp,#0`(dummyPtr) · `add r3,sp,#4`(val2Ignored) · `add r2,sp,#8`(recListHead) ·
`add r0,sp,#0xc`(struct2) · `add r0,sp,#0x1c`(s) · `strh r1,[sp,#0x2a]`(= s+0xe ✓) ·
`ldrb [sp,#0x38]`(= s+0x1c bitfield ✓) · `add r6,sp,#0x3c`(arrayB) · `add r5,sp,#0x48`(arrayC) ·
`ldr [sp,#0x50]`(= arrayC[2] ✓) · `sub sp,sp,#0x54`. **Exact.**

**`src/Combat/Overlay_23/FormatAndDispatchValue_021db3c0.cpp`** — frame `0x58`.
`char buf[0x40]`(band3), `short outA`, `short outB`(band1); one 9-arg call → 5 stacked = `0x14`.
Model: outgoing `0..0x13`; band1 reversed → `outB@0x14, outA@0x16`; band3 → `buf@0x18..0x57`; end `0x58`.
ROM offsets: `0,4,8,0xc,0x10` (outgoing), `0x14`, `0x16`, `0x18`, frame `0x58`. **Exact** — including
the band-1 reversal and `buf` starting at `0x18` (align 1, not rounded to 4).

**`src/Combat/Main/FormatValueAndDispatch02081ce8.cpp`** — frame `0x54`; `char buf[0x40]`, 9-arg call.
Model: outgoing `0..0x13`, `buf@0x14..0x53`, end `0x54`. ROM: `0,4,8,0xc,0x10,0x14`. **Exact.**

**`src/Combat/Main/LoadObjectResourceFiles02039f04.cpp`** — frame `0x5c`; 5-arg call (1 stacked),
`fileSize`,`fileData` address-taken (band2), `table` 80B (band3).
Model: outgoing `0..3`; band2 reversed → `fileData@4, fileSize@8`; `table@0xc..0x5b`; end `0x5c`.
ROM offsets `0,4,8` then `0xc,0x10,0x18,0x20,…,0x58` (10 × 8-byte entries from `0xc`). **Exact.**

### Counterexamples sought and NOT found
* **Use order / first-use order**: enumerated every (decl perm × use perm) for 7 multisets — the
  layout depends on decl order only, **never** on use order. (`inv/frame/e4.py`)
* **Alignment as the sort key**: refuted — `char[2]` (align 1) and `short` (align 2) share band 1
  and interleave by declaration order; `char[8]` (align 1) sits in band 2 above a `short`.
  The key is **`sizeof`**, not alignment.
* **Size-descending / size-ascending sorting inside a band**: refuted — `char[3] char[5] int`
  gives `c3@9, c5@4, i@0`, i.e. pure declaration order.
* **Hole filling**: refuted — `char[3] int char[5]` leaves `5..7` permanently dead.
* **Scope nesting / block scope**: no effect; textual declaration order is what counts, even for
  locals declared inside `if` blocks (`e11.cpp` `p4`).
* **Band boundaries**: probed one byte at a time. `1|2`, `2|3`, `8|9` are all real boundaries
  (order-independent across, order-dependent within). Sizes 3…8 are one band; ≥9 is one band.
* **`register`, `const`, initialiser vs not, declare-then-assign**: no effect.

---

## 4. RECIPE (mechanical)

**Step 1 — split the target frame.** Read `sub sp, sp, #K`.
* `OUT` = `4 * max(0, (max #args over every `bl` in the target) - 4)`. Outgoing-arg slots are
  `str`-ed *immediately before* a `bl` and **never read back**. They occupy `[0, OUT)`.
* `SPILL` slots sit next: 4-byte slots written by `str rX,[sp,#N]` *right after* a `bl` and read
  later, whose address is never taken. Count them → `S` slots, occupying `[OUT, OUT+4S)`.
* Everything from `OUT+4S` to `K` is your named locals.

**Step 2 — list the named locals** as `(offset, size)`. Size comes from how the target uses the
slot: the span up to the next object's offset, or the width of the `ldr/ldrh/ldrb`, or the block
size if a `memset`/`ldm`/`stm` covers it.

**Step 3 — sanity-check the band order.** Sort by offset ascending: sizes must be non-decreasing
*across band boundaries* — all size-1 objects first, then size-2, then size 3–8, then ≥9. If the
target violates that with the sizes you inferred, your sizes are wrong (or see §7).

**Step 4 — declare them in DESCENDING offset order.** That's it.

**Step 5 — check the frame.** Run the bump allocator from `OUT + 4S` (bands 0,1,2,3 in order,
each object aligned up to its own alignment) and `align_up` the end to 4 — that must equal `K`.
If your `K` is short, you are missing an object or a stacked call arg; if long, you declared too
many (mwcc **never** merges slots — see §6). Remember alignment padding is dead space, so
`K` is *not* simply the sum of the sizes.

### Worked example

Target:
```
    stmdb sp!, {lr}
    sub  sp, sp, #0x1c
    add  r0, sp, #4          ; &something (2 bytes: later strh [sp,#4])
    ...
    add  r0, sp, #8          ; 4-byte
    ...
    add  r0, sp, #0xc        ; 16-byte block (memset r1=0, r2=0x10)
    ...
    mov  r0,#5 ; str r0,[sp] ; bl f5      ; <- 5-arg call => OUT = 4
```
`OUT = 4`, no spills. Locals: `0x4`(sz2), `0x8`(sz4), `0xc`(sz16). Bands 1,2,3 — ascending ✓.
Frame check: `4 + 2 + (pad to 8) + 4 + 16 = 0x1c` ✓.

Sizes 2 / 4 / 16 put these in bands 1 / 2 / 3 — **three different bands**, so *every* declaration
order gives the same layout. Both of these compile identically:

```c
short h;  int w;  char blk[16];      /* h@4, w@8, blk@0xc */
char blk[16];  int w;  short h;      /* h@4, w@8, blk@0xc  — byte-identical */
```
That is the point of Step 3: **classify into bands before you spend a try on reordering.**
Reordering across bands is provably wasted effort.

A case where it *does* bite — two objects in the same band, target has `a@0x8`, `b@0x0`,
both 8-byte:
```c
/* BEFORE */ int b[2]; int a[2];   /* -> a@0, b@8   WRONG */
/* AFTER  */ int a[2]; int b[2];   /* -> a@8, b@0   MATCHES */
```

---

## 5. SPILLS

* Spill slots live **between the outgoing-arg area and the locals**, `[OUT, OUT+4S)`, ascending in
  order of first spill. Verified with 12 live call results forcing 4 spills, with and without
  address-taken locals and with and without a stacked-arg call (`e8.cpp` `s1/s2`, `e9.cpp` `q3/q4`).
* **Spill slots are never reused** across disjoint live ranges either: two `if` blocks each
  spilling 4 values produce 8 distinct slots, `0x0..0x1c` (`e11.cpp` `p1`).
* **The number of spills is not source-controllable** — it falls out of register allocation.
  If your local offsets are all uniformly shifted vs the target, you have the wrong spill count:
  that is a **value-structure** problem (recipe #9/#11 in `worker_ov_all.md`), not a frame problem.
* A non-address-taken array with only **constant** indices is fully register-promoted and gets
  **no stack slot at all** (`e11.cpp` `p2`: `int a[3]` → no `sub sp`). A variable index forces it
  to memory (`p3`).

---

## 6. SLOT REUSE: **IT NEVER HAPPENS**

Every declared object gets its own slot, no matter how disjoint the lifetimes:
```c
if (n) { int x[4]; sink(x); } else { int y[4]; sink(y); }   /* frame 0x20, x@0x10 y@0 */
{ int x[4]; sink(x); } { int y[4]; sink(y); }               /* frame 0x20, identical  */
if(n){int x[4];…} if(n){int y[4];…} if(n){int z[4];…}       /* frame 0x30, 3 slots     */
```
**Consequence — the frame size is a hard object count.** If the target's frame is smaller than the
sum of the objects you declared, you declared too many. The ROM's author reused one buffer for two
purposes; so must you. This is the single most useful diagnostic in this document for big frames.

---

## 7. THE BAND LEVER (when you need an object in a different band)

Band membership is decided by `sizeof` alone, so **pad the object** to move it:

| you have | you need it | do this |
|---|---|---|
| `short h` (band 1) above an `int` (band 2) | band 2 | `short h[2];` or `struct { short a; char pad[2]; } h;` — verified: `h@0x4, i@0x0` |
| `char c` (band 0) above a `short` | band 1 | `char c[2];` |
| an 8-byte struct below a 4-byte int | band 2 already | just reorder declarations |
| a 12-byte struct below an `int` | impossible | band 3 is always above band 2 — see §8 |

Verified (`inv/frame`, "neg" case): a bare `short` + `int` gives `h@0, i@4` in **both** declaration
orders; wrapping the short in any ≥3-byte type flips it to `h@4, i@0`.

---

## 8. WHEN TO SKIP (proved to have no C form)

1. **A layout that violates band order** — e.g. the target has a 2-byte object at a *higher*
   offset than a 3–8-byte object, and you cannot pad (because the ROM's own accesses prove the
   object really is 2 bytes and there is no room for padding between it and its neighbour).
   No declaration order produces this. *(This is rare; check §7 first — padding usually works.)*
2. **Your frame is right but every local is uniformly offset by 4·n from the target's.**
   That is a spill-count mismatch, not a layout mismatch. Do not permute declarations — go fix
   the value structure (recipe #9 / #11). Permuting is provably a no-op here.
3. **The target frame is not a multiple of 4.** Doesn't happen in generated code — 2953/2953
   ROM functions have `K % 4 == 0`. If you think you see one, you are reading a varargs prologue
   (`stmdb sp!, {r0,r1,r2,r3}` *before* the real `push`) — that block is not part of the frame.
4. **Trying to change a slot by reordering two locals in different bands.** Byte-identical output,
   guaranteed (2760/2760 permutations). Every try you spend on this is wasted.

---

## 9. REPRODUCING

Everything is in `SP/inv/frame/` (self-contained, nothing written into `src/`):

| file | what it does |
|---|---|
| `model.py` | **the rule, as 20 lines of Python** (`band()`, `layout()`, `frame_size()`) |
| `gen.py` | lab harness: emit N locals, probe each with `sink(&v)`, parse `add r0,sp,#N` |
| `cc.sh` / `adis.py` | compile with the exact project flags + disassemble **all** `.text` sections |
| `hunt.py` | randomized validator (1050 cases) |
| `perm.py` | byte-equivalence-class test over all permutations (2760 compiled permutations) |
| `e1..e12` | the enumerations: decl-vs-use order, band boundaries, spills, outgoing args, slot reuse |
| `romscan.py` | ROM-wide frame/push alignment scan over `build/usa/asm/**/*.s` |
| `scan.py` | compiles matched `src/**/*.cpp` and reports frame + sp offsets (finds validation targets) |
