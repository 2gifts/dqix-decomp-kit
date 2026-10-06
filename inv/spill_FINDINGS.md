# SPILL FINDINGS — the saturated allocator (sav=8) is NOT a wall

## 1. VERDICT — **YES, a controllable lever exists. `sav=8` must be REMOVED from the skip list.**

The premise of this investigation ("at `sav`=8 the spill choice is internal to the allocator and
no source rewrite changes it — 43 variants, 0 effective") is **false**, and I can say precisely why
the 43 variants did nothing.

Three independent proofs:

* **Lab.** Three functions differing *only* in the declaration order of 10 identical values compile
  to **byte-identical instruction streams** in which the spill set follows declaration order exactly
  (`a3.cpp`: declare `v0,v1,v2` first → those three spill; declare `v3,v7,v1` first → *those* three
  spill; every other instruction identical). Declaration order is a clean, codegen-neutral lever.
* **Real ROM function.** `ov031/0220be38` — **`sav`=8, 2 live stack spill slots, 43 instructions —
  already matches byte-exact** (`wgate 031 0220be38` → `MATCH`). Swapping the declaration order of
  two of its computed locals turns `MATCH` into `BYTEDIFF: 9 bytes` and the diff is exactly the
  predicted `r6`↔`r7` exchange. The allocator responds to the source at `sav`=8.
* **Census.** **53 functions with `sav`=8 have already been matched byte-exact by workers**, 5 of
  them with genuine spill traffic. The class is not unmatchable; it is being skipped.

**Why the 43 variants failed.** In `ov004/0215f0c0` the three contested values — `bs`
(`bl GetBattleStruct`), `self` (`bl GetCombatantWithFlag0x100`), `info` (`bl GetFieldAt0x150`) — are
**all three CALL-DEFINED**. Recipe #9 already states that for call-defined values the order is
forced by *call order* and **declaration order does nothing**. The variant list (decl order, decl
scope, types, `void*`+cast, live-range splitting, CSE-vs-recompute) is a list of levers that are
*documented no-ops for that value class*. Byte-identical output was the correct, predictable result —
it is not evidence about the allocator. I reproduced this exactly on the real function: three
declaration-order variants of `0220be38`'s two call-defined values (`v_a`, `v_b`, `v_c`) all still
`MATCH`, while one variant of its *computed* locals immediately breaks the match.

---

## 2. THE RULE

### 2a. The resource ladder (this is the whole finding in one table)

mwccarm fills **exactly 8 callee-saved slots and then memory**, in this fixed order:

| rank | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 … |
|---|---|---|---|---|---|---|---|---|---|---|---|
| resource | `r4` | `r5` | `r6` | `r7` | `r8` | `r9` | `r10` | `r11` | `S@0` | `S@4` | `S@8` … |

* `r4`–`r10` are the seven **good** registers, taken first, in that order.
* **`r11` is the last-resort register** — it is used only when all of `r4`–`r10` are already used.
* Below `r11` the ladder continues into **stack spill slots**, ascending offset = *better*.

**Verified ROM-wide:** of 6000 real ARM functions with a callee-saved push, **5993 have a push set
that is a contiguous prefix `r4..rN`, and `r11` never appears unless all of `r4`–`r10` do.** The 7
exceptions (`_ll_udiv`, `_ull_mod`, `func_0200cd1c/cd2c/ee38/ee94/ef44`) all push `ip` and branch
into a neighbouring function's body — hand-written runtime asm, not mwcc output. So the ladder holds
on **100 % of compiler-generated functions**.

### 2b. How many values spill

> **spills = (peak number of simultaneously-live values across a call) − 8**

Not "how big the function is". `sav`=8 only means peak ≥ 8.

### 2c. Which values land where — recipe #9, continued past the end of the register file

Order the competing values **worst → best**:

> **[ CALL-defined values, REVERSE definition order ] then [ COMPUTED values, FORWARD declaration order ]**

and zip that against the ladder read from the worst end. **This is recipe #9 verbatim** — the only
new fact is that when the values run past `r11` the ladder simply keeps going into stack slots.
Spilling is not a separate mechanism.

*Verified: 420/420 randomized functions (N = 9..16, random call/computed mixes, 7 seeds), zero
counterexamples.*

### 2d. Tie-break when use counts differ

Where values compete for the losing end, the ones with the **lowest static use count** lose first;
equal counts are broken by the band order in 2c. Slot offsets are then assigned by band order over
the **spilled set alone**, first in band order → **highest** offset.

*Verified: 400/400 randomized functions with mixed classes AND mixed use counts (8 seeds), exact on
spill set, register map, and spill-slot offsets.*

**Caveat, stated honestly:** 2d is exact for a clique of values live across the same calls (my labs).
It did **not** predict the fine ordering of `ov031/0220be38`, a real function whose values sit in
several CFG bands with parameters interleaved between call-defined values. Recipe #9 already warns
that *which band* a value lands in is CFG-decided and not source-controllable. **2a, 2b and 2c are
what you should rely on; treat 2d as the tie-break heuristic, not a proof.**

### 2e. What does NOT affect the spill choice — stop trying these

| lever | effect | evidence |
|---|---|---|
| **use ORDER** (which value is read first after the calls) | **none** | `a2.cpp`: forward / reverse / scrambled use order → byte-identical output |
| **loop nesting of a use** | **none** | `h_loop0`,`h_loop2`: a value whose only use is inside a single *or* doubly-nested loop still spills ahead of values used once outside |
| **declaration order of CALL-defined values** | **none** | `v_a`,`v_b`,`v_c` on the real `0220be38` — all still `MATCH` |
| live-range *start* staggering | **none** | `s_stag` |
| declaring a spilled value `volatile`/`register`/address-taken | not a lever (changes codegen instead) | prior REGALLOC work |

Pre-call uses **do** count toward the use total (`h_pre`).

### 2f. Answer to the brief's frame question

**Yes — spill slots obey recipe #19.** They sit at the bottom of the frame, immediately above the
outgoing-argument area and **below every named local**, and the first value in band order gets the
**highest** spill offset — i.e. exactly "declare in descending target-offset order". Confirmed on
real ROM code in `ov004/0215f0c0`: a 7-argument call puts outgoing args at `[sp,#0..#8]`, the four
spill slots occupy `0xc,0x10,0x14,0x18`, and every named local starts at `0x1c`.

---

## 3. EVIDENCE

| what | scale | result |
|---|---|---|
| ladder enumeration, N = 1..14, with and without a live param | 28 ladders | tail is invariably `… S@8 S@4 S@0 r11 r4 r5 r6 r7 r8 r9 r10` |
| declaration-order permutation (`a3.cpp`) | 3 orders × 10 values | spill set follows decl order **exactly**; all other bytes identical |
| use-order permutation (`a2.cpp`) | 3 orders | **zero** effect |
| loop-depth / use-count / pre-call-use probes | 20 functions | loop depth irrelevant; static use count is the tie-break |
| randomized, mixed call/computed classes (`hunt.py`) | **420** functions, 7 seeds | **420/420**, 0 counterexamples |
| randomized, + mixed use counts, incl. slot offsets (`hunt2.py`) | **400** functions, 8 seeds | **400/400** exact |
| ROM-wide ladder test (`romcheck.py`) | **6000** real functions | 5993/6000; all 7 exceptions are hand-asm runtime |
| already-matched `sav`=8 census (`proof8.py`) | 3012 matched functions located | **53 with `sav`=8**, 5 with spill traffic |
| real function `ov031/0220be38` (`sav`=8, 2 spills) | 7 source variants | base `MATCH`; 3 call-decl variants `MATCH`; 3 computed-decl variants `BYTEDIFF` |

**Counterexamples sought and not found:** a case where use order changes the spill set (0/3);
where loop nesting protects a value (0/4); where a call-defined value's declaration order matters
(0/3 on real code, 0/many in lab); where `r11` is used while `r4`–`r10` are not all used
(0/5993 compiler-generated).

**Counterexample found and reported:** the 2d tie-break does not reproduce the exact permutation of
`ov031/0220be38` (multi-band, params interleaved). Reported rather than papered over.

---

## 4. RECIPE (mechanical)

**Step 1 — read the ladder off the target, do not guess.**
```
grep -rhA200 "^func_ovNNN_ADDR:" REPO/build/usa/asm/ovNNN/
```
* `sav` = registers in the opening `stmdb sp!`.
* **spill slots** = distinct `[sp,#N]` offsets that are `str`'d *and* `ldr`'d, lying above the
  outgoing-arg area (`4*max(0,maxargs-4)` bytes) and below the first `add rX, sp, #K` local.
* **peak live values = `sav` + number of spill slots.** Write that number down.

**Step 2 — classify every one of those values by its DEFINING instruction** (recipe #9):
`mov rN,r0` right after a `bl` → **CALL** · `mov rN,r0` at function top → **PARAM** ·
anything else (`ldr`/`add`/`mul`/`lsl`) → **COMPUTED**. A call result written straight to the stack
(`bl f` / `str r0,[sp,#K]`) is a **spilled CALL** value.

**Step 3 — write the whole function** (method_FINDINGS §3). Then compare *counts* first:

| symptom | meaning | fix |
|---|---|---|
| your spill count > target's | you kept a value alive that the ROM recomputes | recipe #2: inline the expression at each use |
| your spill count < target's | you recompute what the ROM keeps alive | recipe #2: hoist it into a named local |
| counts match, registers permuted | pure ladder ordering | step 4 |

**Step 4 — apply the lever that matches the value's class.**

| the mis-placed value is… | lever |
|---|---|
| **COMPUTED** | **swap declaration order. EARLIER declaration = LOWER rank = WORSE resource** (spills first). To stop a value spilling, declare it LATER. |
| **CALL-defined** | declaration order is a **no-op**. The order is `reverse(call order)`. Either change the call order, or **flip the class** (recipe #9 step 4): if the ROM's value comes out of a `bl` and you wrote it as an inline computation, model the call. |
| **PARAM** | not source-controllable; if it disagrees, your signature/arity is wrong. |

**Step 5 — `python SP/wgate.py OV ADDR <abs path to file.cpp>` → `MATCH`.**

### Worked before/after (real, on `ov031/0220be38`, `sav`=8, 2 spills)

Target inner loop: `mov r6,#0` / `sub r7,r8,r5` — so `j`→`r6`, `limit`→`r7`.
Both are COMPUTED, so declaration order is the lever, earlier = lower register:

```c
/* WRONG -- BYTEDIFF: 9 bytes            */    /* RIGHT -- MATCH                        */
do {                                            do {
    int limit = n - i;   /* -> r6 */                int j;               /* -> r6 */
    int j;               /* -> r7 */                int limit = n - i;   /* -> r7 */
    for (j = 0; j < lenB && j < limit; j++) {       for (j = 0; j < lenB && j < limit; j++) {
```
Nothing else in the 43-instruction function changes — only the two register numbers.

---

## 5. WHEN TO SKIP

**Delete these from the skip list — they are wrong and are costing the fleet the whole large-function band:**

* ~~"`sav`=8 (target pushes all of r4–r11). 594 of the 968 ≥600B functions. SKIP ON SIGHT."~~
  **53 `sav`=8 functions are already matched.** `sav`=8 means "peak liveness ≥ 8", nothing more.
  Triage on `big%` alone; `sav` is not a difficulty axis.
* ~~"at `sav`=8 no declaration-level source rewrite changes the spill choice."~~ It does, for
  COMPUTED values, deterministically.

**Genuinely skip:**

1. **A mis-placed CALL-defined value whose call order you cannot legitimately change, and whose
   class cannot be legitimately flipped.** Declaration order will never move it — this is recipe #9's
   existing "mixed pair, COMPUTED lower → no C form" case, now confirmed to extend into the spill
   region. Recognise it in one read instead of 43 variants.
2. **Your spill *count* differs from the target's and you cannot find the missing/extra value.**
   That is a value-structure error, not an allocator problem; permuting declarations cannot fix a
   count mismatch. Re-derive or skip.
3. Everything already in the PROVEN-NO-C-FORM list still applies.

**Honest boundary:** the largest already-matched `sav`=8 function is 155 instructions. Nothing here
promises that a 400-instruction `sav`=8 function is easy — only that its *register allocation* is
governed by the same mechanical rule as a 43-instruction one, and that the reason `0215f0c0` stalled
was a mis-diagnosis (call-class values pulled by a computed-class lever), not an uncontrollable
allocator.

---

## 6. ARTIFACTS — `SP/inv/spill/`

| file | what |
|---|---|
| `sl.py` | compile + per-function disasm with push set / frame / spill traffic |
| `gen.py` | the N = 1..14 ladder enumerator (`python gen.py`) |
| `g2.py` | clique-lab generator: per-value kind, use counts, loop depth |
| `g3.py` | staggered-live-range lab generator |
| `hunt.py` | randomized verification of the band ladder (420/420) |
| `hunt2.py` | randomized verification incl. use counts + slot offsets (400/400) |
| `romcheck.py` | ROM-wide ladder test over 6000 real functions |
| `proof8.py` | census of already-matched `sav`=8 functions |
| `find8.py` | finds small `sav`=8 overlay functions (demonstration targets) |
| `a1..a3, b1, m1, n2, q2, s1, t1.cpp` | the enumerated labs behind §3 |
| `v_base/v_a/v_b/v_c/v_d, w_swap/w_hoist/w_inline.cpp` | the real-function variants on `ov031/0220be38` |

## 7. RECOMMENDED WORKER-PROMPT PATCH

> **Recipe #9 does not stop at `r11`.** When a function's peak liveness exceeds 8, the same band
> ladder continues into stack slots: `r4 r5 r6 r7 r8 r9 r10 r11 S@0 S@4 S@8 …`, `r11` always last
> before memory. **spills = peak simultaneously-live values − 8.** `sav`=8 is NOT a skip signal —
> 53 such functions are already matched. Diagnose in this order: (1) does your spill COUNT match the
> target's? if not, fix live ranges with recipe #2, not declaration order; (2) if the count matches
> and registers are permuted, classify the mis-placed value — **COMPUTED → swap declaration order
> (earlier = worse = spills first); CALL-defined → declaration order is a NO-OP, change call order
> or flip the class; PARAM → your signature is wrong.**
