# SOFT-FLOAT FINDINGS — `-O2 -fp soft -inline noauto -lang=c++`
> **Measured on mwccarm 2.0/sp1p5.** The ROM now builds with 2.0/sp2p2 and `-str pool,reuse`
> ([[dqix-sp2p2-rebase]]). The labs under `SP/lab/` and `SP/pad/` follow the build again, so a
> rule here is worth re-measuring before it is cited as settled.


## 1. VERDICT — **YES. Every soft-float construct in this ROM has a deterministic C form.**

Soft float is not a source of randomness. Which `bl _f*` appears is a pure function of the
**static C types**; which operand lands in **r0 vs r1** is a pure function of one integer
(a Sethi–Ullman register-need number) computed from the expression tree. Both are lookup tables.

Evidence: **512/512** exhaustive operand-pair cells fit the rule with **0 counterexamples**;
**ROM-wide census of 2 630 intrinsic call sites** matches the predicted constant-slot
distribution; **15/15 real unmatched ROM functions matched byte-exact** (`wgate` → `MATCH`),
14 of them on the first or second try, spread over ov000/ov017/ov024/ov028 and covering
add/sub/mul/div, both signed and unsigned conversions, all six compares, and double math.

The one function that resisted (`func_ov011_02184c7c`) failed on **argument-move ordering**,
which is not a float issue at all — see §9.

---

## 2. SCOPE (genuinely-unmatched functions only — raw `func_*` symbol still in symbols.txt)

| bucket | functions |
|---|---|
| unmatched functions in `build/usa/asm` | 6 622 |
| …touching float / int-div / double | **623** |
| …with a float arith/compare intrinsic | **429** |
| …float-only, no double helper needed (fully unblocked) | **406** |
| …needing a double helper (workable, see §7) | 47 |
| …integer-division only (`_s32_div_f` etc., no fp) | 170 |

Float-using function sizes: min 9 instrs, median 95, max 2 526. **107 are ≤40 instrs** — those
are near-mechanical with §5's recipe. Reproduce with `python SP/inv/softfloat/census2.py`.

---

## 3. THE INTRINSIC TABLE — `bl _fXXX` → the C that emits it

`float` = **one** register. `double` = an **aligned register pair** (`r0:r1`, then `r2:r3`).
Return: float in `r0`, double in `r0:r1`. All of these are ordinary AAPCS calls — they clobber
`r0-r3`/`ip`, so anything live across one lands in `r4+`.

### 3a. Arithmetic (all named, all usable)

| target | C | notes |
|---|---|---|
| `bl _fadd` | `a + b` | commutative → operand order by §4 |
| `bl _fsub` | `a - b`, and **`-a`** (`_fsub(0.0f, a)`, r0 = `mov r0,#0`) | slots = source order |
| `bl _fmul` | `a * b` | commutative → §4 |
| `bl _fdiv` | `a / b` | slots = source order |

### 3b. Conversions — **the intrinsic is chosen by the STATIC TYPE, signed vs unsigned**

| target | C | rule |
|---|---|---|
| `bl _fflt` | `(float)x` where `x` has a **signed** type | `int`,`short`,`signed char`, `int:N` bitfield |
| `bl _ffltu` | `(float)x` where `x` has an **unsigned** type | `unsigned`,`unsigned short`,`unsigned char`, `unsigned:N` bitfield |
| `bl _ffix` | `(int)f` / assignment to a **signed** integer | |
| `bl _ffixu` | `(unsigned)f` / assignment to an **unsigned** integer | |

**This is the `_ffltu`-vs-`_fflt` bug from the brief, and it is fully mechanical.**
mwcc does **not** apply C's integer promotion here: a `u8`/`u16` field converted to float gives
`_ffltu`, even though standard C says the operand is promoted to `int` first. Verified with real
`ldrb`/`ldrh` loads, not just parameters.

Both directions are steerable — this is the fix when the target disagrees with you:

```c
(float)p->u16field          -> _ffltu     // plain
(float)(int)p->u16field     -> _fflt      // force signed: inner cast wins
(float)(p->u16field + 0)    -> _fflt      // '+0' promotes to int
(float)(unsigned)p->s16field-> _ffltu     // force unsigned
p->u16field = (u16)f;       -> _ffixu     // the CONVERSION's target type decides…
p->u16field = (u16)(int)f;  -> _ffix      // …not the storage width
p->s16field = (short)(unsigned)f; -> _ffixu
```

### 3c. Comparisons — **six distinct intrinsics, NEVER canonicalized**

`a > b` emits `_fgr`. It is *not* rewritten to `_fls(b,a)`. So **you must write the comparison in
the direction the ROM used** — reading it off the disasm is the whole job.

| target | C | branch **over** the block | predicated set |
|---|---|---|---|
| `bl _fgr`  (`0200bfc4`) | `a >  b` | `bls` | `movhi` |
| `bl _fgeq` (`0200bf68`) | `a >= b` | `blo` | `movhs` |
| `bl _fls`  (`0200c088`) | `a <  b` | `bhs` | `movlo` |
| `bl _fleq` (`0200c020`) | `a <= b` | `bhi` | `movls` |
| `bl _feq`  (`0200c0e4`) | `a == b` | `bne` | `moveq` |
| `bl _fneq` (`0200c14c`) | `a != b` | `beq` | `movne` |

> **The delinked asm often prints these four as `bl func_0200bfc4` / `func_0200bf68` /
> `func_0200c0e4` / `func_0200c14c`.** They *are* `_fgr`/`_fgeq`/`_feq`/`_fneq`; write the C
> operator, do not `extern "C"` the raw name. (`_fls`/`_fleq` always print by name.)

Two extra facts, both load-bearing:

* **Negation flips the BRANCH, not the intrinsic.** `if (!(a<b))` → `bl _fls; blo`.
  So `bl _fls` followed by `blo` (instead of the usual `bhs`) means the source wrote `!(a<b)`.
* **Slot rule differs between ordered and equality compares:**
  * `<`, `<=`, `>`, `>=` → **strict source order**, always. `x < K` puts K in r1; `K < x` puts K in r0.
  * `==`, `!=` → **canonicalized by §4** (so `x == K` and `K == x` are the same code, K in r0).

### 3d. Integer division — 170 unmatched functions, second-biggest family

| target | C | notes |
|---|---|---|
| `bl _s32_div_f` | `a / b` (signed) | quotient r0, **remainder r1** |
| `bl _s32_div_f` + `mov r0,r1` | `a % b` (signed) | same call, take r1 |
| `bl _u32_div_f` (+`mov r0,r1`) | unsigned `/` and `%` | |
| `bl _ll_udiv` / `bl _ull_mod` | `u64 /` and `u64 %` | |

* Division by a **power of two** is strength-reduced, no call:
  signed `/4` → `asr r1,r0,#1; add r0,r0,r1,lsr#30; asr r0,r0,#2`; unsigned `/4` → `lsr r0,r0,#2`;
  unsigned `%4` → `and r0,r0,#3`. Any other constant → a real `bl` with the constant in r1.
* **mwcc does NOT share one call between `a/b` and `a%b`.** If the ROM has two `_s32_div_f`
  calls with the same operands, the source wrote `a/b` and `a%b` as separate expressions.
* `_ll_sdiv` / `_ll_mod` (signed 64-bit) are **not** in symbols.txt and do not appear in the
  remaining asm — if you somehow emit one, you wrote signed 64-bit `/` or `%` by mistake.
* Beware: `func_0200d958` / `func_0200d9e4` are **not** float — they are signed/unsigned
  varint (LEB-style) byte decoders. Don't route them here.

---

## 4. THE OPERAND-ORDER RULE — Sethi–Ullman (512/512 exhaustive, 0 counterexamples)

For a commutative float op (`+`, `*`, and `==`/`!=`) mwcc reorders the two operands. The rule is
a register-need number computed on the **source expression tree**:

```
su(float literal)                                    = 0
su(any lvalue read: param, local, global, field, [i]) = 1
su(unary: -x, (float)i, (int)f)                      = su(operand)          // 1 for a leaf
su(a ⊕ b)  = max(sa,sb)      if sa != sb
           = sa + 1          if sa == sb
su(call to a normal function f(...))                 = ∞  (beats any tree you will write)
```

Then, for `x = a ⊕ b` with `⊕` commutative:

> **The operand with the HIGHER su is evaluated FIRST and is passed in `r1`.
> The lower-su operand goes in `r0`. On a TIE, source order: left → `r0`, right → `r1`.**

For non-commutative `-`, `/` and the four ordered compares the **slots are fixed by source
order**, but the higher-su operand is still **evaluated first** (that is what decides which value
gets stashed in a callee-saved register and which gets a `mov r1,r0; mov r0,rX` pair).

### Reading it off a target (what a worker actually does)

| you see, just before the `bl` | what it means |
|---|---|
| `mov r0,#K` / `ldr r0,<pool>` and the value in r1 | the constant is the **lower-su** operand — normal `x = K*y` / `x = y*K`. Write it either way, it is the same code. |
| the value in r0 and `mov r1,#K` on a **commutative** op | **not** a plain assignment — it is `x op= K` (§5 lever A) |
| `mov r1,r0` then `mov r0,rN` after a `bl` | the call result (su ∞) was forced into r1 — the other operand is a leaf |
| the call result stays in r0, other operand `mov r1,rN` | the call result was bound to a **named local** first (§5 lever B) |

### Consequences you can verify in 10 seconds

```c
(s->a * s->b) + s->c   ->  _fmul(a,b) ; mov r1,r0 ; ldr r0,[..,#8] ; _fadd(c, prod)   // 0x20
float t = s->a*s->b; t + s->c
                       ->  _fmul(a,b) ; ldr r1,[..,#8]             ; _fadd(prod, c)   // 0x1c
```
Source order of a commutative op is **irrelevant** — `a+b` and `b+a` are byte-identical whenever
their su differs. Stop permuting it; change the su instead.

### ROM-wide confirmation (`SP/inv/softfloat/slots.py`, 2 630 call sites)

| intrinsic | const in r0 | const in r1 | reading |
|---|---|---|---|
| `_fmul` | 656 | 17 | 656 = `x = K*y` (su); the 17 = `x *= K` |
| `_fadd` | 72 | 22 | 72 = su form; 22 = `+=` |
| `_fdiv` | 7 | 336 | non-commutative, source order (`x / K` dominates) |
| `_fsub` | 16 | 37 | `K - x` vs `x - K` |
| `_fgr` / `_fgeq` | **0** / **0** | 66 / 20 | nobody ever wrote `K > x` |
| `_fls` / `_fleq` | 13 / 3 | 66 / 24 | both directions occur — read the target |
| `_feq` | 9 | **0** | canonicalized, as predicted |
| `_fneq` | 7 | 6 | the 6 are `if (x)` truthiness (§5 lever C) |

---

## 5. THE FIVE LEVERS (each verified, each changes bytes)

**A — compound assignment forces SOURCE ORDER (defeats su canonicalization).**
```c
p->f  = p->f + 1.0f;   ->  ldr r0,[r4] ... mov r1,r0 ; mov r0,#0x3f800000 ; bl _fadd   // K in r0
p->f += 1.0f;          ->  ldr r0,[r4]     ; mov r1,#0x3f800000            ; bl _fadd   // K in r1
```
Use `op=` when the target has the **value in r0 and the constant in r1** on `_fadd`/`_fmul`.
(For locals the two forms even differ in size: `t += 1.0f` is 0x10, `t = t + 1.0f` is 0x14.)

**B — a named local collapses su to 1, and pins the load above an intervening call.**
Two separate effects, both useful:
* *su collapse*: `float t = G(p); t + q;` → `_fadd(t, q)`, whereas `G(p) + q` → `_fadd(q, G(p))`.
* *load hoist*: `float r = o->rate;` emits the `ldr` **at the declaration**, above any later
  `bl`, and keeps it in a callee-saved register. Writing `o->rate` inline lets the load **sink
  below** the call into `r1`. This is what turned `func_ov028_021d942c` from 1-instruction-short
  into `MATCH`.

**C — truthiness vs explicit `!= 0.0f`.** Both emit `_fneq`; the slots differ.
```c
if (p->f)            ->  ldr r0,[..] ; mov r1,#0 ; bl _fneq      // value r0  (6 ROM sites)
if (p->f != 0.0f)    ->  ldr r1,[..] ; mov r0,#0 ; bl _fneq      // const r0
if (!p->f)           ->  same as the first, branch inverted
```

**D — signed/unsigned cast steering.** See §3b. Changes `_fflt`↔`_ffltu`, `_ffix`↔`_ffixu`.

**E — comparison direction.** See §3c. Write the operator the ROM used, in the ROM's operand
order. Flipping `3.14f < obj->angle` to `obj->angle > 3.14f` changed the function size (0x74 vs
0x78) — an instant fail.

---

## 6. TRAPS (each one silently produces the wrong code)

1. **A bare `0.5` is a `double`.** `x * 0.5` on a float emits `_f2d`, `_dmul`, `_d2f` — three
   calls instead of one, and two of the names don't exist (→ `UNDEF-SYM`). **Always write `f`.**
   The one exception: an *integer* literal is fine — `x / 2` and `x / 2.0f` are identical.
2. **mwcc applies non-IEEE-safe algebraic identities.** `x * 1.0f`, `x + 0.0f`, `x / 1.0f` all
   compile to **nothing**; `x * 0.0f` becomes `mov r0,#0`. If you need a `bl _fmul`, the constant
   must not be an identity.
3. **All-constant float expressions fold**, including `(float)5`, `(int)2.5f`, `1.0f/3.0f`, and
   `const float c = 2.5f`. Only mixed const/variable expressions emit a call.
   Round-trip casts are **not** folded: `(int)(float)i` really does emit `_fflt` then `_ffix`.
4. **A `float` passed to a varargs function promotes to `double`** → `bl _f2d`
   (= `func_0200c578`). That is the origin of most `func_0200c578` call sites (`sprintf("%f")`).
5. **Reading the constant:** a pool `.word 0xXXXXXXXX` next to a float op *is* the IEEE-754 bit
   pattern. Decode it, don't guess:
   `python -c "import struct;print(struct.unpack('<f',struct.pack('<I',0x3e4ccccd))[0])"` → `0.2`.
   Encodable-as-ARM-immediate constants appear as `mov r0,#0x3f000000` instead of a pool load;
   that happens automatically, you don't control it.
6. **`_ffix` truncates toward zero** (it is a C cast, not a round). If the ROM adds `0.5f` first,
   that `_fadd` is in the source.

---

## 7. DOUBLES — blocked implicitly, but fully unblocked via explicit calls

The compiler emits these names, and **they are not in `config/**/symbols.txt`**, so any real
`double` arithmetic in your C fails `wgate` with `UNDEF-SYM`:

> `_dadd _dsub _dmul _dfix _dfixu _dflt _dgr _dgeq _dls _dleq _deq _dneq _f2d`
> `_ll_sto_f _ll_uto_f _ll_sfrom_f _ll_ufrom_f _ll_sto_d _ll_sfrom_d _ll_ufrom_d _ll_uto_d`

**Available today:** `_fadd _fsub _fmul _fdiv _ffix _ffixu _fflt _ffltu _fgr _fgeq _fls _fleq
_feq _fneq _d2f _ddiv _dfltu _s32_div_f _u32_div_f _ll_udiv _ull_mod`.

### The workaround — call the routine by its committed `func_0200xxxx` name

Those symbols **do** exist, and an explicit call is **byte-identical** to the implicit form
(verified: `p->a + p->b` vs `func_0200ab28(p->a, p->b)` differ only in the masked reloc).
Precedent already in the tree: `src/Combat/Main/GetRandomUpTo02032370.cpp`.

> Because an explicit call fixes the slots, **choose the argument order to match the target's
> registers**: `func_0200b0f0(K, x)` puts K in `r0:r1`, `func_0200b0f0(x, K)` puts it in `r2:r3`.
> The implicit form applies §4 (const su 0 → `r0:r1`), so a literal usually goes **first**.

### Identification table (built from ROM disassembly + already-matched sources)

| addr | size | identity | confidence |
|---|---|---|---|
| `func_0200ab28` | 0x318 | **`_dadd`** | **confirmed** — mantissa combine is `adds/adcs`; mutual sign-flip branch with b608, exactly `_fadd`'s shape |
| `func_0200b608` | 0x3b4 | **`_dsub`** | **confirmed** — mantissa combine is `subs/sbcs`; has `_fsub`'s `eor …,#0x80000000` in the |a|<|b| path |
| `func_0200b0f0` | 0x364 | **`_dmul`** | **confirmed** — sign = xor of signs; used as a multiply in matched `GetRandomUpTo02032370` |
| `_ddiv` `0200d34c` | 0x544 | `_ddiv` | already named |
| `func_0200b074` | 0x40 | **`_dflt`** (int→double) | **confirmed** — matched src declares `double func_0200b074(int)`; sign-magnitude prologue; adjacent to `_dfltu` |
| `_dfltu` `0200b0b4` | 0x3c | `_dfltu` | already named |
| `func_0200af44` | 0x4c | **`_dfix`** (double→int) | **confirmed** — matched src declares `int func_0200af44(double)`; `rsbmi` negate |
| `func_0200af90` | 0x58 | `_dfixu` | strong — same body, no negate, negatives → 0 |
| `func_0200c578` | 0x84 | **`_f2d`** (float→double) | **confirmed** — exponent bias `+0x380` (127→1023); matched src `AnimateSubBG0Shake0208ca00.cpp` |
| `_d2f` `0200ae40` | 0x104 | `_d2f` | already named |
| `func_0200bbe0` | 0x98 | `_dgeq` | confirmed — `movhs r0,#1` |
| `func_0200bc78` | 0x98 | `_dgr` | confirmed — `movhi` |
| `func_0200bd10` | 0xa4 | `_dleq` | confirmed — `movls` |
| `func_0200bdb4` | 0x9c | `_dls` | confirmed — `movlo` |
| `func_0200be50` | 0x8c | `_deq` | confirmed — `moveq` |
| `func_0200bedc` | 0x8c | `_dneq` | confirmed — `movne` |
| `func_0200b454` | 0x19c | `sqrt(double)` | likely — halves the exponent (`asrs ip,ip,#1`) |
| `func_0200c700` | 0x6c | `_ll_sto_f` (s64→float) | strong — sign-handles then tails into `_fflt` |
| `func_0200c76c` | 0x68 | `_ll_uto_f` (u64→float) | strong — tails into `_ffltu` |
| `func_0200afe8` | 0x8c | `_ll_ufrom_d` (double→u64) | strong — 64-bit shift path, negatives → 0 |

Worked example — `func_ov024_021f729c`, matched byte-exact:

```c
extern "C" double func_0200b074(int v);              // _dflt
extern "C" double func_0200b0f0(double a, double b); // _dmul
extern "C" float  _d2f(double v);
...
obj->asFloat = (float)v;
obj->scaled  = _d2f(func_0200b0f0(0.85, func_0200b074(v)));   // 0.85 first -> r0:r1
```

**For the orchestrator:** adding those 20-odd names to `config/usa/arm9/symbols.txt` would let
workers write plain `double` arithmetic. Until then the explicit-call form above is exact and
needs no config change.

---

## 8. RECIPE (mechanical, follow in order)

1. **Read every `bl` in the target.** Map each through §3. Rewrite the four aliased compare
   addresses (`func_0200bfc4/bf68/c0e4/c14c`) to `_fgr/_fgeq/_feq/_fneq`.
2. **Decode every float pool `.word`** with the one-liner in §6.5. Write the literal with an
   `f` suffix. Never a bare `0.5`.
3. **Pick signedness from the intrinsic, not from the field name.**
   `_fflt`/`_ffix` ⇒ signed; `_ffltu`/`_ffixu` ⇒ unsigned. Declare the struct field to match;
   if you can't, force it with the cast forms in §3b.
4. **Write each comparison in the ROM's direction and operand order** (§3c). Check the
   branch-over condition code against the table — a `blo` after `_fls` means `!(a<b)`.
5. **Write the arithmetic naturally**, then compare operand registers against §4:
   * constant in r1 on `_fadd`/`_fmul` → use `op=` (lever A);
   * call result in r0 → bind it to a named local (lever B);
   * a load that the ROM does *before* an intervening `bl` → bind it to a named local
     declared at that point (lever B);
   * `_fneq` with the value in r0 → `if (x)`, not `if (x != 0.0f)` (lever C).
6. **Gate.** `python SP/wgate.py <OV> <addr> <file>`. Only `MATCH` is a pass.

### Before / after — the whole recipe on one real function (`func_ov000_02170460`, MATCH)

```
    ldrb r0, [r4, #0x24]        ldr r0, .L_021704d0   ; 0x3e4ccccd = 0.2f
    cmp r1, #0x0                bl _fmul              ; _fmul(0.2f, cvt)   const su0 -> r0
    movle r1, #0x1              mov r1, r0
    tst r0, #0x8                ldr r0, [r4, #0x40]   ; _fadd(field, product)  su 1 < 2
    beq .L_021704c4             bl _fadd
    mov r0, r1                  str r0, [r4, #0x40]
    bl _fflt                    mov r1, r0
                                ldr r0, .L_021704d4   ; 0x40490fda = pi
                                bl _fls               ; CONST IN r0 -> source is `K < x`
                                ldrlo r0, .L_021704d4
                                strlo r0, [r4, #0x40]
```
```c
ARM void AdvanceAngle_02170460(struct Spin_02170460* obj, int step) {
    if (obj->state < 0) return;
    if (step <= 0) step = 1;
    if (obj->flags & 8) {
        obj->angle = obj->angle + 0.2f * (float)step;   // 0.2f su0 -> r0; product su2 -> r1
        if (3.1415925f < obj->angle) obj->angle = 3.1415925f;   // K first — NOT `angle > K`
    } else {
        obj->angle = 0.0f;
    }
}
```
Negative controls proving each choice matters: writing `obj->angle > 3.1415925f` → size 0x74 vs
0x78; declaring the fields of `func_ov000_02155a04` `short` instead of `unsigned short` →
`BYTEDIFF` at 2 offsets; writing `0.75` instead of `0.75f` in `func_ov024_021f6bc0` → 0x5c vs 0x30.

---

## 9. WHEN TO SKIP

Only two things here are genuinely out of reach, and **neither is about float**:

* **Argument-move ordering before a multi-arg call.** `func_ov011_02184c7c` matches except that
  the ROM emits `mov r1,r4; mov r2,r0; mov r0,r5` where every C form I could write emits
  `mov r2,r0; mov r0,r5; mov r1,r4`. The order tracks the order in which the argument *values
  were defined*, which the target's fixed call order pins — 13 source variants (decl hoisting,
  naming the quotient, inlining the call, reordering locals) were all no-ops. If your only
  remaining diff is the *order* of `mov rN,rM` argument setup, that is this case → SKIP.
* Everything already on the `PROVEN: NO C FORM EXISTS` list in `worker_ov_all.md` — soft float
  does not change any of it.

**Do NOT skip** on any of these, they are all solvable with §5:
operand registers swapped · `_ffltu` where you wrote `_fflt` · an extra/missing `mov` around a
constant · a load appearing before instead of after a `bl` · a compare using the "wrong"
intrinsic · double arithmetic (§7).

---

## 10. VALIDATION LEDGER — 15 byte-exact matches produced by these rules

Scratch sources live in `SP/inv/softfloat/`; each was verified with `wgate` → `MATCH`.
They are integrable as-is (correct `// USA:` tag, `ARM` macro, no `0x` in any function name).

| ov | addr | file | what it exercises |
|---|---|---|---|
| 024 | `021d8bec` | `t1.cpp` | `_fflt`+`_fmul`+`_ffix`, const su0→r0, 6th stack arg |
| 024 | `021d8ab4` | `s1.cpp` | same idiom, 1.25f |
| 024 | `021d8cd4` | `s2.cpp` | same idiom, 0.5f (immediate, not pool) |
| 024 | `021d9204` | `s3.cpp` | same idiom, 1.5f |
| 024 | `021d8bc8` | `s4.cpp` | same idiom, 2.5f |
| 024 | `021f6bc0` | `t2.cpp` | `K * field` twice, const→r0 |
| 024 | `021f6d2c` | `t8.cpp` | same, guarded by `if (A() \|\| B())` |
| 000 | `02155a04` | `t3.cpp` | `_ffltu`, `_feq` vs 0, `_fdiv`, non-CSE'd conversion |
| 024 | `021db358` | `t3b.cpp` | identical twin of the above in another overlay |
| 000 | `02170460` | `t4.cpp` | `_fflt`/`_fmul`/`_fadd`/`_fls`, `K < x` direction, su ordering |
| 000 | `02170c7c` | `t6.cpp` | `_fleq` cascade, zero shared between result var and 0.0f |
| 017 | `021d6110` | `ta.cpp` | tagged-union accessor, `_fflt` after early return |
| 017 | `021d60f4` | `tb.cpp` | same shape, `_ffix` |
| 028 | `021d942c` | `tc2.cpp` | su **tie** → source order; load-hoist lever (B) |
| 024 | `021f729c` | `t5.cpp` | **double** via explicit `_dflt`/`_dmul`/`_d2f` calls |

Not matched: `ov011 02184c7c` (`t7.cpp`) — argument-move ordering, §9.

---

## 11. REPRODUCING

All under `SP/inv/softfloat/` (nothing was written into `src/`, no config touched):

| file | what it does |
|---|---|
| `c.sh <f.cpp>` | compile with the exact flags + full multi-section disasm with reloc names |
| `fdis.py <f.o> [name]` | the disassembler (`odis.py` shows only the first `.text`; this shows all, and resolves `bl`/pool relocs to symbol names) |
| `mat2.py [+ \| * \| - \| /]` | the 16×16 operand-order matrix + Sethi–Ullman prediction check |
| `census2.py` | unmatched-function census (the §2 table) |
| `slots.py` | ROM-wide constant-slot distribution (the §4 table) |
| `find.py <intrinsic> <r0\|r1> [n]` | list functions containing a given intrinsic/slot shape |
| `cands.py [n]` | smallest unmatched float-using functions, sorted by instruction count |
| `l1..l11.cpp` | the labs: intrinsic surface, literals/promotion, operand order, compares/conversions, compound assign, int division, folding, arg-move probing |

Note: `mat2.py`'s `-` / `/` mode reports evaluation order via a first-materialisation heuristic
that is confounded by prelude statements and constant rematerialisation — the non-commutative
conclusions in §4 come from hand-read cases (`l2/l4/l5.cpp`), not from that table.
