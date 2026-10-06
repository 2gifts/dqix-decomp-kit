# SCRATCH REGISTERS (r0–r3, ip) — plain -O2
> **Measured on mwccarm 2.0/sp1p5.** The ROM now builds with 2.0/sp2p2 and `-str pool,reuse`
> ([[dqix-sp2p2-rebase]]). The labs under `SP/lab/` and `SP/pad/` follow the build again, so a
> rule here is worth re-measuring before it is cited as settled.


Lab: `SP/inv/scratch/`. Compile+per-function disasm helper: `python SP/inv/scratch/d.py <f.cpp>`.

---

## 1. VERDICT — **PARTIAL, and better than expected**

Both reproducers were re-examined.

* **`0215dd3c` — SOLVED. `python SP/wgate.py 004 0215dd3c SP/inv/scratch/a2.cpp` → `MATCH`.**
  The worker's "unfixable via decl-order/expr-form" was correct *about decl order* and wrong about
  the function: three independent **form** levers were needed (block-copy form, CSE form, bitfield
  form). Each was proved necessary by single-lever ablation on the matching file.
* **`02159a2c` — NEGATIVE, 2 bytes.** `SP/inv/scratch/b1.cpp` reproduces the worker's exact
  `BYTEDIFF @0x9d,0xa6`. ~30 source forms tried; none moves it. Proof + the exact pattern to skip
  is in §6.

The headline: **declaration order is a NO-OP for straight-line scratch temps** (proved, §3) — that
is why workers grind on it forever. The scratch analogue of the callee-saved rule is not a decl-order
rule; it is a **form** rule. What moves a scratch register is changing the *shape* of the value:
compiler-block-copy vs hand loop, CSE vs recompute, bitfield vs shift.

---

## 2. THE RULE — how mwcc hands out r0–r3/ip

### 2a. Register order

Within a region, scratch registers are handed out **r0, r1, r2, r3, ip** in that order,
skipping any register pinned by something else (a live parameter, an ABI argument, a block-copy).

### 2b. Straight-line, call-free block, temps that die inside the block

> **Registers ascend r0,r1,r2,r3,ip in REVERSE order of the EMITTED definitions:
> the LAST def before the uses gets r0, the first def gets the highest.**

mwcc first **sinks every def as late as it legally can** (to just before its first use), *then*
allocates. So the source order you wrote is erased; only the order of *uses* survives.

Verified exhaustively (`s1.py`, `s2.py`, `s3.py`): n = 2,3,4,5 simultaneously-live temps × **every
permutation of use order** (2+6+24+120 = 152 cases), with r0 pinned and with r0 free. Zero
counterexamples. Staggered/nested live ranges (`s4.cpp`, k1–k4) also fit: colour in reverse def
order, each value taking the lowest register not held by an already-coloured *interfering* value.

```
n=5, r0 free, decl order v0..v4, v1 used first:
   ldr ip,[r4,#4](v0)  ldr r3,[r4,#0xc](v2)  ldr r2,[r4,#0x10](v3)  ldr r1,[r4,#8](v4->r1)  ldr r0..(v1)
   emitted defs:  v0    v2    v3    v4    v1
   registers:     ip    r3    r2    r1    r0      <-- last emitted def gets r0
```

### 2c. Loop-carried values held in scratch registers (loop body contains no `bl`)

> **Opposite direction: registers ascend from the LOWEST free scratch in SOURCE DECLARATION ORDER.
> Earlier declaration → LOWER register. Here decl order IS a lever.**

Verified `s7.py`: 3 loop-carried values (induction var, 2 base pointers), all **6/6** declaration
permutations, pool {r2,r3,ip}:

| decl order | regs |
|---|---|
| i,q,r | i=r2 q=r3 r=ip |
| i,r,q | i=r2 r=r3 q=ip |
| q,i,r | q=r2 i=r3 r=ip |
| q,r,i | q=r2 r=r3 i=ip |
| r,i,q | r=r2 i=r3 q=ip |
| r,q,i | r=r2 q=r3 i=ip |

Same direction as the callee-saved COMPUTED rule (`REGALLOC_FINDINGS.md`): earlier → lower.
Confirmed again on a real function (`gen9a2c.py` v01 vs v08: adding a bare `int i;` before
`Rec* rec = …` swapped `i`↔`rec` between r1 and r2 and changed nothing else).

### 2d. What has NO lever

**Declaration order of straight-line temps.** `s8.py`: n = 2,3,4 temps, **every** decl permutation
(2+6+24 = 32 cases) with use order held fixed → byte-identical output every time, all temps in r0.
The scheduler sinks each def to its use and the declarations vanish.

*This is the single most important negative in this report: a worker who sees two swapped scratch
temps and starts permuting declarations is wasting the fleet's time unless the values are
loop-carried (§2c).*

---

## 3. THE BLOCK-COPY CONVENTION (new, high value, mechanical)

An inline struct copy emitted by the **compiler** and a **hand-written** copy loop have *opposite,
fixed* register conventions. They are byte-distinguishable in the target and neither can be coaxed
into the other's shape. This alone was 4 of the bytes in `0215dd3c`.

### Target signature → what to write

| target disasm | it is | write |
|---|---|---|
| `ldr r3,=src` / `add r2,sp,#K` / `mov r1,#N` / `L: ldrb r0,[r3],#1 ; subs r1,r1,#1 ; strb r0,[r2],#1 ; bne L` | **compiler block copy**, align 1, N≥4 | `struct B{unsigned char b[N];}; B v; v = gSrc;` |
| `ldr r2,=src` / `add r3,sp,#K` / … `ldrb r0,[r2],#1 ; … ; strb r0,[r3],#1` (**src/dst swapped**) | **hand loop** | `unsigned char* s=…,*d=…; do{*d++=*s++;}while(--n);` |
| `mov r3,#N` / `ldrb r2,[r1],#1 ; subs r3,r3,#1 ; strb r2,[r0],#1 ; bne` (no pool load; r0=dst, r1=src are params) | block copy, both operands already in regs | `*d = *s;` |
| `ldr r3,=src` / `mov r2,#N` / `ldrb r1,[r0],#1 ; subs r2 ; strb r1,[r3],#1` | block copy **into a global** | `gDst = *s;` |

**Compiler block copy, by (size, alignment)** — sweep of 17 struct shapes (`c2.cpp`):

| shape | emitted |
|---|---|
| align-1, size 1 | `ldrb`/`strb`, no loop |
| align-1, size 2 or 3 | 2–3 straight `ldrb`/`strb` pairs, no loop |
| align-1, size ≥ 4 (4,5,8,12,16,20,32,64 all tested) | the byte loop above, `mov r1,#SIZE` |
| align-4, 2 words | two `ldr`/`str`, **high word first** |
| align-4, 3 words | `ldm rS,{r0,r1,r2}` / `stm rD,{…}` (rD=`r3`) |
| align-4, 4 words | `ldm rS,{r0-r3}` / `stm ip,{…}`, `mov r0,ip` |
| align-4, 5 words | `ldm r4!,{r0-r3}` / `mov ip,lr` / `stm lr!,{…}` / `ldr r1,[r4]` / `str r1,[lr]` |
| align-4, 8 words | two unrolled `ldm r4!`/`stm lr!` pairs |
| align-4, 16 words | loop: `mov ip,#4` / `ldm r4!,{r0-r3}` / `stm lr!,{…}` / `subs ip,ip,#1` / `bne` |

Sanity: the `mov rN,#K` immediate in a byte-loop copy is the **byte count**; in a word-loop copy it
is the **word count / 4**. `worker_ov_all.md`'s "never write `*dst=*src`" over-generation warning
does **not** apply to these — a whole-struct copy of a *small* struct (≤64 B tested) inlines into a
single `.text`; wgate confirms no OVERGEN.

---

## 4. THE TWO FORM LEVERS (verified by ablation on a byte-exact function)

### Lever B — shift-extract on a named int ⟷ real bitfield member  *(reliable)*

Reproduces in isolation (`lever.cpp`, `c1` vs `c2`) — the two registers swap, nothing else changes:

```c
int v = *(int*)(p+4);                      ldr r2,[r0,#4]      <- word in r2
if ((unsigned)(v<<2)>>31) return 0;        lsl r0,r2,#2        <- extract temp in r0
snk(((unsigned)(v<<4)>>31)?1:0, …);

struct BF{unsigned lo:27,b27:1,b28:1,b29:1,hi:2;};
struct NB{unsigned w0; BF f;};
if (((NB*)p)->f.b29) return 0;             ldr r0,[r0,#4]      <- word in r0   (SWAPPED)
snk(((NB*)p)->f.b27?1:0, …);               lsl r2,r0,#2        <- extract temp in r2
```

Bit numbering: **mwcc allocates bitfields from the LSB up.** `mov rX,v,lsl #k ; movs rX,rX,lsr #31`
extracts bit `31-k`, so that bit is the `(32-k)`-th declared 1-bit member. Pad with an explicit
low field: bit 29 ⇒ `unsigned lo:27; unsigned b27:1; unsigned b28:1; unsigned b29:1; unsigned hi:2;`.

**Bonus — this also explains "N bitfield reads but only N-1 loads".** mwcc does *not* fully CSE
bitfield loads: 3 bitfield reads of the same word emit **2** `ldr`s (`lever.cpp`, `d2`). If the
target shows a redundant re-load of the same field, that is the bitfield signature — **do not**
reach for a `volatile` cast to force it (the previous worker did); real bitfields give it for free.
Removing the `volatile` from the matching `a2.cpp` still gates `MATCH`.

### Lever A — named local (CSE) ⟷ address expression written inline at both sites  *(context-dependent)*

Same instructions either way; sometimes it flips the scratch pair. **It is a no-op in isolation**
(`lever.cpp` a1≡a2, b1≡b2) but it was worth 27 bytes on the real `0215dd3c`. Treat it as a cheap
thing to try, not a rule.

```c
unsigned char* arr = *(unsigned char**)(o+0x150);          // named  -> arr=r0, arr+0x194=r1
char* e = (char*)(arr+0x194) + i*0x20;  …  arr[0x49c]

char* e = (char*)(*(unsigned char**)(o+0x150) + 0x194) + i*0x20;   // inline -> arr=r1, temp=r0
…  (*(unsigned char**)(o+0x150))[0x49c]                            // (mwcc still CSEs to ONE ldr)
```

### Ablation proof (each lever removed from the byte-exact `a2.cpp`, one at a time)

| variant | wgate |
|---|---|
| `a2.cpp` as written | **MATCH** |
| block copy → hand loop | BYTEDIFF 4 @0x39,0x3d,0x4a,0x52 (exactly the 4 copy instrs) |
| inline recompute → named `arr` local | BYTEDIFF 27 |
| bitfields → shifts on a named int | BYTEDIFF 6 |

---

## 5. RECIPE (mechanical)

1. **Classify the mismatching values from the TARGET disasm.** For each scratch register, find its
   defining instruction and its last use.
2. **Is it a copy loop?** (`ldrb rX,[rS],#1 ; subs ; strb rX,[rD],#1 ; bne`, or `ldm rS!/stm rD!`.)
   → §3 table. `src` in the **higher** register (r3) ⇒ compiler block copy ⇒ write a struct
   assignment. `src` in the **lower** register (r2) ⇒ hand loop. Nothing else changes this.
3. **Is one value a word that gets `lsl #k ; lsr #31`-ed?** → Lever B. Toggle between
   `int v = *(int*)(p+K);` + shifts, and a real bitfield struct member + `?1:0`.
   Layout the bitfield from the LSB (bit `31-k` = the `(32-k)`-th 1-bit member).
4. **Are the two values live across the loop back-edge, in a loop with no `bl`?** → §2c:
   **swap their declarations; earlier decl = lower register.** Codegen-neutral.
   (Adding a bare `int i;` earlier is enough to move an induction variable down a register.)
5. **Are they straight-line temps inside one block?** → **do NOT touch declaration order** (§2d).
   Your only levers are (a) changing which value is *used first* — the value used first gets the
   lowest register, because its def is sunk last; (b) Lever B; (c) Lever A (try once, cheap).
6. Re-gate after each single change. Ablate: change one lever at a time so you learn which one moved.

### Worked before/after (`0215dd3c`, 37 bytes → MATCH)

```c
/* BEFORE — 3 wrong forms */                    /* AFTER — SP/inv/scratch/a2.cpp, MATCH */
unsigned char buf[8];                           struct Bytes8 { unsigned char b[8]; };
{ int n=8; unsigned char* s=data;               extern Bytes8 data_ov004_0216fb20;
  unsigned char* d=buf;                         Bytes8 buf;
  do{*d++=*s++;}while(--n); }                   buf = data_ov004_0216fb20;

unsigned char* arr =                            char* entry = (char*)(*(unsigned char**)
   *(unsigned char**)(comb+0x150);                  ((char*)combatant+0x150) + 0x194) + byte*0x20;
char* entry=(char*)(arr+0x194)+byte*0x20;       …
… ((FlagByte*)(arr+0x49c))->bit0                … ((FlagByte*)(*(unsigned char**)
                                                     ((char*)combatant+0x150) + 0x49c))->bit0

int field4 = *(int*)(p2+4);                     struct Bits{unsigned lo:27,b27:1,b28:1,b29:1,hi:2;};
if (!((unsigned)(field4<<2)>>31)) {             struct Node{unsigned w0; Bits f;};
  int bit27=((unsigned)(field4<<4)>>31)?1:0;    if (!((Node*)p2)->f.b29) {
  flags[1]=((unsigned)(*(volatile int*)          int bit27 = ((Node*)p2)->f.b27 ? 1 : 0;
      (p2+4)<<3)>>31)?1:0;                       flags[1] = ((Node*)p2)->f.b28 ? 1 : 0;
```

---

## 6. WHEN TO SKIP — proved to have no C form

### 6a. Declaration reordering of straight-line scratch temps
32/32 permutations byte-identical (§2d). If the values are not loop-carried, stop.

### 6b. A dead-after-one-use pool-address temp that the ROM parks in a **callee-saved** register
The `02159a2c` pattern. ROM:

```
ldr  r5, =data_ov004_021707d8    <- callee-saved reg for a temp that dies 2 instrs later
mov  r1, #0
ldr  r2, [r5, #8]
ldrsh r5, [r2, #0x3a]            <- r5 immediately reused for the loop bound
```

Every C form puts that temp in **r0** (r0–r3 are provably free there). `b1.cpp` is byte-exact except
those 2 bytes (`0x9d` = the `ldr` Rd nibble, `0xa6` = the following `ldr` Rn nibble).

Ruled out — **~30 forms, all identical `BYTEDIFF @0x9d,0xa6`** (`sweep9a2c.py` + follow-ups):
* address expression: `g.rec`, `(&g)->rec`, `*(Rec**)((char*)&g+8)`, `((Rec**)&g)[2]`,
  named `Glob* g`, named `Rec* rec`, `volatile` global, pointer-walk in the loop body
* `n` as `int` / `short` / `register`, `n` re-read in the loop condition, extra dead temps
* loop form: `for` / `while` / `i != n`, `int i;` hoisted, `k` hoisted
* struct-copy form: assignment vs copy-init, `Blk4` vs `int[4]` + cast, copy moved after
* declaration placement of `buf` / `n` / `k` at block vs function scope, `ok` folded into the `if`
* hoisting the address/`rec`/`n` out of the enclosing `if` (all cost instructions: 43–114 bytes off)

**Rule for workers: if the target holds a short-lived address temp in r4–r11 while r0–r3 are
demonstrably free at that point, and the register is re-defined within ~2 instructions, SKIP.**
Do not grind — it is the same class of unreachable allocation as
`REGALLOC_FINDINGS.md`'s mixed CALL/COMPUTED band.

---

## 7. WHAT WAS RULED OUT / METHOD NOTES

* A naive predictor (interference computed from the *source* statement timeline + reverse-def
  colouring) scores only 78/122 on 150 randomised straight-line functions (`hunt.py`). The 44
  misses are all *over*-estimated interference: mwcc sinks defs to their uses, so source-level
  live ranges are wrong. Predict from the **emitted** order, never from the source order. This is
  the same fact as §2d and is why the decl-order intuition from the callee-saved rule misfires here.
* `register`, `const`, type width changes (int/short/unsigned/pointer), `?:` vs if/else, extra
  temps, and expression re-association were all no-ops on every scratch pair tested.
* ABI argument registers at call sites are forced and were never in question.

## 8. FILES

`SP/inv/scratch/` — `d.py` (compile + per-symbol disasm), `lab.py` (variant harness),
`s1–s4,s7–s9` (ordering labs), `c1.cpp`/`c2.cpp` (block-copy sweep), `lever.cpp` (lever isolation),
`hunt.py`/`swap.py` (randomised), `gendd3c.py`/`gendd3c2.py`/`gen9a2c.py`/`sweep9a2c.py` (real-function sweeps),
**`a2.cpp` (0215dd3c — MATCH)**, `b1.cpp` (02159a2c — 2 bytes, the proved negative).
