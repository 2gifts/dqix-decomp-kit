# REGALLOC FINDINGS — plain -O2
> **Measured on mwccarm 2.0/sp1p5.** The ROM now builds with 2.0/sp2p2 and `-str pool,reuse`
> ([[dqix-sp2p2-rebase]]). The labs under `SP/lab/` and `SP/pad/` follow the build again, so a
> rule here is worth re-measuring before it is cited as settled.


## VERDICT

**A reliable lever EXISTS, but it only covers one of the three cases.** The register the compiler
picks is decided by exactly one source-visible property: **how each value is DEFINED — by a `bl`
return, or by any other instruction.** Nothing else a decompiler can write changes it.

| the two values in swapped registers are… | can you swap them from C? | how |
|---|---|---|
| both **COMPUTED** (def is `ldr`/`add`/`mul`/`lsl`/…) | **YES — codegen-neutral** | swap their declaration order. Earlier decl → **LOWER** register. |
| both **CALL-DEFINED** (def is `mov rN,r0` after a `bl`) | only by changing call order | order is forced: earlier call → **HIGHER** register |
| one of each | **NO** | the call-defined one *always* gets the lower register. Unfixable → SKIP. |

`0215d534` is case 3 → **it is not matchable from C and should be SKIPped.** Proof below.

---

## THE RULE (verified, 28/28 exhaustive + 220/220 randomized + 5/5 on the real d534 shape)

mwcc splits the callee-saved values into contiguous **bands** (which band a value lands in is decided
by the CFG and is *not* source-controllable). **Within a band** the order is decided solely by def kind:

```
band, from the LOWEST register upward:
    [ call-defined values, in REVERSE definition order ] then [ computed values, in FORWARD definition order ]
```

* **call-defined** = the value's def instruction is a copy out of the return register
  (`mov rN, r0` / `movs rN, r0` immediately after a `bl`). Incoming parameters behave like a
  third class that sits above both.
* **computed** = literally anything else (`ldr`, `ldrsh`, `add`, `smulbb`, `lsl`, `and`, …).

So the two classes run in **opposite directions**:
* two calls → the FIRST one gets the HIGHER register (`a=f(); b=g();` → a=r5, b=r4)
* two computed → the FIRST one gets the LOWER register (`a=p[1]*3; b=p[2]*5;` → a=r4, b=r5)

> ### Correction to `worker_ov_all.md` recipe #9
> Recipe #9 says "reorder decls: EARLIER→HIGHER reg". **That is only true for call-defined values.**
> For computed values it is exactly backwards: EARLIER→**LOWER**. This is why decl reordering
> "randomly" works for some workers and not others. Recipe #1's "within group, alloc is REVERSE
> def order" has the same half-truth.

### Evidence

`ra/lab2.py` — all 2^n def-kind patterns for n=2,3,4 (`C`=call, `L`=computed), 28 cases, all fit:

```
CC ['r5','r4']   CL ['r4','r5']   LC ['r5','r4']   LL ['r4','r5']
CCL ['r5','r4','r6']    CLC ['r5','r6','r4']    LLC ['r5','r6','r4']    LLL ['r4','r5','r6']
CCLL ['r5','r4','r6','r7']   CLCL ['r5','r6','r4','r7']   LCLC ['r6','r5','r7','r4']  ... (all 16 fit)
```

`ra/hunt.py` — 220 randomly generated branchy functions (if/else, loops, early return, mixed uses):
zero counterexamples to "call-defined below computed *within a band*".

---

## RECIPE (mechanical — a Sonnet worker can follow this)

**Step 1 — classify every callee-saved value in the TARGET disassembly.**
For each `r4..r11`, find its defining instruction.
* `mov rN, r0` / `movs rN, r0` on the line right after a `bl` → **CALL**
* anything else (`ldr rN,…`, `add rN,…`, `smulbb rN,…`) → **COMPUTED**
* `mov rN, r0` at the very top of the function → **PARAM**

**Step 2 — do the same on your `.o` (`bash SP/try.sh f.cpp`).** Find the mis-assigned pair.

**Step 3 — apply:**

* **Both COMPUTED → swap their declaration order in the source. This is the lever.**
  It is codegen-neutral: only the two destination registers change.
  ```c
  int a = p[3] * 6;    /* a -> r4 */          int b = q[7] * 6;    /* b -> r4 */
  int b = q[7] * 6;    /* b -> r5 */    ==>   int a = p[3] * 6;    /* a -> r5 */
  ```
  Verified byte-for-byte identical apart from the two registers (`ra/k5.cpp`, functions `k5`/`k6`).
  Caveat: if the two values load from the *same* base pointer at ascending offsets, reordering can
  merge/unmerge an `ldm` — check the whole disasm, not just the two registers.

* **Both CALL → you cannot fix it with decl order.** The order is `reverse(call order)`, and the
  call order is fixed by the target. If the target disagrees, the bug is in your *value structure*
  (an extra/missing variable, a wrong helper arity), not the allocator. Re-derive or SKIP.

* **Mixed (one CALL, one COMPUTED) and the target has the COMPUTED one LOWER → SKIP immediately.**
  No C or C++ source produces that with this compiler. Do not grind.

**Step 4 — the reverse direction, when it is legitimately available.** If the target's value comes
out of a `bl` but you wrote it as an inline computation (or vice versa), fixing that changes its
class and moves it across the whole band. This is the one case where the lever is both usable and
semantics-correct. Confirmed on the d534 shape: turning `mult` from `field5c*6` into a call return
moved the whole function from `code=r4,mult=r5` to `code=r5,mult=r4` — i.e. exactly the ROM's
assignment (`ra/s2.cpp`).

---

## SECOND LEVER — constants staged into callee-saved registers (the `0215c30c` family)

Symptom: ROM does `mov r4,#K; bl helper; mov r0,r4`; every C form you write emits
`bl helper; mov r0,#K` (one instruction short).

**Cause:** mwcc rematerialises a constant after the call whenever the constant has exactly one
reaching definition at its use. It does **not** rematerialise when the use is a control-flow
**merge** of two or more definitions — then it becomes a real vreg and gets a callee-saved register.

**Recipe:** give the value a second definition on another path so the use is a merge point.
```c
int r = 0;
g(0);
return r;                 /* -> bl g ; mov r0,#0        (one def, rematerialised) */

int r = 0;
if (cond()) r = 0;        /* second def -> merge */
g(0);
return r;                 /* -> mov r4,#0 ; bl g ; mov r0,r4   (staged, matches ROM) */
```
Verified in `ra/e1.cpp` (`e1` vs `e2`), and in the c30c shape in `ra/e7.cpp` (`e10`, `e11`).
`int r = 0; … if (!c) return r; … r = 0; return r;` does **not** work — each `return` still has a
single reaching def. The merge must dominate the use.

---

## THIRD FINDING — `0215c30c` is a C++ object with a constructor and destructor

Not a register-allocation problem at all. Read the callees:

* `func_ov004_0215c448` does `str <&data_ov023_021fe3e4>, [this,#0]` then initialises two
  sub-objects and returns `this` → **it is a constructor storing a vtable pointer.**
* `func_ov004_0215c474` tears down the same two sub-objects in reverse order and returns `this`
  → **it is the destructor.**

That explains everything about the function's shape:
* the two early `return 0`s (`movs r6,r0 / moveq r0,#0 / beq`) call **no** cleanup — they are
  *before* the object is constructed;
* the two later `return 0`s each run `bl func_ov004_0215c474` — they are *inside* the object's
  lifetime, and C++ evaluates the return expression **before** running destructors, which is
  precisely why the `0` is materialised into r4 *before* the call.

To match it the local must be a real `struct` with a ctor/dtor. **Blocker:** mwcc emits ctor/dtor
calls by mangled name (`_ZN…C1Ev`/`_ZN…D1Ev`), which will not resolve to `func_ov004_0215c448` /
`func_ov004_0215c474`, and wrapping them in inline members makes mwcc emit an extra out-of-line
`.text` section (→ `SIZE/OVERGEN`, confirmed in `ra/d2.cpp`). So c30c is blocked until those two
callees are decompiled as a real class — an integrator/orchestrator task, not a worker task.

---

## WHY `0215d534` IS UNMATCHABLE

Everything except register numbering already matches (18 differing bytes over 8 offsets, all pure
permutation). The five callee-saved values and their bands are stable across every source form:

| value | how defined | band | target | ours |
|---|---|---|---|---|
| `obj` | param | r6 | r6 | r6 |
| `code` | `bl 02156e2c` → **CALL** | {r4,r5} | **r5** | **r4** |
| `mult` | `smulbb` → **COMPUTED** | {r4,r5} | **r4** | **r5** |
| `entry` | `bl 021570a4` → CALL | {r7,r8} | r7 | r7 |
| `state` | `bl 0202ae18` → CALL | {r7,r8} | r8 | r8 |

The `{code, mult}` band is a mixed pair, so the rule forces **call below computed** → `code=r4`.
The ROM has `code=r5`. Toggling the def kind confirms the band structure is fixed and only the
kind matters (`ra/s1,s3,s4,s5,s2.cpp`):

```
code=CALL  mult=COMPUTED  -> obj r6, code r4, state r8, mult r5, entry r7   (ours)
code=COMP  mult=COMPUTED  -> obj r6, code r4, state r8, mult r5, entry r7   (forward order)
code=CALL  mult=CALL      -> obj r6, code r5, state r8, mult r4, entry r7   (= THE ROM)
```

The ROM's assignment is the "both call-defined" outcome, but the ROM's own asm shows `mult` is a
`smulbb`, not a `bl`. So either the value in r4 is not what it appears to be, or the file was built
from a source shape that produces a different band split. **Recommend SKIP.**

## WHAT WAS RULED OUT (all no-ops on d534's register assignment)

~40 source variants of the real function plus ~60 lab functions. Everything below left
`code=r4, mult=r5` bit-identical:

* `register` storage class (on either or both), `const`, `volatile` (breaks codegen instead)
* declaration hoisting (decl before/after, decl-then-assign, decl in outer vs inner block scope)
* every type permutation: `int`/`short`/`long`/`unsigned`/pointer↔int for all five values
* expression forms: `6*x` vs `x*6`, extra temps, removing temps, two-step assignment, casts,
  self-assignment, pointer-arithmetic form `(S6*)0 + n`, ternary/merged definition
* removing the named local so the multiply becomes a CSE temp (it sinks to the use sites instead)
* **inlining in every form** — `static inline`, `inline`, `__inline`, inline class member, macro.
  An inlined call is **computed**-class, not call-class. This kills the obvious workaround.
* restructuring: `goto end` vs per-path `return 0` (duplicates the epilogue), `if/!cond` vs
  `if/else`, `else if` chains, extra block nesting, moving the definition across the next `bl`
  so the live range crosses it
* **all 79 internal optimizer pragmas** extracted from the binary's string table, each tried
  `on` and `off` (`opt_lifetimes`, `opt_propagation`, `opt_scalarizeliveranges`,
  `opt_marknonregtemps`, `opt_common_subs`, `reg_class_allocs`, `peephole`, `scheduling`, …).
  Only `opt_repositioncode on` changed anything, and it reorders blocks, not registers.
* `#pragma optimize_for_size off`, and `-opt speed|space|level=2|nolifetimes|nocse|nopropagation|nodeadstore`
* **all 24 compiler builds** in `tools/mwccarm/` — every 2.0 build (`base`…`sp2p4`) produces
  byte-identical output, the project's `2.0/sp2p2` included; 1.2 and dsi builds are much
  further off. The compiler build is not the variable here.
* `-lang=c99` instead of `-lang=c++` — same allocation.

## REPRODUCING

Scaffolding is left in `$REPO/ra/` (untracked, not part of the kit):
`run.sh` compile+disasm · `dif.py <src> <target.txt>` aligned diff · `chk.py` one-line reg report ·
`odis2.py` dumps **all** `.text` sections (the stock `odis.py` shows only the first — that is how an
over-generated out-of-line ctor/dtor hides) · `lab2.py` the def-kind matrix · `hunt.py` the
randomized search · `sweep.py <src> opts.txt` the pragma sweep · `ver.py` the all-compilers sweep.
