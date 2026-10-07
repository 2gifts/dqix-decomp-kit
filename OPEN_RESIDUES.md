# Named residues nobody has cracked

Split out of OPEN_WORK.md on 2026-09-10 so the handoff stays small. Read on demand, and grep
it for an address rather than reading it whole.

Pool paths below (`clswork_*`, `hold_*`, `handwork/`, `*_stage`) name files in the original
pipeline's state, which the kit does not ship. The closest attempt per unmatched address is in
`priors/`, listed with its residue in `priors/INDEX.tsv`.

## OPEN — named residues nobody has cracked

### The `clswork_*` directories are PRIORS, not scratch — do not delete them

Twelve `$SP/clswork_<pid>/c.cpp` files are orphaned `colorsweep` work copies from dead sessions, and
four of them are the best surviving attempt at an unlanded address. Re-gated 2026-08-26, none is a
free match:

| addr | prior | verdict |
|---|---|---|
| `031:0223a374` | `clswork_60048/c.cpp` | **BYTEDIFF 5 bytes** at `0xd 0x12 0x35 0x3a 0x48` — the closest unclaimed residue in the pools |
| `031:02226e08` | `clswork_62276/c.cpp` | BYTEDIFF 100 |
| `017:021cf730` | `clswork_49264/c.cpp` | BYTEDIFF 161 |
| `main:020dd7ac` | `clswork_29208/c.cpp` | SIZE/OVERGEN `0x10c` against a `0x108` slot |

They are still outside every sweep (`clswork_*` is not a pool name any tool reads). Either add the
glob to `repairsweep.py` or move the four into `staging/<mod>/` before the next sweep.

### `031:02232678` — 4 bytes, scratch r1-vs-r2 on one store (worked 2026-08-25, not closed)

Prior: `$SP/repair_work_3of4/031_02232678.cpp` (the copy colorsweep already improved; the other
shards hold a 146-byte variant of the same address — use the 4-byte one).

```
 *0x0048  ldr r1, [r4, #4]        | ldr r2, [r4, #4]
 *0x0050  add r1, r1, sl, lsl #2  | add r2, r2, sl, lsl #2
 *0x0054  str r0, [r1, #0x10]     | str r0, [r2, #0x10]
```

Only the address temp's register differs, on the `*(int*)(field4 + i*4 + 0x10) = f(...)` store
inside the `do` loop. Disproven, each verified by `wdiff` (do not repeat):

* inlining the `e0`/`e1` pair pointers — 137 bytes AND 4 short: those locals are load-bearing;
* binding the call result to a local first (`int r = f(...); *p = r;`) — byte-identical, 4;
* array-index form `((int*)field4)[i + 4] = ...` — 16 bytes, the index computation changes too;
* moving the `e0` declaration above the store — byte-identical, 4.

Consistent with recipe #15 (scratch registers ignore declaration and statement order), so the lever
is a FORM change that alters when the address temp is first materialised — not any reordering.
`colorsweep` cannot reach it: the surrounding declarations are not a run of three, so r4/r14/r17
never fire on them.

Two more with identical diff offsets (`0x5, 0xa, 0xf, 0x15, 0x19, 0x1d, 0x1e, 0x21`), so one crack
should close both: `main:020a1bb4` (60 bytes) and `main:020a1ccc` (62 bytes). The full residue list
is `$SP/wlog/repair_verdicts_*of4.txt` — repairsweep now records per-address verdicts instead of
only counts.

### `main:02061c04` — `BYTEDIFF 42`, every stack slot right, four register clusters left

State at the end of the 2026-08-25 session (the newest entry; everything below this sub-section is
older and is kept for its per-case notes).

    python pad/casescore.py <ABS path>    68 shape=9 reg=32 sp=0 unaligned=0
    python pad/spslots.py  <ABS path>     NEW: every distinct sp+N, ROM vs ours -- 62/62 match
    python pad/fulldiff.py <ABS path> --list reg     the four clusters below, in full

#### `0xbf` CRACKED — 42 -> 38 bytes, and the method that did it

The standalone probe said the fix was `dst` declared before `src` plus a split increment; applying
that to the real function made `0xbf` WORSE (4 bytes to 7), because the real case keeps `count` live
through a five-iteration tail with a `memmove` and the loop's registers are chosen under that
pressure. **A probe that omits the case's own tail is not faithful.** `pad/bfsweep2.py` carries the
tail and pads the frame so `deck` lands at a non-zero `sp` offset, and its winner landed first time:

```c
unsigned short count = 14;
int n = 14;                      /* the counter FIRST, and from the literal, not from `count` */
unsigned short *dst = deck;      /* destination before source */
unsigned short *src = (unsigned short *)data_020e7de8;
do {
    *dst = *src;                 /* the compound increment SPLIT into statements */
    ++src;
    ++dst;
} while (--n);
```

**The idiom, stated generally:** when a case's registers are named wrongly and the instruction
sequence already matches, the lever is the ORDER values are defined *together with* how far a
compound statement is broken up — neither alone. Three changes at once were needed here; each on its
own left the residue where it was, and the declaration swap alone had already been tried and
dismissed twice.

**`pad/casegrid.py <src> <case> <from.txt> <to-dir>` is the tool to use from now on**: it swaps one
case's body against every `to_*.txt` in a directory, compiles the REAL function each time (~10 s)
and reports that case's reloc-masked byte residue. Generators for the grids live beside it
(`gen_d3grid.py`, `gen_d3grid2.py`, `gen_e4grid.py`, `gen_e4grid2.py`, `gen_e7grid.py`,
`gen_e7grid2.py`).

#### The three register clusters left — 36 bytes, and NOTHING else

`python pad/bytemap.py <ABS src>` attributes the `BYTEDIFF` per case (`wgate` prints one total and
eight offsets, so a residue spread over four cases reads as one number):

    case 0xe7  20 bytes      case 0xe4  11      case 0xd3  7      case 0xbf  4
    (case 0xde 12 and case 0x6f 4 also print -- those are literal-pool RELOCATIONS, which wgate
     masks and bytemap does not; they are not work)

| case | instrs | what | tried and failed |
|---|---|---|---|
| `0xe7` | ~20 | `battle`/`rec` swapped: ROM `battle=r5, rec=r4`, ours `battle=r4, rec=r5`. `snap=r6` and `fields=r7` are already right | declaring `char *rec;` before `battle` and assigning later (the trick that fixed `0xce`) |
| `0xe4` | 2 | 9 bytes: `s` now matches, only pool/byte are swapped (ours r3/r1, ROM r1/r3) | a named pointer local; binding the boolean first (fixes the colouring but adds `and r0,r0,#0xff`, +4 B); `caseperm` (the case has no declarations to permute) |
| `0xd3` | 4 | ROM `base=r1, value=r2`, ours `base=r2, value=r1` | `*w \|= x`; no-`w` form; a named `wv` value local; `wv` declared first; `caseperm` depth 3 |
| ~~`0xbf`~~ | — | **CRACKED**, see above | |

Grids run against the REAL function since (`pad/casegrid.py`), all `BEST (base)`:

* `0xd3`, 27 variants (declaration order x RMW spelling) and 14 more (call-result position, a return
  variable, integer types, decl/def splits, a spare local). The ORDER is what matters and the
  current one is already optimal: `bit,one` = 7 bytes, `one,bit` = 11, `one` after the call = 14,
  and every spelling of the read-modify-write is identical. `-inline auto` changes nothing.
* `0xe4`, 14 variants (pointer/boolean order, explicit byte RMW, ternary, `!`) and 9 more (duplicate
  pool literal, array index, wrapper struct, reference). `unsigned char v` avoids the `and r0,#0xff`
  that an `int` temp costs, but the naming never moves.
* `0xe7`, 24 variants (4 address-chain forms x 3 positions for the fold-defeating constant x
  declaration-vs-assignment for `snap`), 11 more over the INNER block (`fields`/`level`/`comb`
  order, `comb` inlined, the load split, a guarded else), and 3 that bind the call result to a
  separate `b0` so `battle` is defined inside the guard. Every one is 20 bytes.

Then the PRODUCT grids, built because 0xbf needed three simultaneous changes and each alone was
inert — still all `BEST (base)`:

* `0xe7` x32 (chain x constant position x `level` type x inner form), x3 deriving `battle` back from
  `rec` (each +4 for the `sub`), x14 over the TYPE of the fold-defeating constant and of `rec`.
* `0xd3` x24 (splitting `one` into two independent literals — one for the shift, one for the return
  — at four positions x three RMW spellings), x15 over the pointed-to type of the updated word and
  the base pointer's char type, x3 nesting forms (bare block, `do{}while(0)`, a real guard: +4).
* `0xe4` x16 (message field / pointer / boolean each bound or inline, in both orders).

##### `0xe4`: SIX committed sources have the shape, and a bisection localises the trigger

`pad/shapecat.py` searches for a case's micro-shape and cross-references the committed corpus. **Its
first run was WRONG** and so was `findshape.py`'s: both keyed sources by the address in the FILENAME,
which misses every semantically-named file — `PeekInputLogB.cpp` *is* `func_020a1fa8`. Fixed to read
the `// USA:` line as well. Any "0 inside a committed source" from before that fix is void.

With the map fixed, `0xe4`'s shape (`ldr rP,[pc]` then `ldrb rB,[rP,#imm]` with `rP < rB`, the
direction we never get) has **18 sites, 6 of them in committed, byte-exact sources**:

    020a1f50  r1/r2  PopStack1AndTrigger.cpp              <- the ROM's pairing
    020a1fd4  r1/r2  StoreByteAtCountTail020a1fd4.cpp     <- the ROM's pairing
    0205beac  r0/r1  CheckByteFlagAndInvoke0205bea0.cpp
    02096844  r0/r1  MaybeDispatchPendingAction02096824.cpp
    020a1fa8  r0/r1  PeekInputLogB.cpp
    020e1e58  r0/r1  FindRectIndexForCurrentCoords020e1e4c.cpp

Both `r1/r2` examples reach the pool through a **volatile cast at the point of use**
(`*(volatile unsigned char *)(&data_02109da4 + 1)`), not a volatile declaration. Applying that to
`0xe4` — as a cast, as a volatile pointer local, and as a volatile byte read-modify-write — moves
nothing.

`pad/probe_morph.cpp` then bisects from the working function to `0xe4` one step at a time, and the
flip is sharp:

    m0 the committed function verbatim                        pool r1
    m1 guard removed                                          pool r1
    m2 store target becomes a bitfield at a fixed offset       pool r2
    m3 non-volatile                                           pool r2
    m4 value becomes `msg->p1 == 0` through a pointer param    pool r2
    m5 ADD `return 1;`                                        pool r3   <- the trigger
    m6..m12 every other way of writing that return value      pool r3

**Having a return value at all is what pushes the pool pointer up a register** — not how it is
written (a variable, an `unsigned` return, a declaration/assignment split, a volatile cast: all
`r3`). The ROM has a return value *and* keeps the pool in `r1`, which under the creation-order model
means its shift temp is created BEFORE the byte load.

**Acting on that took `0xe4` from 11 bytes to 9.** Write the bitfield insert out by hand and bind
the shifted value to its own local BEFORE the byte is read:

```c
unsigned char *p = (unsigned char *)&data_02109bf4 + 0xc8;
unsigned int v = (unsigned int)(msg->p1 == 0);
unsigned int s = (v << 31) >> 29;        /* the ROM's lsl #31 / lsr #29 pair, explicit */
unsigned int b = *p;
*p = (unsigned char)((b & ~4u) | s);
```

The shifted value now lands in `r2` exactly as the ROM has it, and `lsl r2, r0, #0x1f` matches. What
is left is a clean two-register swap: ours `pool=r3 byte=r1`, ROM `pool=r1 byte=r3`, with `r2` taken
by `s` in both. Nine further forms (pointer bound at every position, `b` as `unsigned char`, the
byte read inlined, the OR accumulated into `s` instead of into `b`, the mask split out) all stay at
9. **The same technique applied to `0xd3` — binding `one << bit` to a local at every position
relative to the pointer and the load, 16 forms — does not move it.**

##### `0xe7`: the ladder flip is PARAMETER vs CALL RESULT (`pad/findladder.py`, `pad/probe_morph2.cpp`)

The first `0xe7` shape query was mis-specified twice over — it required the first `add` of the chain
to target a callee-saved register (even our own target does not), and it masked the destination
field out of the `mov rA, r0` test, so `mov r5,r0` never matched `0x01A00000`. Both fixed. The
corrected query finds **8 sites, 4 in committed byte-exact sources**:

    0202bd6c  saved=r7 derived=r4  CheckBitFlagsAcrossEntries_0202bd68.cpp
    02058708  saved=r6 derived=r4  InitAllEntries02058704.cpp
    0208762c  saved=r10 derived=r7 InitRegionArray02087628.cpp
    02090298  saved=r9 derived=r4  AssignUniqueByteId_02090294.cpp

Morphing the first of those toward `0xe7` (`pad/probe_morph2.cpp`) isolates the trigger in one step:

    n0  saved value is a PARAMETER      mov r7,r0 ; add r4,...   derived BELOW saved  = the ROM
    n1  identical body, saved value is a CALL RESULT
                                        mov r4,r0 ; add r5,...   derived ABOVE saved  = ours

**Nothing else in the body matters** — n2 and n3 vary the loop, the guard and the use counts and
never reach the ROM's order. `0xe7`'s `battle` is a call result, so its ladder is the n1 case by
construction, and the ROM's is the n0 case. The obvious bridge — put the body in a helper that takes
the value as a parameter and let it inline — is closed: mwcc leaves the helper out of line even with
`-inline auto` and even with `#pragma always_inline on`. A function-scope `battle`
(`pad/e7fnbattle.py`) does not move it either, because a live range starts at the DEF.

That is the whole of `0xe7`'s 20 bytes, stated as a mechanism rather than a mystery. Whatever makes
a call result colour like a parameter is the remaining question, and these narrow it further:

* **It is not "live from function entry".** `n8` gives the call result a variable declared and
  initialised (`void *self = 0;`) before the branch that calls, so its live range starts at entry —
  and it still takes the LOW register. Being a formal parameter is what matters, not the range.
* **It is not the use count, the loop, or the guard** — `n2`, `n3`, `n7`, `n9` vary all three.
* **A parameter that stays live past the call does not lend its colouring** — `n5` and `n6` add one
  and two such parameters; the derived pointer only reaches `r4` in `n5`, and there only because the
  call result never needed a callee-saved home at all (mwcc kept it in `r0`), which is not `0xe7`.

The one remaining reading is the register-reuse tie-break: in `0xe7` the three parameters all die
before the call, so `r4`/`r5`/`r6` are all reusable, and the ROM picks `r5` (whose occupant,
`param3`, is unused in this case and therefore dead longest) where we pick `r4` (whose occupant,
`msg`, died most recently). Nothing source-level has been found that reverses that preference.

##### `0xd3`'s other site, read directly (`pad/romdis.py`)

`020aa51c` is in unsplit code, so `wlist` cannot reach it; reading the bytes shows it touches the
SAME structure as case `0xd3` — `add r1, r6, #0x1840` / `ldr r2, [r1, #0xb48]`, and four
instructions later `ldr r3, [r1, #0xb4c]`, which is `0xd3`'s exact field. But its base is a
long-lived value in `r6`, not a fresh call result in `r0`, and it sits inside a large field-copy
routine — so it is evidence that the structure is shared, not a small laboratory.

##### `0xd3`: this shape occurs TWICE in the whole ARM9, and the project has never once produced it

`pad/findshape.py` scans the pristine image for `add rLOW, rBASE, #imm` immediately followed by
`ldr rHIGH, [rLOW, #imm]` — the pairing case `0xd3` needs — and cross-references every hit against
the committed sources.

    169 add/ldr pairs in ARM9;   4 fall inside a committed source
    of those 4, the only one with distinct registers is ClampArg1_02008f3c.cpp -- and it is
    r2/r1, OUR direction, not the ROM's

    --twonode (decoded add immediate NOT a multiple of 0x1000, so a genuine two-node address,
    and a load offset >= 0x100, so a real field access):

        02019114  r0/r2      020193d0  r0/r2      020aa498  r0/r2
        02063db0  r1/r2   <- case 0xd3 itself      020aa51c  r1/r2

Five sites. **`02063db0` is `02061c04 + 0x21ac` — our own case.** The only other `r1/r2` instance,
`020aa51c`, sits in a region `symbols.txt` has no function for, so it cannot be used as a
laboratory. **mwcc has never emitted this pairing anywhere in the project**: not in 6202 config'd
functions, not in any committed source, and not from ~800 source forms across 24 builds, 29 flag
sets and 40 pragmas.

That reframes the residue. It is not a quirk of how case `0xd3` is written — it is a codegen shape
this toolchain does not produce. And `02061c04` **already** needs a non-default compiler
(`2.0/sp2p2` against the project's `sp1p5`), which is independent evidence that its translation unit
was built differently from the rest of the game. The unexplored axis is that TU's build
configuration as a whole, not the compiler version alone.

*(Side find, unrelated to c04: the scan lists small UNMATCHED functions carrying the ordinary
one-node form of the shape, which the toolchain does reproduce — `func_0207bfac` and `func_0208e778`
are 44 B each and identical in structure (a linked-list walk then a counter bump), `func_02098970`
is 92 B. Cheap matches for whoever wants them.)*

**`0xe7`'s early-return form is a TRAP worth naming.** Rewriting `if (!snap->done) { … }` as
`if (snap->done) return 1;` (`pad/e7flat.py`) drops `casescore`'s `reg` from 29 to 9 and looks like
the crack. It is not: the case goes 4 bytes OVER (the early return materialises `mov r0,#1` before
the branch), and `casescore` excludes unaligned cases from the `reg` count — the same artefact as
`opt_propagation on`. The registers in it are still `battle=r4`. **Always read `unaligned` before
believing a `reg` drop.**

Every one of them is the SAME symptom: our scratch/callee-saved names are the reverse of the ROM's
while the instruction sequence is identical. Source-level reordering does not move it, which is the
signal that the lever is elsewhere — a `colorsweep` rule that renames rather than reorders, or an
extra live temp.

**`colorsweep --budget 1200` with `CS_SCORER=case` came back `(no change)` on this base** — all 21
rules, 1200 compiles (`$SP/wlog/colorsweep_c04c.log`). On the PREVIOUS base its only find was
`r13_dup_pool_literal` at `&data_0211e33c`, which is already applied. **Do not re-run it on these
four; it is exhausted.**

##### Two allocation rules, measured — and the wall between them

* **Callee-saved (`r4`+) follow the order the values are DEFINED.** Proved directly: swapping
  `int bit = msg->p1;` and `int one = 1;` in `0xd3` swapped `r4`/`r5` and nothing else. This is the
  same lever as the `0xce` fix, where hoisting `int rowBase;` out of the loop lengthened its live
  range and rotated four registers into place.
* **Scratch (`r1`–`r3`) numbering does not respond to ANY source form.** In `0xd3` the address wants
  `r1` and the value `r2`; ours is the reverse, and it survived: compound assignment, the no-pointer
  form, a named value local, the value local declared first, the base declared before the call, the
  base offset bound to a local, `base[0xb4c/4]` indexing, and a four-statement split. `0xbf` and
  `0xe4` behave the same way.

**`0xe7` is the case that breaks the first rule**, which is why it does not yield: the ROM defines
`battle` first (`mov r5,r0`) and still gives `rec` the LOWER register. `rec` cannot be defined before
`battle` in C because it is derived from it, and every workaround costs bytes — deriving `battle`
back from `rec` (`rec - 0x7504`) does put `rec` in `r4`, but mwcc then clobbers `r0` with
`add r0,r0,#0x104` and has to rematerialise `battle`, +4 B, with `snap` landing in `r5` instead of
`r6`.

**Also disproven this session, all zero-change on the four clusters:** a 40-entry pragma sweep
(`pad/pragmasweep.py` — `register_coloring`, `opt_lifetimes`, `opt_common_subs`, `scheduling`,
`peephole`, `global_optimizer`, `optimization_level 0..4`, `optimize_for_size`, `ARM_conform`,
`opt_pointer_analysis`, …); the three equal-scoring builds `sp2p2`/`sp2p3`/`sp2p4` emit
byte-identical registers; perturbing the NEIGHBOURING case `0xd2` does not move `0xd3`'s naming, so
the allocation is local, not a knock-on; a function-scope declaration for `0xe7`'s `rec`
(`pad/e7rec.py`) and a dead initialiser (`char *rec = 0;`) both leave the ladder alone, because a
live range starts at the DEF, not the declaration; nesting `0xe7`'s block scopes three deep.

**Careful with `opt_propagation on`: it looks like it improves the colouring (`reg` 32 -> 8) and does
not.** `casescore` SKIPS unaligned cases when it counts `reg`/`shape`/`sp`, and propagation-on makes
7 cases the wrong length — including the two that carry 24 of the 32 rows. Its `reg` count is only
comparable between candidates with the SAME `unaligned`.

##### THE COMPILER AXIS IS EXHAUSTED — and the build set on disk is INCOMPLETE

`wgate.py` now honours `MWCC` (it only read `cc_overrides.txt`, keyed by basename — the trap that
made a scratch copy gate at 3853 instead of 144). With that, the whole function was gated under
**all 24 mwccarm builds**:

    2.0/sp2p2, sp2p3, sp2p4   BYTEDIFF 42     <- the only three that reach it
    2.0/sp1, sp1p2, sp1p5, sp1p6, sp1p7, sp2  BYTEDIFF 3836
    2.0/base                  0x27d4 (8 short)
    1.2/*                     0x28c8-0x28cc (over)
    dsi/*                     0x25f4-0x260c (short)

Flags too: `pad/flagsweep.py` scores 29 command lines (`-proc arm7tdmi/arm9tdmi/arm946e/arm966e/
arm1020e`, `-O3`, `-O4`, `-O2,p`, `-O2,s`, `-Op`, `-opt speed/space/level=2/noglobal/nopeephole/
noschedule/schedule`, `-char unsigned`, `-enum min`, `-sym off`, `-inline auto/off`, `-str reuse`,
`-nointerworking`, …). **Nothing beats the project's own line.**

**The gap:** the 2.0 directory holds `base sp1 sp1p2 sp1p5 sp1p6 sp1p7 sp2 sp2p2 sp2p3 sp2p4` — the
numbering skips **`sp1p1`, `sp1p3`, `sp1p4` and `sp2p1`**. `sp2` fails on case-body lengths and
`sp2p2` passes, so **`sp2p1` is the one untested build sitting exactly in that gap**, and it is the
only compiler-axis lead left. `tools/download_tool.py mwccarm` re-fetches the same
`decomp.aetias.com/files/mwccarm.zip`, so getting it needs a different drop — a user decision, not
something to fetch unasked.

**Explicit register variables are NOT available.** mwcc accepts the `asm("rN")` syntax but rejects a
local ("cannot declare global register variables after code has been generated") and rejects a
file-scope one ("illegal storage class"), so pinning a register in C is off the table
(`pad/probe_regvar.cpp`, `pad/regpin.py`).

##### The source axis is exhausted too, by brute force rather than by hand

`pad/d3sweep.py` emits **512** variants of case `0xd3`'s read-modify-write into ONE translation unit
(4 spellings of `bit` x 4 of `one` x 4 address forms x 4 RMW forms x 2 returns) and greps the
disassembly; `pad/d3sweep2.py` adds **40** more over the lvalue KIND (pointer, C++ reference, struct
member, member reference, union member and reference, array element, array reference) x 5 RMW
spellings. **0 of 552 put the address in `r1`.** `pad/romwords.py` confirms there is nothing to
misread: ROM `e2801d61` against ours `e2802d61` — the same instruction with the destination nibble
changed.

##### What the ISOLATED PROBES established (`pad/probe_alloc.cpp`, `probe_d3.cpp`, `probe_e4.cpp`, `probe_e7.cpp`)

Iterating a construct against the 76 KB source costs a minute; a ten-line probe costs a second, and
`pad/probe_cc.py` now honours `MWCC` (it was pinned to `sp1p5`, so every probe ever run against a
`sp2p2` function was measuring the wrong compiler). Four mechanisms came out of it:

* **Callee-saved order is REVERSE definition order — until a nested block contributes a long-lived
  value, and then it is FORWARD.** `g0`–`g4` (four call results in one block) give
  `first -> r7 … last -> r4`; `g5`, which defines the fourth value inside an `if`, gives
  `first -> r4 … last -> r7`. Every big-switch case has a guarded block, which is why `02061c04`
  reads as plain forward order and why [[dqix-regalloc-rule-corrected]] needs this qualifier.
* **An address as ONE IR node lands in `r1`; as TWO nodes it lands in `r2`** — independent of the
  constants (`d1`/`d11`/`d12`/`d14` against `d0`/`d8`/`d10`/`d13`/`e1`–`e9`).
* **mwcc's one-node split is always `add = C & ~0xfff`, offset = `C & 0xfff`**, on all seven 2.0
  builds. `0xd3` needs `add #0x1840` + `[#0xb4c]`, which is a valid split of `0x238c` but not that
  one — so the ROM's is a TWO-node address, which our compiler always gives `r2`. That is the
  contradiction the case is stuck on, stated exactly.
* **`0xe7` needs `battle` in a callee-saved register AND `rec` below it.** `f4` shows `rec` takes
  `r4` the moment `battle` stops needing a home, so the ladder is not arbitrary — but `battle` is
  defined first and no form moves it: a copy of the call result is coalesced (`f5`, `f6`), an early
  return (`f7`), `level` first (`f8`), `comb` inlined (`f9`), `rec` read in the guard (`f10`), a
  function-scope `battle` (`pad/e7fnbattle.py`) and a fourth function parameter all leave it at `r4`.
* `0xe4`'s pool pointer reaches `r1` only in `k6`, the hand-written read-modify-write — which is a
  different instruction shape (48 B against 52). Eight other forms (`k0`–`k5`, `k7`–`k11`) all give
  `r3`.

#### CRACKED this session — five, each general

| idiom | symptom | fix | worth |
|---|---|---|---|
| **constant in a local defeats address folding** | ROM materialises `add rD,rBase,#K` and then uses `[rD,#small]`; ours folds K into every offset and is one instruction short | with `#pragma opt_propagation off` in force, bind the constant to a local: `int snapOff = 0xcc; p = (T *)(rec + snapOff);`. `rec + 0xcc` written literally always folds | closed the LAST size hole in `0xe7`; took the function from 4 bytes short to exact, and with it the `push {r3}` and `sub sp,#0x220` |
| **one struct for a whole stack region** | objects sit at the wrong offsets and NOTHING moves them | mwcc ignores both declaration order and block scope when placing these (measured: exhaustive permutation, and hoisting a local into its case, both changed nothing) — so make the relative offsets a property of ONE struct type and reach the members through it. `pad/lowregion.py` is the transform | `sp` 44 -> 9 in one compile |
| **a local's SIZE is a frame lever** | the frame is 4 bytes short and one object at the TOP is 4 low, while every other slot matches | grow the object DIRECTLY BELOW the misplaced one — here `sa` is `{int a0,a1,a2,a3;}`, not three ints, because the callee fills four slots. Find it with `pad/spslots.py`: the ROM-only/ours-only pair brackets the gap | `sp` 9 -> 0, and the prologue went byte-exact |
| **declaration hoist rotates a register ladder** | 4 callee-saved registers are a ROTATION of the ROM's, not a permutation | declare the variable that the ROM gives the LOWEST register in the OUTER block (`int rowBase;` before the loop, assigned in it) | `reg` 25 -> 12, `0xce` closed |
| **64-bit accumulate emits `adds`** | ROM `add rD,rA,rB`, ours `adds rD,rB,rA` | `long long s = A; s += B;` makes mwcc keep a carry chain. Two `int` locals summed as `int` gives the plain `add`, and fixes the operand order with it | `0x8f` closed |

Also applied: `colorsweep`'s `r13_dup_pool_literal` at `&data_0211e33c` (case `0xa1`) — it does not
move `wgate`'s count (the row is a masked relocation) but it removes a real pool mismatch, 72 -> 68.

#### New tooling

* **`pad/spslots.py <ABS src>`** — every distinct `sp+N` the ROM references and every one ours does,
  printed as `ROM-only` / `ours-only`. `stackmap.py` reports only each case's LOWEST slot, so an
  object no case reaches at its base is invisible to it — which is exactly the shape of a missing or
  undersized local. This found the 4-byte frame gap in one compile after a session of guessing.
* **`pad/lowregion.py <src> <out>`** — folds c04's five low-frame locals into one `struct LowRegion`
  at the ROM's offsets. Read it before inventing a sixth: `frame` and `d5Local` share one struct
  because `sizeof(SharedFrame)` rounds to 12 and the ROM puts `d5Local` at `frame+10`, inside that
  padding.
* **`pad/vary.py <src> <out> <from-file> <to-file>`** — one-shot variant maker that FAILS if the
  pattern does not occur exactly once. Written because a silently-missed replace reads as "the
  compiler ignored my change".
* `pad/e7ref.py` — the C++-reference binding for `0xe7`'s snapshot pointer. It did NOT work (mwcc
  folds a reference exactly like a pointer); kept as the disproof.

#### Disproven this session — do not retry on `0xe7`'s address chain

`snap` is `rec + 0xcc` and mwcc folds it into every addressing mode. None of these changed one byte:
`register` on the declaration; a separate declaration + assignment statement; `&((RecE7 *)rec)->snap`
with a wrapper struct; a C++ reference (`pad/e7ref.py`); `#pragma opt_strength_reduction off`;
writing the chain from `e7base + 0x74cc` (that one also merges `+0x104` with `+0xcc` and gives the
WRONG split, `0x1d0 + 0x7400`). **Only the constant-in-a-local form works.**

### `main:02061c04` — PROLOGUE BYTE-EXACT, 127 of 134 case bodies the right length

State at the end of the 2026-08-26 session. Source: `$SP/c04work/c04.cpp`, checkpointed as
`c04work/c04_best.cpp`. Gate: **`BYTEDIFF` — the function is the RIGHT LENGTH (`0x27dc`)** and 123
of 134 case bodies are individually the right length too. `pad/casescore.py` reads
`shape=27 reg=24 sp=82 unaligned=11`: 133 real instruction differences, the rest of the raw byte
count being relocations and literal pools.

    python pad/fulldiff.py <src>              classify every differing instruction, per case
    python pad/fulldiff.py <src> --list shape classify + print one kind (reg | sp | shape | pool)
    python pad/casetable.py <src>             every case's size delta, one compile
    python pad/headdiff.py <src> [n]          the prologue
    python pad/stackmap.py <src>              each case's lowest stack slot, ROM against ours

Residue: `reg=97` (register naming), `shape=56` (real per-site differences), `sp=84` (stack layout),
`pool=533` (relocation and pool noise, not work). **The whole prologue now matches byte for byte** —
`push {r3,r4,...}`, `sub sp,#0x220`, and `mov r6,r0` / `mov r5,r2`.

#### 144 BYTES FROM MATCHING — what is left, and what is PROVEN not to reach it

`wgate` on `$SP/c04work/c04.cpp` (`MWCC=2.0/sp2p2`): **`BYTEDIFF 144`**, from 5183 at the start of
the day. All 134 case bodies the right length, total exact, prologue 4 bytes of stack short.
Classified residue: `reg=67  shape=21  sp=47`.

| block | diffs | what it is |
|---|---|---|
| `0xe7` | 46 | a 3-register permutation: the target colours `rec=r4, battle=r5, snap=r6`, we get `battle/snap/rec` in declaration order. Plus the address split (`add r1,r5,#0x104` / `+0x7400` / `+0xcc`) which folds in ours |
| stack layout | 47 | five constant shifts: `fb` +4, `d5Local` −12, `frame` +8, `packed` −28, frame size −4 |
| `0xce` | 13 | register ladder starts one register lower than the target's |
| the rest | ~38 | `pop {r3,...}` (the frame's 4 bytes), literal-pool rows misread as code, `0xe4`/`0xd3`/`0xbf` scratch registers |

**Three searches came back empty — do not re-run them:**

* **`colorsweep --budget 400` with `CS_SCORER=case`: no change.** All 21 rewrite rules, twice.
* **`pad/layoutsweep.py` hill-climb, both builds: one −1 improvement then a local optimum.**
* **Exhaustive permutation of the five misplaced low-region declarations (all 120 orders, via
  `layoutsweep --only`): no change at all.** This is the important one — **the stack layout of
  those objects is not a function of declaration order**, so no reordering of the source can move
  them. The target packs them tightly (its `frame` spans `0x10..0x1a` and `0xd5`'s struct starts at
  `0x1a`, inside what would be `frame`'s alignment padding) which is what stack-slot COALESCING of
  case-scoped locals looks like. Ours are all function-scope now, so their live ranges span the
  whole function and nothing coalesces. Getting that packing back means making the low-region
  objects case-locals again — which is testable now that the array-member copy form removes the
  copy-init constraint that forced the hoist in the first place.

`pad/caseperm.py <src> <case>` permutes the declarations INSIDE one case (only swapping declarations
that do not reference each other) — built for `0xe7`, found nothing at depth 1; manual `fields` /
`level` / `comb` reorderings scored 211 and 220 against 198.

#### 234 BYTES FROM MATCHING — and the copy form that got it there

`wgate` on `$SP/c04work/c04.cpp` (build `2.0/sp2p2`): **`BYTEDIFF 234`**, down from 5183 at the start
of the day and 4623 an hour before. **All 134 case bodies are exactly the right length** and the
total is exact. What is left: `shape=46 reg=67 sp=47`, and the prologue is 4 bytes of stack short.

Three cracks did it, in order of size:

1. **`2.0/sp2p2` instead of the project default** (see below).
2. **A struct whose member is an ARRAY assigns inline as `ldm`/`stm`.** This is the copy form that
   reaches an EXISTING object in three instructions, and it is what unblocked the whole stack
   layout. Measured (`pad/probe_copyforms.cpp`, `pad/probe_veccopy.cpp`):

   | form | codegen |
   |---|---|
   | `Vec3 a = b;` (copy-init, fresh declaration) | inline `ldm`/`stm` — but only into a NEW object, and a chain collapses to one copy |
   | `a = b;` (assignment, struct of 3 scalars) | `bl` to an out-of-line helper, in a SECOND `.text` |
   | `new (&a) Vec3(b);` (placement new) | inline `ldm`/`stm`, both copies survive, **plus `cmp rX,#0`** — 4 bytes per site |
   | **`*(Vec3W *)&a = *(Vec3W *)&b;`** with `struct Vec3W { int w[3]; };` | **inline `ldm`/`stm`, no guard, no helper** |
   | `memcpy(&a, &b, 12)` | a real call |

   A copy also survives instead of collapsing when the SOURCE object is used elsewhere.
3. **Every local hoisted to function scope in the order the target's stack addresses imply.** With
   the array-member copy form, `midOut`/`half`/`halfOut` can be function-scope — which is what the
   target's layout requires and what copy-init made impossible. Moving `halfOut` alone took the
   dominant stack shift from +16 (39 references) to +4; hoisting the rest took `sp` from 85 to 47.

#### THE COMPILER IS A VARIABLE — `02061c04` needs `2.0/sp2p2`, not the project default

Measured 2026-08-26 with `pad/ccscore.py`, which scores the candidate under all 22 mwccarm builds
using the per-case classified score:

| build | score | detail |
|---|---|---|
| `2.0/sp1p5` (project default) | 918 | shape=27 reg=24 sp=82 **unaligned=11** |
| `2.0/base` | 1080 | shape=23 reg=24 sp=68 unaligned=14 |
| **`2.0/sp2p2` / `sp2p3` / `sp2p4`** | **382** | shape=37 reg=24 sp=82 **unaligned=2** |
| `1.2/*` | 2134 | shape=55 reg=82 sp=40 unaligned=28 |
| `dsi/*` | 8576 | every case body the wrong length |

**132 of 134 case bodies are the right length on sp2p2 against 123 on sp1p5**, and the two that are
not (`0x8f` +4, `0xe7` −4) cancel, so the total stays exact. The first differing byte moves from
`0x24` to `0xd8`.

An earlier matrix run said "every 2.0 build is identical" and the question was dropped. That run
compared RAW BYTE DISTANCE on a source whose frame was still wrong: byte distance counts relocations
and literal pools, and a wrong-length function hides build differences behind them. **Score a build
sweep with `casescore`, never with a byte count.** `tools/cc_overrides.txt` now carries `c04.cpp`;
the landed filename needs its own line when this commits. Every `pad/` tool takes `MWCC=<ver>/<sub>`.

#### The stack layout rule, measured rather than guessed (`pad/probe_layout*.cpp`)

Locals with distinct sizes and escaping addresses, compiled and read off:

* **big arrays go to the TOP of the frame**, whatever their declaration position;
* everything else is allocated **bottom-up**: the function-scope group first, then the case-locals,
  each group internally in **reverse declaration order** (last declared gets the lowest address);
* small locals fill alignment gaps left in that layout, which is why `0xd5`'s 6-byte struct sits at
  `sp+0x14` between two unrelated objects.

Identical on `sp1p5` and `sp2p2`. **The rule does NOT yet predict `02061c04`**: there the case-locals
sit BELOW the function-scope group, the opposite way round. Whatever the missing condition is, it is
not the count, not a function-scope local being used inside a case, and not the presence of a big
array — all three are probed. Extend `probe_layout` until it reproduces c04 before trusting any
hand-derived declaration order.

`pad/layoutsweep.py` hill-climbs the declaration order against `casescore` (adjacent transpositions
plus move-to-front/back). It found one improvement (`dbAlloc` to the front, −1) and then a local
optimum on both builds — the neighbourhood does not include hoisting or un-hoisting a case-local,
which is what the remaining `sp=82` needs.

#### Neither DQIX project has this function

`DQIX/dqix-decomp` (upstream) and `StanHash/dqix` (an independent decompilation of the same game)
were both searched for `02061c04`: **zero hits in either.** There is no external source to port from
for this one, and no reference project has published notes on the mwccarm stack-layout ordering.

#### The two whole-function levers, both CRACKED

* **The frame is a SOURCE property, not a compiler-build one.** `pad/ccmatrix_c04.py` compiles the
  function with all 22 mwccarm builds: every 2.0 build gives the same `0x140` frame, so no build
  switch was ever going to produce `0x220`. The missing 224 bytes were four undersized locals, found
  by comparing each case's lowest stack slot (`pad/stackmap.py`) against the target's:
  `0xcd`'s `char local[0x40]` is really `[0xb8]`; `CmdMsg64` is **0x1c bytes, not 0xc** (the five
  cases that build one on the stack put the next local exactly 0x1c higher); `0x7f`'s second
  out-param struct is 12 bytes, not 8; `0xd8`'s `&local` is a 12-byte struct, not an `int`.
* **The parameter colouring followed the frame.** `obj→r6, param3→r5` came out on its own once
  the shared 16-byte struct at `sp+0x20` was modelled as a FUNCTION-SCOPE `struct FrameB fb;` — 148
  register diffs fell to 51 with no other change. Three separate case-local structs do NOT work:
  mwcc eliminates the dead stores in `0x94`, and the frame shrinks by 20 bytes again.
* **Hoisting every case-local to function scope is what places them.** `pad/hoist_c04.py` moves the
  13 hoistable declarations to the top in a chosen order; that alone took the frame from `0x21c` to
  exactly **`0x220`** and took aligned cases from 112 to 123. Our build lays locals out
  first-declared-highest; the target's order is recoverable by reading its stack addresses downward.

#### The sweep now scores this function correctly (`CS_SCORER=case`)

`colorsweep` scored with `wdiff`, which aligns the WHOLE function — so on a 134-case switch one
wrong-length body made every later instruction read as a diff and the score stopped tracking
progress (5562 "differing bytes" against 204 real ones). `pad/casescore.py` aligns each case body
against its own jump-table entry, drops relocation and pool rows, and weights
`shape*4 + reg + sp + 64/unaligned-case`. `colorsweep.py` uses it when `CS_SCORER=case` is set;
the default path is untouched and `regress.py --slow` is 10/10 with the change in.

    cd $SP && CS_SCORER=case python colorsweep.py main 02061c04 c04work/c04.cpp --budget 600

**The declaration-order lever is real and cheap.** Five cases declared `node` before `list` where
the target declares `list` first; swapping the two lines took register diffs from **97 to 64** in one
compile. Look for it wherever `fulldiff --list reg` shows two locals with swapped registers.

#### How mwcc copies a 12-byte struct (measured, `pad/probe_veccopy.cpp`)

| form | codegen |
|---|---|
| `Vec3 a = *src;` (copy-init) | inline `ldm r0,{r0,r1,r2}` / `stm r3,{r0,r1,r2}` — the target's shape |
| `a = *src;` (assignment) | a `bl` to an out-of-line helper that does three `ldr`/`str` pairs, in a SECOND `.text` section |
| `memcpy(&a, src, 12)` | a real `memcpy` call |
| any chain `a = *src; b = a;` | collapses to ONE copy, whatever its length |

This is why the stack layout cannot be finished by hoisting alone: `0x8f`'s `midOut`/`half`/`halfOut`
are copy-initialised, a function-scope declaration turns that into an assignment, and the helper
leaks as `2 .text`. `#pragma always_inline on` does not rescue it — mwcc runs **out of memory** on a
function this size.

#### Still open on this function

* `sp=84`: four objects are still in the wrong place — `fb` (ours `sp+0x58`, target `sp+0x20`),
  `frame` (`0x68` vs `0x10`), `0x7f`'s `packed` (`0x10` vs `0x30`) and `0x8f`'s Vec3 group
  (`0x1c` vs `0x34`). The target's order, read downward, is Vec3s, packed, fb, d5Local, frame,
  a1p1 — which needs `packed` and the Vec3s declared BEFORE `fb`, i.e. hoisted. **`0x8f`'s Vec3s
  cannot be hoisted**: they are copy-initialised (`struct Vec3 midOut = mid;`) and a function-scope
  declaration turns that into an assignment, which mwcc emits as an out-of-line `operator=`.
* `0x8f` +4 and its missing 12-byte `halfOut` slot at `sp+0x140` (mwcc collapses any copy chain to
  ONE `ldm`/`stm`, whatever its length).
* `0xbe` +4, `0xbf` +4, `0xa0` +4: each is a `return 0` that the target reaches with a bare `beq`
  into a shared `mov r0,#0` stub, where ours emits `moveq r0,#0` and branches to the epilogue. Our
  function tail carries one extra `b` for the same reason.
* `0xd3` −4: the target materialises the constant 1 in a register and copies it (`mov r5,#1` /
  `mov r0,r5`). `int one = 1; … return one;` does not reproduce it — mwcc folds the constant even
  with propagation off.
* `0x86`/`0x87` (22 diffs): argument evaluation ORDER inside two calls; every instruction is present
  and the count matches.

#### Idioms cracked in the second half of the session

| idiom | symptom | fix |
|---|---|---|
| signed compare of an unsigned narrow load | target `blt`/`ble`/`bgt`, ours `blo`/`bls`/`bhi` | bind the load to an `int` local first: `int v = *(unsigned char*)p; if (v <= 1)`. Used inline, mwcc picks the unsigned condition. Closed a 20-branch ladder in `0xc5` |
| bit test | target `lsl #29`/`lsr #31`, ours `tst r0,#4` | a bitfield THROUGH A POINTER, same rule as the 9-bit field. `& 4` is always a `tst` |
| boolean argument | target builds the value in a register (`mov r2,#0` … `mov r2,#1` … `mov r0,r2`) | `int flag = 0; if (cond) flag = 1; Call(flag);` — not `Call(cond)` |
| two-call if/else | target calls the same function twice with different constants, ours computes the constant | write both calls out; `Call(x, p1 == 0)` collapses them into one |
| conditional store | target `strhge` / `strhlt`, ours `movge` + `strh` | `if (c) *p = a; else *p = b;` — not `*p = c ? a : b` |
| address split | target `add r0,rX,#0x104` + `add r0,r0,#0xa4`, ours one `add` | two intermediate pointer variables. One variable is not enough — mwcc folds `p + A + B` at parse time |
| parameter width | target converts once (`lsl`+`lsr`), ours three shifts | the extern's declared parameter type is ours to choose: `unsigned short` where a `short` forces a second sign-extending pair |
| word-wide message read | target `ldr r2,[r4,#4]`, ours `ldrh` | the field is a 32-bit read of `msg+4`, not `msg->p2` |
| reload per call site | target re-loads `[r6,#0x332]` at every call, ours caches it | delete the local and re-dereference; the intervening calls kill the CSE |
| guard vs early return | target's conditional branch lands INSIDE the case | `if (cond) { … }` with one `return` after it, not `if (!cond) return;`. Worth ±4 either way — measure both |

### the entry below is the state before the 2026-08-26 session

The function now compiles to **exactly the right length**. What is left is byte-exactness: 5183 of
10204 bytes differ, and the residue is the two whole-function consequences the earlier sessions
deferred — the frame (`sub sp,#0x220` against our `#0x140`) and the parameter colouring
(ROM `obj→r6, param3→r5`; ours `obj→r5, param3→sb`). Checkpoint of this state:
`$SP/c04work/c04_best_exact_size.cpp`.

**The size total is a coincidence, not convergence.** Per-case rows still cancel: `0x94` is 12
short, `0xd6` 12 over, `0x9b`/`0x9c` 4 short each. Do not read a matching total as "only the
registers are left" — read `pad/casetable.py`.

#### New tooling, 2026-08-26 (use these, not the global `wdiff`)

* `pad/casetable.py [src] [--show=c1,c2]` — EVERY case's size delta from ONE compile, and the full
  body of any case you name from the same compile. `casediff.py` costs one compile per case, so
  surveying 134 of them cost 134 compiles and nobody ever did it; the survey is what found the
  OVER-emitting cases (`0xbf` +16, `0xd6` +12, `0xd8` +8) that no short-case list mentioned.
* `pad/headdiff.py [src] [n]` — the prologue and dispatch head, the one region `casediff` cannot
  reach. This is where the frame size and the parameter colouring are visible.
* `casediff.py` is now importable (its main is under `if __name__ == "__main__"`).

**Rows that are pool artefacts, not work:** `0xa0`, `0xa2`, `0xde`, `0xe6`, `0xdf`, and the tail
cases `0x6f`/`0x70`/`0x71`/`0x98`/`0x9a`/`0xa3` (all six share the function's return-0 tail, so their
"‑8" is the trailing literal pool). Chase none of them.

#### Cracked this session — each closed a case and each is general

| idiom | symptom | fix | closed |
|---|---|---|---|
| `#pragma opt_propagation off` | scattered 4-byte shortfalls where the ROM re-materialises a value | the function was compiled with propagation OFF; the pragma belongs in the source, not the flags | +16 bytes, `0xcb` and `0xe8` outright |
| byte field is PER SITE | ours `ldrb [r4,#N]`, ROM `ldrh`+`and #0xff` (or the reverse) | `(unsigned char)msg->pN` emits ldrh+and; `((unsigned char*)msg)[2N]` emits ldrb. Neither is "the" right form — check each site | `0x95`, `0x96`, `0xaa`, `0xdb`, `0x86`, `0x87`, `0xe8` (7 cases, 32 bytes) |
| non-immediate mask | ROM `ldr rX,[pc]` + `and`, ours `and rX,rX,#imm` | write the mask as the full 32-bit complement the original used (`flags &= ~0xfffcu`), NOT the arithmetically equal short form (`&= 3`). An ARM-encodable immediate is the tell that you simplified it | `0xa1`, which went from 52 bytes over to instruction-identical |
| 9-bit field extract | ROM `lsl #23` + `lsr #23`, ours `and` against a pool constant | a bitfield reached THROUGH A POINTER. Every shift form (`(x<<23)>>23`) folds back to `and`; a bitfield in a struct or union LOCAL is correct but gives the local a stack home (+8) | `0x90` |
| reload after a store | ROM re-loads the same fields a second time, ours keeps them in locals | drop the `int minX = ...` locals and re-dereference in both expressions: an intervening store through a pointer kills the CSE and forces the reload | `0x8f`, +24 bytes |
| pointer copy loop | ROM `ldrh [r]!`/`subs`/`strh [r]!`/`bne`, ours an indexed up-counting `for` | `do { *dst++ = *src++; } while (--n);` with `n` initialised from a variable already live | `0xbf`, +12 |
| guarded call, not early return | ROM's null test skips ONE call and falls through, ours returns | `if (p) Call(...);` — read where the `beq` actually lands before writing an early return | `0xcf` |
| shared exit | ROM `b` into the case's own tail, ours `mov r0,#1; b exit` in each arm | make the two arms a real `if/else` with ONE `return` after it | `0xb3` |
| WRONG CALLEE | one extra `mov r1,#0` in the argument setup | `0xd9` called `_Z12Init020d9decP14Struct020d9deci(node, 0)`; the ROM calls `_Z15InitObj021beba4Pc(node)`, a different 1-arg function. **An arity mismatch in the ARGUMENT SETUP is the visible symptom of a wrong callee** — the `bl` itself is masked by the reloc | `0xd9` |

#### Disproven again, with numbers — do not retry

* **`struct FrameB` at function scope for `0x84`/`0x94`.** The evidence for it is real (`add r2,sp,#0x20`
  in `0xa1` proves the address escapes, and `sp+0x24/0x26/0x28` are written only by `0x94`), but
  writing those fields through the function-scope struct cost **+24 bytes on each of `0x84` and
  `0x94`** — base-register setup and reloads, not the three `strh` we wanted. Reverted. This
  reproduces the earlier session's result from a different direction: the model is right and the
  codegen is not reachable this way.
* **A copy chain does not multiply copies.** `0x8f` needs TWO `ldm`/`stm` pairs; mwcc emits exactly
  ONE no matter how long the chain (`a=diff; b=a; c=b;` → one copy). An inlined by-value helper
  removes them all (388 B, 80 short). 8 bytes of `0x8f` remain open on this.
* `int one = 1; ... return one;` does not produce the ROM's `mov r5,#1` / `mov r0,r5` — mwcc folds
  the constant even with propagation off. `0xd3` keeps its 4-byte residue.

#### Still open, per case

`0x94` −12 and `0x84` −4 (the FrameB dead stores), `0x9b` −4 / `0x9c` −4 / `0xd6` +12 (all three are
frame LAYOUT: whichever build puts the struct past a `strh`'s 8-bit offset pays an `add rX,sp,#imm`),
`0x8f` +4, `0xbe` +4, `0xbf` +4, `0xd8` +8 (ours sign-extends a halfword the ROM takes as
`lsr #16`), `0xc5` −4, `0xd3` −4.

The frame is 224 bytes short of the ROM's `0x220`, and `0x9b`/`0x9c`/`0xd6`/`0x94`/`0x84` are all
downstream of that. Model the missing stack before grinding those five.

**The source is `$SP/c04work/c04.cpp`. It is NOT the file the entry below describes.** That file was
session 4's (`0x14c8`), because sessions 5 and 6 worked on a `cp` of it under two other basenames
and no replay followed the copy. Recovered with
`dqix-recovery/seed_replay.py <base> f.cpp,CmdMsgDispatch_02061c04.cpp,wCmdMsgDispatch_02061c04.cpp
<out> <transcripts>` — seed the replay with the file as it stands instead of with nothing, and pass
every basename the session used. That alone restored `0x14c8 -> 0x22b0`.

**Four idioms cracked this session. Each is general; none is specific to this function.**

| idiom | symptom | fix |
|---|---|---|
| dead-looking range ladder | 20 `cmp lo/blt/cmp hi/ble` pairs where every branch converges on the next instruction and no register is written | each body re-assigns the value the variable already holds, and the variable IS read afterwards: `int a = cur; if (cur >= 0xd && cur <= 0xd) a = cur; else if ...; if (a != b) ...`. A REDUNDANT store keeps its branch; a DEAD one takes the whole chain with it |
| byte-addressed message field | ROM `ldrb [r4,#4]`, ours `ldrh`+`and #0xff` (+4B each site) | `((unsigned char *)msg)[4]`, not `(unsigned char)msg->p2`. Check per case: 0xc7/0xdb/0xe8 really do use `ldrh` |
| clause order | bodies land in the wrong place and every jump-table entry after them diverges, while every body is the right size | mwcc emits bodies in SOURCE order, so the ROM's jump table IS the original clause order. Recover it and apply with `pad/reorder_cases.py` |
| boolean materialisation | ROM `cmp/moveq #1/movne #0/cmp #0/beq`, ours branches straight off the compare (+12B) | bind the comparison to a variable first: `int isTwo = f(x) == 2; if (!isTwo) ...` |

Also closed: `0x99` is **not** a tail-merge into `0x6d` (the old note was wrong) — it is a separate
168-byte loop the original wrote immediately after `0x6d`; cases `0x72/0x73`, `0xab`-`0xae`,
`0xb5`-`0xb7` each need their OWN clause where we shared one; `0xd6`'s three `strh` survive only
because a frame local's address escapes (`struct SharedFrame` at the top of the function, handed to
`CopyOutBattleField0x7ac0` by `0xe7`); `0x8f`'s missing 112 bytes were struct-by-value `ldm/stm`
copies of three `Vec3`s through stack temporaries.

**Read `pad/casediff.py <case>` output before touching any case.** The global `wdiff` is useless
here: one wrong-sized body early makes every later body read as a diff.

**PHASE 3 IS OUTSTANDING FOR ALL OF THESE.** They were cracked by hand and none is a `colorsweep`
rule yet, so today they are worth one function each instead of every future occurrence. Three are
mechanical local rewrites and should become rules (`regress.py --slow` after ANY colorsweep change):

* boolean materialisation — `if (f(x) != K) …` becomes `int t = f(x) == K; if (!t) …`; fires on a
  `cmp/moveq #1/movne #0/cmp #0` residue, +12B per site;
* byte-addressed field — `(unsigned char)m->pN` becomes `((unsigned char *)m)[2N]`; fires on a
  `ldrh`+`and #0xff` where the target has `ldrb`, +4B per site. Must re-gate: some sites really are
  halfword (`0xc7`, `0xdb`, `0xe8` here);
* struct copy — assignment becomes copy-initialisation; fires on an out-of-line `operator=` call
  where the target has inline `ldm/stm`, and on a gate line reading `2 .text`.

The other two are not colorsweep material: the range ladder is whole-construct synthesis, and clause
order is already automated as `pad/reorder_cases.py`.

A fifth idiom, found closing `0x8f`: a 12-byte struct copy must be written as **copy-initialisation**
(`struct Vec3 midOut = mid;`), not assignment. Assignment makes mwcc emit an out-of-line
`operator=` in a second `.text` section and CALL it, where the target has an inline `ldm/stm` pair;
`memcpy` inlines as six separate `ldr`/`str`. Only copy-init produces `ldm r0,{r0,r1,r2} /
stm r3,{r0,r1,r2}`. Watch the gate's section count: `1 .text` is right, `2` means a generated
helper leaked in.

**Do not model a stack store as an escaping frame local without checking.** `0x84`, `0x94` and
`0xa1` all write halfwords into what looks like one struct at `sp+0x20`, but forcing that (a
function-level `struct FrameB` whose address `0xa1` takes) made all three BIGGER — +48 on `0xa1`
alone — and pushed the function 32 bytes over. Those stores are register SPILLS from whole-function
pressure, so they will appear on their own when the colouring matches. Reverted; do not retry.

Remaining, in order of size (56 bytes net short, plus colouring):

* small per-case deltas, measured after the reorder: `0x8f` +32, `0x9a` +28, `0x94` +12, `0x8a` +8,
  `0x95` +8, `0x96` +8, `0x86` +4, `0x87` +4, `0x90` +4, `0x9b` +4, `0x9c` +4, with roughly 24
  bytes of OVER-emission after `0x9c` to find. Work them with `pad/casediff.py`, biggest first;
* `0xa0` is already byte-exact — its "+36" is the ROM's mid-function literal pool, which appears on
  its own once the function is the right length. Do not chase pool-adjacent size rows;
* frame size `sub sp,#0x220` against ours, and the `r4`/`r6`/`r5`-vs-`sb` colouring. Both are
  whole-function consequences; leave them until every body matches.

### the entry below is session 4's state, kept for its per-case notes

### `main:02061c04` (10204B / 0x27dc, the largest function in main) — 63%, three cases left

Six paid sessions, roughly $28.4 total. Cost per emitted byte improved every session
(0.0052 → 0.0035 → 0.0029 → 0.0025), which is why chaining fresh sessions beats one long one: a
fresh session pays about $0.062/message at 10–39 messages against $0.178 past 220.

State at the last session that produced anything (session 5, ended 2026-08-21 05:42Z):

* `0x2298` of `0x27dc` emitted — 63%.
* All **134 of 134** case labels are real; the dense dispatch table compiles back to the ROM's
  `addls pc, pc, rN, lsl #2`.
* The `0xcb`/`0xce` loops matched exact byte size on the first attempt.
* Three cases still fall through to `default`:
  * **`0x99`** — tail-merge. Its jump-table entry points at `+0x5cc`, inside case `0x6d`'s body.
  * **`0xc5`** (~700B) — a range-ladder shape. **Tried and failed**; do not re-dispatch a worker at
    it without a new idea. This is the one to hand-crack.
  * **`0xe7`** (~540B) — bitpack.

Session 6 died on its first API call against the weekly limit and produced nothing. That limit reset
2026-08-24 20:00 America/New_York and is now clear.

The source survives at `$SP/staging/main/CmdMsgDispatch_02061c04.cpp` (40911 bytes, 896 lines),
recovered by transcript replay. **Two header edits from session 5 failed to replay**, so the include
block may be behind what session 5 actually compiled — re-gate before trusting it, and expect the
`SESSION 4 UPDATE` header note to be one revision stale. `$SP/wlist_02061c04.txt` (96140 bytes, the
full 2551-instruction listing) was recovered from `%TEMP%` and is the listing to read.

Priors that did NOT survive: `attempts/Trans_02061c04.cpp`, the complete transliteration used as the
per-case reference. `translate.py` survived, so it can be regenerated — it emits the whole function,
zero UNTRANSLATED, gating at `total=0x2880 slot=0x27dc` (164 bytes over, 1.6%). Its residue is
register discipline: it colours r7/r8/sb where the ROM uses r4/r5/r6. **Declaration order does not
move that** — as-is, reversed, high-first and low-first all emit exactly `0x2880`, because
allocation follows USE order. Do not re-run that experiment.

`$KIT/pad/c04_gencases.py` mechanically emits the 30 trivially-shaped cases and they are all already
written. The remaining work is genuinely 79 distinct shapes over 124 blocks
(`$KIT/pad/c04_shapes.py`) — there is no repeated shape left to exploit, which is exactly why this
function is expensive.

### Other open residues

* **`main:0200b0f0` (868B) — OVERGENERATES by 168 bytes.** Worker missed it at $4.98; the free
  re-gate did not move it: `SIZE/OVERGEN total=0x40c slot=0x364`. Emitting `0x40c` against a `0x364`
  slot is not a colouring problem — something in the C is structurally larger than the target, so
  colorsweep cannot help and neither can a bigger budget. Read the listing for a construct the source
  expands (an inlined helper, a duplicated tail, an if/else the target predicates) before spending
  another session.
* **`add rD, rS, #0` where we emit `mov rD, rS`** (register-to-register copy). `core.md` records ~73
  configurations tried: 26 source shapes, 10 pragmas, 9 optimisation levels, 24 compiler builds.
  Report it, do not grind it.
* **The >1KB band.** 436 functions, zero verdicts ever. Needs a different method (skeleton-first
  decomposition), not more worker slots at the current recipe set.
* **ov029 is a config task, not a decompilation.** dsd typed its remaining code as rodata. Re-type it
  in the config; do not send a worker at it.

