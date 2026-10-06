# LDM/STM FINDINGS — block moves, struct copies, and the OVERGEN trap
> **Measured on mwccarm 2.0/sp1p5.** The ROM now builds with 2.0/sp2p2 and `-str pool,reuse`
> ([[dqix-sp2p2-rebase]]). The labs under `SP/lab/` and `SP/pad/` follow the build again, so a
> rule here is worth re-measuring before it is cited as settled.

plain -O2, `-lang=c++ -inline noauto -fp soft`. Lab: `SP/inv/ldmstm/`.

## 0. VERDICT

**YES — a fully controllable, deterministic lever exists.** Block moves are not scheduler luck:
mwcc's block-copy expander is a fixed table, and the choice between *inline block copy* and
*out-of-line `operator=` helper* (the OVERGEN bug) is decided by two source-visible properties.
Both are stated below as lookup tables.

Also a **scale correction to the brief**: "99% of unmatched >256B functions contain ldm/stm" is
true but misleading — almost all of it is the push/pop prologue.

| unmatched funcs | >256 B | contain ANY ldm/stm | contain a **non-sp** ldm/stm (real block move) |
|---|---|---|---|
| overlays (6319) | 1948 | 1903 (97.7%) | **259 (13.3%)** |
| main+tcm (8464) | 843 | 832 (98.7%) | **173 (20.5%)** |
| all (14783)     | 2791 | 2735 (98.0%) | **432 (15.5%)** |

Real block moves exist in **421 overlay functions / 841 sites**. Bucketed (`SP/inv/ldmstm/classify.py`):

| sites | funcs | bucket | covered by |
|---|---|---|---|
| 385 | 230 | `ldm rS,{r0,r1,r2}` + `stm rD,{r0,r1,r2}` — 3-word copy | RULE A |
| 157 | 123 | `ldm rS!,{r0-r3}` + `stm rD!,{r0-r3}` — copy of ≥5 words | RULE A |
| 55  | 44  | `ldm/stm {2 regs}` pair — **`long long`** | RULE C |
| 34  | 34  | `ldm rS,{r0-r3}` + `stm` — 4-word copy | RULE A |
| 91  | ~70 | lone ldm/stm using `r0..` — call args / by-value / adjacent-field store | RULES D, E |
| ~118| ~100| lone ldm/stm on other regs — allocator-accident merges | RULE D/E, partial |
| 1   | 1   | `{r9,r10,r11,ip}` — hand asm | SKIP |

≈86 % of overlay block-move sites are directly recipe-covered.

---

## 1. RULE A — THE BLOCK COPY (the main family)

mwcc has one block-copy expander. Given a copy of **N words** it emits *exactly* this, always,
transferring through **r0…r(N−1)**:

| N (words) | emitted code |
|---|---|
| 1 | `ldr rT,[rS] ; str rT,[rD]` (offsets folded into the immediates) |
| 2 | `ldr ; ldr ; str ; str` (**never an ldm**; offsets folded) |
| 3 | `ldm rS,{r0,r1,r2}` `stm rD,{r0,r1,r2}` |
| 4 | `ldm rS,{r0,r1,r2,r3}` `stm rD,{r0,r1,r2,r3}` |
| 5 | `ldm rS!,{r0-r3}` `stm rD!,{r0-r3}` `ldr` `str` |
| 6 | `ldm rS!,{r0-r3}` `stm rD!,{r0-r3}` `ldm rS,{r0,r1}` `stm rD,{r0,r1}` |
| 7 | `ldm rS!,{r0-r3}` `stm rD!,{r0-r3}` `ldm rS,{r0,r1,r2}` `stm rD,{…}` |
| 8 | `ldm rS!,{r0-r3}` `stm rD!,{r0-r3}` `ldm rS,{r0-r3}` `stm rD,{r0-r3}` |
| ≥9 | `mov ip,#(N/4)` `ldm rS!,{r0-r3}` `stm rD!,{r0-r3}` `subs ip,ip,#1` `bne` **+ tail block of N%4** |

Facts a worker can key on:

* **N ≥ 3 ⇒ the base addresses are materialized** (`add rX,base,#K` / `mov rX,base`) because `ldm`
  has no offset field. N ≤ 2 ⇒ offsets fold into the ldr/str immediates and **no ldm appears**.
* **The transfer registers are always r0..r(N−1)** for N ≤ 4. If r0..r3 hold live values mwcc
  *moves them out* (into `ip`/`lr`/r4+) rather than picking other transfer registers. So a
  3-word copy on `{r2,r3,ip}` or `{r9,r10,r11,ip}` is **not compiler output** → hand asm → SKIP.
* N ≥ 5 forces `push {r3,lr}` / `push {r4,lr}` because the expander needs `ip` and `lr` as the
  two address registers.
* It predicates like anything else (recipe #3): a 3/4-word copy is 4 instructions with the two
  `add`s, so `if(c) dst=src;` gives `addCC/addCC/ldmCC/stmCC`.
* **The copy width is the struct's ALIGNMENT, not its size.** align 4 → word ldm/stm.
  align 2 (`struct{short v[6];}`) → a 6-iteration `ldrh/strh` loop. align 1
  (`struct{char v[12];}`) → a 12-iteration `ldrb/strb` loop. **A `char[N]` wrapper will not give
  you an ldm.** Use `int` / `unsigned int`.

### Which C forms take the block-copy path

The whole-object block copy is used by **every** copy form *except* copy-assignment:

| form | path |
|---|---|
| `T t = *s;` (copy-**init**) | **always** whole-object block copy, any T, any offset |
| `f(*s)` — pass by value | **always** whole-object block copy (into the arg regs) |
| `return *s;` — return by value | **always** whole-object block copy |
| `*d = *s;` (copy-**assign**) | **memberwise plan** — this is the one that can go out of line |

There is **no implicit copy constructor** in mwcc's output — copy-init is a raw block move.
Only `operator=` is ever synthesized.

---

## 2. RULE B — THE OVERGEN TRAP, EXACTLY

`*d = *s` uses the class's implicit `operator=`, which mwcc emits as a **second `.text` section**
(`_ZN…aSERKS_`) and *calls*. That is the `SIZE/OVERGEN` failure, and it hides from `try.sh`
because `odis.py` prints only the **first** `.text` (use `ra/odis2.py` to see them all).

Count **copy units** of the assigned type: one per direct data member (base classes count as one
member), except that a run of adjacent bitfields sharing a storage unit counts as **one**.

| copy units | copy-assign `*d = *s` |
|---|---|
| **1** (one scalar, or one array member, or one `long long`) | **ALWAYS INLINE** |
| **2** | inline **iff both operands are at offset 0 of a pointer/local**; the moment either side needs an `add` (offset, `->member`, `[i]`, cast+K) → **OUT OF LINE** |
| **≥3** | **ALWAYS OUT OF LINE** — no addressing form, no context, no pragma helps |

Verified exhaustively: 16 104 struct shapes over 11 member kinds × arities 1–4, **0 counterexamples**
(`enum1.py` / `enum2.py`); 12 shapes × 10 addressing forms (`enum3.py`); 14 shapes × 6 copy forms
(`enum5.py`).

Two more things that do **not** work (proved, don't try them):

* **Writing your own `inline operator=` does not help.** Under `-inline noauto` mwcc emits it
  out-of-line and calls it anyway — byte-identical to the implicit one (`e5.cpp`).
* **Cross-function contamination inside one TU:** once *any* function in the file forces the
  helper out of line, *every later* copy of that class in the same file calls it instead of
  inlining. (Irrelevant while workers write one function per file — but do not "test" a form by
  adding it as a second function next to a failing one; you will get a false negative.)

### The tell
Your disasm has a `bl`/`bx ip` exactly where the target has `ldm`+`stm`, or wgate says
`SIZE/OVERGEN: 2 .text`. → you wrote a copy-assign of a ≥2-unit struct.

---

## 3. RULE C — 2-REGISTER `ldm`/`stm` = `long long`

A 2-word block copy **never** produces an ldm (RULE A). So when the target shows
`add rA,base,#K ; add rB,base,#J ; ldm rA,{rX,rX+1} ; stm rB,{rX,rX+1}` (both bases materialized,
two consecutive registers), the source is a **64-bit integer move**:

```c
*(long long*)(o + 0x84) = *(long long*)(o + 0x8c);
```

* Only for whole 64-bit **moves**. If either half is *consumed* (arithmetic, a call argument, a
  return value, a stored call result) mwcc emits two plain `ldr`/`str` instead.
* `double` does **not** do this under `-fp soft` — it falls back to ldr/str pairs.
* Works at offset 0 too (`ldm r1,{r2,r3}; stm r0,{r2,r3}`), and predicates cleanly.

---

## 4. RULE D — LONE `ldm` (merged loads)

Two separate `ldr`s are fused into one `ldm` by a **post-register-allocation peephole**. Conditions
(all required):

1. same base register, **consecutive ascending word** addresses;
2. first address is **base+0** (`ldmia`) or **base+4** (`ldmib`) — base+8 or more never merges;
3. destination registers **strictly ascending** in address order (they need not be consecutive:
   `{r1,r3}`, `{ip,lr}`, `{r4,r6}` all merge);
4. the loads are adjacent in the final schedule (no `bl` between).

Condition 3 is a register-allocation outcome, so it is only *indirectly* controllable — but there
are two forms where the ABI **forces** it and the ldm is guaranteed:

| target shape | write this |
|---|---|
| `mov rB,r0 ; ldm rB,{r0,r1,r2}` (a `mov` first, base **not** in the list) | pass the adjacent fields as **separate consecutive arguments**: `f(p->w0, p->w1, p->w2)` |
| `ldm r0,{r0,r1,r2}` (base **is** r0, no preceding `mov`) | pass the object **by value**: `f(*p)` |
| `ldmib rB,{r0,r1,r2}` | args come from `+4,+8,+0xc`: `f(p->w1,p->w2,p->w3)` |
| args in r1..r3 (`ldm rB,{r1,r2,r3}`) | `f(k, *p)` — struct by value as the 2nd argument |

That distinction (`mov` + base-outside-list vs base-inside-list) is the real content of the old
recipe #6, and it is the *opposite* way round from how #6 reads: **N scalar args also give an
`ldm`** — by-value only removes the `mov`.

If the target's ldm registers are not r0-based and none of the above applies, it is an allocator
accident: fix it with recipe #11 form changes, or SKIP. Decl-order permutation does nothing.

---

## 5. RULE E — LONE `stm` (merged stores)

Same peephole, mirrored, and **much more controllable** because you choose which value lands in
which field:

* offsets must be consecutive ascending words starting at **base+0** (`stmia`) or **base+4** (`stmib`);
* stored registers **distinct and ascending** with address;
* word stores only (`strh`/`strb` never merge);
* no `bl` between them.

Consequences worth memorizing:

* `f(S* p, int a, int b, int c) { p->w0=a; p->w1=b; p->w2=c; }` → `stm r0,{r1,r2,r3}` (args land
  in r1,r2,r3 in order — guaranteed).
* Same at `+4,+8,+0xc` → `stmib r0,{r1,r2,r3}`.
* **Statement order is irrelevant** — `p->w2=c; p->w1=b; p->w0=a;` still merges. What matters is
  the (register, address) correspondence: reverse *that* (`p->w2=a; p->w1=b; p->w0=c;`) and it
  does **not** merge.
* **Constants never merge**: `p->w0=0; p->w1=0; p->w2=0;` reuses one register → 3 plain `str`s.
  Likewise `p->w0=1; p->w1=2;` (each constant is re-materialized into the same register). If the
  target has an `stm` of a zeroed range, it is a *copy* from somewhere, not a constant fill.
* Storing a call's 64-bit result (`*(u64*)(o+0xa0) = f();`) is two `str`s, not an `stm`.

---

## 6. RULE F — clearing and filling

* `memset(p,0,N)` / `memcpy(d,s,N)` are **never** inlined — always a real `bl`. If the target has
  `bl memset` write `memset` (3 args); if it has inline ldm/stm it is *not* memset.
* An aggregate zero-initializer (`V3 t = {0,0,0};`, `int t[8] = {0};`) emits
  `add r0,sp,#K ; mov r1,#size ; bl __clear`. **`__clear` does not exist in this project's symbol
  table → instant `UNDEF-SYM`.** The ROM's helper is `func_0200f374`. Call it explicitly:

  ```c
  extern "C" void func_0200f374(void* dst, int size);
  Vec3 local;  func_0200f374(&local, 0xc);  *dst = local;   /* MATCHes ov004_0215377c, ov023_021f735c */
  ```
* A **non-zero** local aggregate initializer (`int t[3] = {0x32,0x33,-1};`) emits its image into
  the TU's data section and block-copies it to the stack:
  `ldr rX,<pool> ; add rY,sp,#0 ; ldm rX,{r0,r1,r2} ; stm rY,{r0,r1,r2}`. When the ROM's pool word
  points at a `data_ovNNN_xxxxxxxx` symbol, `extern` that symbol as the wrapper struct and assign
  it — the pool word is a reloc and is masked by the gate either way.

---

## 7. RECIPE — mechanical steps

**Step 1. Is it a compiler block move at all?**
Look at the register list. Compiler block copies transfer through `r0..r(N-1)`. `{r2,r3,ip}`,
`{r9,r10,r11,ip}`, `ldmia r0!,{r2,r3,ip}` + `add r0,r0,#4` chains → Nitro SDK hand asm (MTX/VEC
copy). **SKIP.**

**Step 2. Count the words** = size of the ldm register list (for N ≥ 5 read the whole expansion
against the RULE A table: `4! + 4!` → 8 words, `4! + {r0,r1}` → 6 words, `mov ip,#3` loop → 12+).

**Step 3. Pick the form from the word count.**

* **N == 2, both bases materialized** → `long long` (RULE C). Done.
* **N == 2, no `add`s / offsets folded** → not a block copy; it is a merged load/store (RULES D/E).
* **N >= 3** → block copy. Declare a **one-member, int-array** struct and copy it:

```c
struct Blk3_021e613c { int v[3]; };            /* exactly ONE member, element type int */
...
*(Blk3_021e613c*)(dst + 0x00) = *(Blk3_021e613c*)(src + 0x50);
```

  One member = 1 copy unit = **always inline**, at any offset, for any N (RULE B). This is the
  universal safe form. `unsigned int v[N]` is equally fine; `char`/`short` is **not**.

**Step 4. If you want readable named fields instead of `v[N]`**, you have exactly three
non-array options, in order of preference:

1. **copy-initialize** a fresh local: `Vec3 t = *s;` — always a block copy, any shape.
2. keep the named struct as a **member** of the object and copy the *member*, provided the member
   type itself is 1 unit.
3. give the parent ≤2 units **and** put both operands at offset 0 — rarely worth it.

**Step 5. Gate.** `SIZE/OVERGEN: 2 .text` ⇒ you took the copy-assign path on a ≥2-unit struct;
go back to step 3.

### Worked before/after

Target (`func_ov023_021e613c`):
```
mov r3, r0
add r0, r1, #0x50
ldmia r0, {r0, r1, r2}
stmia r3, {r0, r1, r2}
bx lr
```

WRONG (`SIZE/OVERGEN: 2 .text` — 3 members = 3 copy units):
```c
struct Vec3 { int x, y, z; };
ARM void f(Vec3* dst, void* src) { *dst = *(Vec3*)((char*)src + 0x50); }
```

RIGHT (`MATCH`):
```c
struct Vec3_021e613c { int v[3]; };
ARM void CopyVectorField_021e613c(Vec3_021e613c* dst, void* src) {
    *dst = *(Vec3_021e613c*)((char*)src + 0x50);
}
```

---

## 8. EVIDENCE

**Enumerations** (all in `SP/inv/ldmstm/`):
* `enum1.py`/`enum2.py` — 16 104 struct shapes (11 member kinds × arity 1–4), each compiled and
  its symbol table checked for `_ZN…aSERKS_`. Hypothesis "≥3 members ⇒ out-of-line": 42
  counterexamples, **all** of them runs of adjacent bitfields. Refined to "≥3 **copy units**
  (adjacent bitfields collapse)": **0 counterexamples / 16 104**.
* `enum3.py` — 12 shapes × 10 addressing forms, each in its **own TU**. Produced the 1 / 2 / ≥3
  unit table in RULE B, including the "2 units inline only at offset 0" boundary.
* `enum5.py` — 14 shapes × 6 copy forms; established that copy-init / by-value arg / by-value
  return are *always* whole-object block copies and never synthesize a helper.
* `a5.cpp` — the N = 1…64 expansion table (RULE A), read directly off the disassembly.
* `c1.cpp` — register pressure: 7 contexts (loop, across a call, extra live params, global source);
  transfer registers were `r0..r(N-1)` in **all** of them.
* `e2.cpp` — store-merge boundaries: offsets 0/4 merge, 8 does not; gaps split; descending
  register order blocks it; halfwords never merge; a `bl` between blocks it.
* `b1.cpp`/`b2.cpp` — load-merge boundaries, incl. all four argument-position shapes.
* `e1.cpp` — 12 forms of 64-bit access; `e4.cpp` — alignment-driven copy width.
* `f_1..f_4.cpp` — base classes count as **one copy unit each**: `B{int a,b}` + 1 member → 2 units
  → inline; +2 members → 3 units → out of line; two bases + 1 member → 3 units → out of line;
  base + `int[3]` → 2 units → inline.

**Real functions gated with `wgate.py` (`MATCH` = byte-exact).** Five were fresh, five reproduce
functions already committed to `src/` (independent confirmation that the rule generalizes):

| addr | ov | what it exercises | status |
|---|---|---|---|
| `021537c8` | ov006 | `stmib r0,{r1,r2,r3}` — RULE E | **MATCH** (fresh) |
| `021f735c` | ov023 | `__clear` → `func_0200f374` + 3-word copy — RULE F | **MATCH** (fresh) |
| `021e8748` | ov025 | 3-word copy into a stack local, then passed on | **MATCH** (fresh) |
| `021d9530` | ov028 | `ldm/stm {r2,r3}` = `long long` — RULE C | **MATCH** (fresh) |
| `021d97b0` | ov028 | `ldm r1,{r0,r1}` from a global into arg regs — RULE D | **MATCH** (fresh) |
| `021e613c` | ov023 | 3-word copy at `+0x50` | MATCH (reproduces committed) |
| `021f7174` | ov023 | 3-word copy at `+0x3c` | MATCH (reproduces committed) |
| `021f8cc0` | ov023 | 3-word copy at `+0x24` | MATCH (reproduces committed) |
| `0215377c` | ov004 | clear-helper + 3-word copy | MATCH (reproduces committed) |
| `02154984` | ov004 | pool-image → stack array + loop | MATCH (reproduces committed) |
| `022276cc` | ov031 | `ldm r0,{ip,lr}` merged loads | **partial** — ldm reproduced exactly; 6 bytes differ, all scratch-register numbering in the *unrelated* tail (recipe #11) |

Sources: `SP/inv/ldmstm/{s1,s2,s3,s5,s6,r1,r_*,r5,r6,s7}.cpp`.

Note on `02154984`: the committed version wraps the array in a second struct
(`struct Wrap { Inner mid; }; local.mid = global.mid;`). **The wrapper is unnecessary** — the bare
one-member struct gates identically (`r6.cpp` → MATCH). Recipe #6's wrapper is superstition; what
matters is *one member, array type*.

**Counterexamples sought and not found:**
* a ≥3-unit struct whose copy-**assign** inlines: none in 16 104 shapes × 10 addressing forms.
* a 2-word block copy that produces an `ldm`: none (that shape is always `long long`).
* a compiler-emitted block copy on transfer registers other than `r0..r(N-1)`: none in the lab;
  the only ROM instances (`{r2,r3,ip}`, `{r9,r10,r11,ip}`) are Nitro SDK hand asm.
* a pragma or flag that changes the out-of-lining: an explicitly-written `inline operator=` is
  still emitted out-of-line and called (`e5.cpp`).

---

## 9. WHEN TO SKIP — proved, do not grind

1. **`ldm`/`stm` on transfer registers that are not `r0..r(N-1)`** — e.g. `ldmia r0!,{r2,r3,ip}` +
   `add r0,r0,#4` (4×3 matrix copy), `{r9,r10,r11,ip}`, `{r2,r3,r4,r5,r6,r7,r8,ip}`. Nitro SDK
   hand asm (MTX_/VEC_/memcpy variants). No C form. SKIP on sight.
2. **A ≥3-unit struct copy-assign where the target genuinely calls a helper you cannot name.**
   If the target really does `bl <something>` for a copy, that callee is a real ROM function —
   find it in symbols.txt; do not let mwcc synthesize `_ZN…aSERKS_`, which will never resolve.
3. **`bl __clear` / `bl __copy` style aggregate-initializer helpers** — these mangled names do not
   exist here. Either call the ROM helper explicitly (`func_0200f374`) or restructure. If the
   target's helper address has no symbol at all, SKIP.
4. **Byte/halfword copy loops** (`ldrb/strb` or `ldrh/strh` + `subs/bne`) are *not* an ldm family
   member — they come from align-1/align-2 structs. Do not try to reach them with an int wrapper.
5. **Merged-load `ldm` whose register set you cannot reach** and which is not an argument-position
   or by-value case: this is recipe #11 (scratch registers). Decl-order permutation is a proven
   no-op; if a form change (CSE↔recompute, bitfield↔shift) does not move it, SKIP.
6. **`double` copies** — never produce ldm/stm under `-fp soft`; if the target has a 2-register
   move it is `long long`, not `double`.

---

## 10. SUGGESTED EDITS TO `worker_ov_all.md`

Replace the "Struct-copy over-generation (CRITICAL)" block and recipe #6 with:

> **#6 BLOCK MOVES / STRUCT COPY.** `ldm rS,{r0..}`+`stm rD,{r0..}` = a compiler block copy of
> N = (register-list length) words; read N ≥ 5 off the expansion table in
> `SP/inv/ldmstm_FINDINGS.md` §1. Write it as a **one-member int-array struct**:
> `struct Blk{int v[N];}; *(Blk*)(d+K) = *(Blk*)(s+J);` — one member ⇒ always inlined.
> `*d = *s` on a struct with **≥3 members** (or 2 members at a non-zero offset) synthesizes an
> out-of-line `_ZN…aSERKS_` ⇒ `SIZE/OVERGEN`. `T t = *s;` (copy-**init**), by-value args and
> by-value returns are always block copies and never over-generate.
> 2-register `ldm/stm` with materialized bases = **`long long`**, not a 2-word struct
> (a 2-word block copy emits plain ldr/ldr/str/str).
> Transfer registers are always `r0..r(N-1)`; any other set is Nitro hand asm → SKIP.
> `char`/`short` element types give byte/halfword loops, not ldm.
