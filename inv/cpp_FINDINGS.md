# C++ CLASS / CTOR / DTOR / VTABLE MODELING — plain -O2
> **Measured on mwccarm 2.0/sp1p5.** The ROM now builds with 2.0/sp2p2 and `-str pool,reuse`
> ([[dqix-sp2p2-rebase]]). The labs under `SP/lab/` and `SP/pad/` follow the build again, so a
> rule here is worth re-measuring before it is cited as settled.


Investigator `cpp`. Lab files: `SP/inv/cpp/`. Six fresh `MATCH`es produced while deriving this
(list at the bottom). Nothing here is theory — every rule below was gated.

---

## 1. VERDICT

**PARTIAL — but the partition is clean and mechanical.**

| disasm shape | is a real C++ class needed? | verdict |
|---|---|---|
| `bl <ctor/dtor>` — direct call, `this` in r0 | **NO. Never.** Explicit call is byte-identical. | **YES, lever, fully solved** |
| function that returns its own r0 argument (`mov r4,r0` … `mov r0,r4`) | **NO** — plain `T* F(T* obj){…; return obj;}` | **YES, fully solved** |
| `bl _ZN<Class><Method>E<args>` — direct method call | **NO** — `extern "C"` the literal mangled name | **YES, fully solved** |
| `ldr rX,[r0]; ldr rY,[rX,#N]; blx rY` — vtable dispatch | **YES, always** — class w/ N/4 dummy virtuals | **YES, fully solved** |
| pointer-to-member dispatch (2-word table entries) | **YES** — `T::*` typedef | already committed precedent |
| `mov r4,#0; bl <dtor>; mov r0,r4` (c30c's 8 missing bytes) | **irrelevant — a class does NOT produce it** | **NO. Proven negative. SKIP.** |

The reproducer's diagnosis was **wrong**. `0215c30c` is not blocked on "model the callees as a real
class with mangled ctor/dtor". A real class and explicit calls emit **the same bytes** (proof §3.1),
and the real class can never link (§3.2). c30c is blocked on a rematerialization quirk that occurs
**4 times in the entire ROM** and is not a class phenomenon at all (§5).

### Why "declare a real ctor/dtor" is not just unnecessary but *impossible*

* mwcc uses **Itanium / ARM-EABI mangling**, not CodeWarrior `__ct`/`__dt`. `class Widget{Widget();~Widget();}`
  emits `bl _ZN6WidgetC1Ev` / `bl _ZN6WidgetD1Ev` (verified, `SP/inv/cpp/m1.cpp` symtab).
* `grep -rho "_ZN[A-Za-z0-9_]*[CD][12]Ev" config/` → **0 hits.** `_ZTV*` → **0 hits.** `__ct`/`__dt` → **0 hits.**
  No ctor, dtor or vtable symbol exists anywhere in the project's symbol maps.
* So any declared ctor/dtor is an instant `UNDEF-SYM`, and any class with virtuals **plus a defined
  ctor** additionally references `_ZTV<Class>` → also `UNDEF-SYM` (verified: `og4.o` undef =
  `['S','_ZN1P1MEv','_ZTV1P']`).

**Corollary the fleet should internalise: never write `Ctor()`/`~Dtor()` in a class declaration.
Ever. Call the function explicitly instead — it costs nothing.**

---

## 2. THE RULE

### 2A. Decision list — read the target disasm, pick a row

| you see in the target | write this | never write this |
|---|---|---|
| `add r0,sp,#K` then `bl func_ovNNN_xxxx`, later the same pair again near the end | `Local v; func_ovNNN_xxxx(&v); … func_ovNNN_yyyy(&v);` — plain calls on a **plain struct** | `struct L{ L(); ~L(); };` |
| `mov rN,r0` at top, `mov r0,rN` right before the return, no other use of rN as a result | `ARM T* Name_addr(T* obj){ …; return obj; }` (ARM-EABI ctor/dtor return `this`) | `void` return |
| …and additionally `ldr r1,<pool>; str r1,[rN,#0]` | first statement is `obj->vtable = &data_ovNNN_xxxxxxxx;` | anything class-y |
| `bl _ZN…` / `bl _ZNK…` with `this` in r0 | `extern "C" <ret> _ZN…(void* self, <args>);` — copy the string **verbatim** from symbols.txt | re-deriving the mangling from a natural decl |
| `mov r0,<obj>` then `ldr rX,[r0]; ldr rY,[rX,#N]; blx rY` | class with **N/4** dummy `virtual void vNN();` then the real one | function-pointer struct / raw cast |
| `add r0,sp,#K` (hidden buffer) then `mov r1,<obj>`, chase through r1 | `virtual S M();` + **`const S& v = o->M();`** | `*out = o->M();` (emits `operator=` → OVERGEN) |

### 2B. Vtable index arithmetic (verified)

```
method at vtable byte offset N  →  declare exactly N/4 dummy virtuals before it
```
* Base-class virtuals are counted **first**, then derived (`Base{b0,b1}` + `Der{d0,M}` → `M` at 0xc). ✔
* The vptr is **always at object offset 0**, even if data members are declared before the virtuals;
  the first data member then sits at +4. ✔
* Declaring virtuals is **free**: no `.data`, no `_ZTV`, no second `.text` — as long as you define
  **no** member function. ✔ (`vt2.o` undef list contains only the three real callees.)
* Count in the source, not in your head: emit them 4-per-line `virtual void v00(); … virtual void v57();`.

### 2C. OVERGEN table — what may and may not be *defined* in a class

| you write | extra `.text` emitted? | verdict |
|---|---|---|
| any member **declared only** (incl. `virtual`, ctor, dtor) | no | safe (but ctor/dtor → UNDEF-SYM) |
| non-virtual member **defined** in-class (`void Set(int v){a=v;}`) | no — fully inlined | safe |
| **constructor** defined in-class | no (inlined)… | …but pulls in `_ZTV` if the class has virtuals → UNDEF-SYM |
| **destructor** defined in-class | **YES — out-of-line `_ZN…D1Ev`** | **`SIZE/OVERGEN`. Never do this.** |
| `*dst = obj->VirtualReturningStruct();` | **YES — out-of-line `_ZN…aSERKS_`** | **OVERGEN. Use `const S&`.** |

---

## 3. EVIDENCE

### 3.1 A real class and explicit calls are byte-identical (the central result)

`SP/inv/cpp/v1.cpp` — same CFG twice, once with `Blob b; Ct(&b); … Dt(&b);` (explicit) and once with
`class Wid{Wid();~Wid();}; Wid w;` (real ctor/dtor). Output: **identical instruction-for-instruction**,
including the two dtor sites and all four `return 0`s. Repeated in `v5.cpp` (`u1` vs `u2`) with a
different return shape — identical again. **2/2 shapes, 0 counterexamples.**

Same for methods: `mm.cpp` compiles `a->Allocate(n)` through the real `SafeAllocator` class and
`_ZN13SafeAllocator8AllocateEj(a,n)` through an `extern "C"` literal-mangled decl — identical bytes,
identical undef symbol. So the class buys nothing there either.

### 3.2 …and the class cannot link
See §1. Zero `C1Ev`/`D1Ev`/`_ZTV` symbols project-wide.

### 3.3 A class IS required for vtable dispatch — with a measured counterexample

`func_ov004_02156fd4` (ov004), same source, two dispatch spellings:

| form | result |
|---|---|
| `class VNode{58 dummies; virtual int MethodE8();}; node->MethodE8()` | **MATCH** |
| `(*(int(**)(void*))(*(char**)node+0xe8))(node)` | **BYTEDIFF @0x28–0x2f** |

The 8 bytes are an ordering difference, and it is a reliable fingerprint:

```
ROM / class form :  mov r0, r4      ldr r1,[r0]    ldr r1,[r1,#0xe8]   blx r1
raw-cast form    :  ldr r1,[r4]     mov r0, r4     ldr r1,[r1,#0xe8]   blx r1
```
A real virtual call materialises `this` into **r0 first** and chases the vtable **through r0**; a
function-pointer cast chases from the original register and moves `this` later.

**ROM-wide census of the chase order (14 783 functions scanned):**
```
chase through r0 right after `mov r0,<obj>`   (virtual-call form)  : 39
chase through <obj>, `mov r0,<obj>` afterwards (fn-ptr form)       :  0
obj already in r0 as the incoming param (both forms coincide)      : 54
```
**Zero** function-pointer-form sites in the whole game. The ROM was compiled from real virtual calls
everywhere → **always use the class; never the raw cast.** (In simple no-pressure thunks the two
coincide — `func_ov016_0218f6a0` MATCHes either way — so the class form is never worse.)

### 3.4 Family sizes (why this is worth a recipe)

Scan of all 14 783 disassembled functions, of which 7 864 are still `func_*` (undecompiled):

| family | still undecompiled |
|---|---|
| functions containing a vtable dispatch | **62** |
| "returns-this" ctor/dtor-shaped functions | **48** (89 already committed with names like `InitAndReturnSelf_*`, `ClearAndReturnSelf_*`, `ConstructStructArray_*`) |
| functions that `bl` a mangled C++ method `_ZN…` | **798** |

### 3.5 Counterexamples sought and not found
* No source form of a declared-only ctor/dtor differs from an explicit call (2 CFG shapes × 2 return shapes).
* No fn-ptr-order vtable chase anywhere in the ROM (0/14 783).
* No `-lang`, decl-order, or cast variation changed the vtable index rule (5 layouts: flat/inherited/
  members-first/sret/const — all N/4, vptr at 0).

---

## 4. RECIPE

### 4A. Object-lifetime call site (stack object with ctor + dtor)

**Recognise:** `add r0,sp,#K; bl F` early, and `add r0,sp,#K; bl G` on every exit path after it,
where `F`/`G` return their argument.

1. Declare the local as a **plain struct sized from the disasm** (the `sub sp,sp,#S` and any
   `memcpy(...,#len)` give you the size).
2. `extern "C"` both callees with `T*` in and `T*` out.
3. Call them explicitly, one per exit path, exactly where the target's `bl`s are.
4. **Do not** write `T(); ~T();`.

```c
/* WRONG — UNDEF-SYM _ZN17SceneNode_0215c30cC1Ev / D1Ev */
struct SceneNode_0215c30c { unsigned char raw[0xac]; SceneNode_0215c30c(); ~SceneNode_0215c30c(); };
SceneNode_0215c30c local;                 /* implicit ctor */
if (!Init(block)) return 0;               /* implicit dtor */

/* RIGHT — identical codegen, resolvable symbols */
struct SceneNode_0215c30c { unsigned char raw[0xac]; };
extern "C" SceneNode_0215c30c* func_ov004_0215c448(SceneNode_0215c30c*);
extern "C" SceneNode_0215c30c* func_ov004_0215c474(SceneNode_0215c30c*);

SceneNode_0215c30c local;
func_ov004_0215c448(&local);
memcpy(block, &local, 0xac);
if (!Init(block)) { func_ov004_0215c474(&local); return 0; }
...
func_ov004_0215c474(&local);
```

### 4B. The ctor / dtor bodies themselves ("returns-this" family, 48 left)

**Recognise:** `mov rN,r0` in the first 2 instructions + `mov r0,rN` immediately before the return.
That is the ARM-EABI rule that ctors and dtors return `this`. It is **not** a register-allocation
accident — write `return obj;`.

```c
// USA: func_ov004_0215c448          <- ctor: note the vtable store first
ARM SceneNode_0215c448* ConstructSceneNode_0215c448(SceneNode_0215c448* obj) {
    obj->vtable = &data_ov023_021fe3e4;                              /* str r1,[r4,#0] */
    ClearNameTable((NameTable02048080*)((char*)obj + 0x34));
    func_0204719c((char*)obj + 0x20);
    return obj;                                                      /* mov r0,r4 */
}
// USA: func_ov004_0215c474          <- dtor: same members, reverse order, no vtable store
ARM SceneNode_0215c474* DestroySceneNode_0215c474(SceneNode_0215c474* obj) {
    MaybeInvoke0204719c((Struct02047230*)((char*)obj + 0x20));
    ClearNameTable((NameTable02048080*)((char*)obj + 0x34));
    return obj;
}
```
The vtable pointer is just `struct T { void* vtable; };` + a store of `&data_ovNNN_xxxxxxxx`.
Both gated **MATCH** first try. A third (`func_ov034_022a29e4`, different overlay, same shape) also
MATCHed first try.

### 4C. Calling a decompiled C++ function or method

**Copy the mangled string verbatim out of `symbols.txt` and `extern "C"` it, with `this` as an
explicit first parameter.** Do not try to re-derive the mangling from a natural declaration: the
symbol is a function of the *exact* parameter types, so one wrong type silently produces a different
symbol (`ClearNameTable(void*)` → `_Z14ClearNameTablePv`, not `…P17NameTable02048080` → `UNDEF-SYM`).

```c
extern "C" void* _ZN13SafeAllocator8AllocateEj(void* self, unsigned int len);
extern "C" unsigned int _ZNK13SafeAllocator30GetSizeWithLargestBlockRemovedEv(const void* self);
...
_ZNK13SafeAllocator30GetSizeWithLargestBlockRemovedEv((char*)owner + 4);
void* block = _ZN13SafeAllocator8AllocateEj((char*)owner + 4, 0xac);
```
(Committed precedent: `src/Combat/Overlay_11/SetAllocFields_021844a4.cpp`.) A natural declaration
that happens to mangle correctly is equally fine — `mm.cpp` proves the two are byte-identical — so
prefer natural decls when the demangling is unambiguous (`_Z25RestorePairTables0207df90Pc` →
`void RestorePairTables0207df90(char*);`) and the literal form when it is not.

### 4D. Virtual dispatch (62 left)

**Recognise:** `ldr rX,[r0]` (or `[r0,#0]`) → `ldr rY,[rX,#N]` → `blx rY`.

1. `N/4` = the method's index. Emit that many `virtual void vNN();` declarations, then the real one.
2. Give the real one the arity you read off the `mov r1/r2/r3` setup, and the return type from
   whether r0 is consumed afterwards.
3. Extra data members go after the virtuals; they start at object offset +4.
4. Define **nothing**.

```c
// vtable byte offset 0xe8 -> index 58
class VNode_02156fd4 {
public:
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    /* … v04 … v57 … */
    virtual int MethodE8();
};

// USA: func_ov004_02156fd4
ARM int CallVTableFnAtE8IfType17_02156fd4(void* ctx, int id) {
    VNode_02156fd4* node = func_ov023_021f6880(func_ov011_021849c8(ctx), id);
    if (node != 0) {
        if (ScaleStatsIfType12_021f6f10(node) == 0x11) {
            return node->MethodE8();
        }
    }
    return 0;
}
```
Note the nested-if funnel — the flat `if(!node) return 0; if(x!=0x11) return 0;` form predicated
(`moveq/popeq`) and BYTEDIFFed. Recipe #3 still applies on top of the class recipe; the class fixes
the dispatch, the funnel fixes the epilogue.

### 4E. Virtual returning a struct (sret)

`add r0,sp,#K` (hidden buffer) + `this` demoted to r1 + chase through r1.
```c
const S3& v = o->Get();      /* RIGHT — single .text            */
Sink(&v);
/*  *out = o->Get();            WRONG — emits _ZN2S3aSERKS_ → OVERGEN */
```

---

## 5. WHEN TO SKIP

### 5.1 `0215c30c` — SKIP. Proven, not guessed.

Everything except **8 bytes** reproduces from plain C++ (`SP/inv/cpp/c1.cpp`: `SIZE 0x134 vs 0x13c`,
structure otherwise identical). The 8 bytes are two `mov r4,#0` staged before the two dtor calls:
```
ROM   :  add r0,sp,#0xc ;  mov r4,#0 ;  bl dtor ;  mov r0,r4
mwcc  :  add r0,sp,#0xc ;               bl dtor ;  mov r0,#0
```
Ruled out (each compiled and disassembled):

* real class with ctor/dtor — **identical to explicit calls**, does not stage (`v1.cpp`, `v5.cpp`);
* `return callResult` where the result is provably 0 on that path (`v2.cpp w1`) → `movs r4,r0`, wrong instruction;
* result var assigned 0 in both arms (`w2`), declared before the object (`w4`), `do{}while(0)`
  second def (`v4.cpp t4`), per-branch merges (`t2`), late merge (`t3`), address-of-local merge (`t1`),
  one-variable-reused-as-call-result-then-0 (`v3.cpp s4/s5`), inlined `Zero()` returned across a real
  dtor (`v5.cpp u1/u2`), nested second object (`u4`) — **all** emit `mov r0,#0` after the call.
* The only form that stages at all (recipe 9b's merge, `v3.cpp s1` / `v2.cpp w5`) hoists **one**
  `mov rN,#0` to a single dominator and adds a stray `cmp` — it fixes the *size* (`c2a.cpp`,
  `c2b.cpp` both reach `0x13c`) but not the bytes (143 differ, mostly register permutation).

**And the pattern is not a family:** `mov rN,#0 ; bl … ; mov r0,rN` occurs **4 times in the entire
ROM** — two of them inside `0215c30c` itself, the other two (`func_0207a734`, `func_020a75ec`) are
multi-use zeroes, a different situation. There is nothing to unlock. Do not spend fleet time on it.

### 5.2 Other stop signs

* **Any function whose target `bl`s a ctor/dtor that is still `func_ovNNN_xxxx`** — that is *not* a
  blocker. Call it explicitly (§4A). Do not wait for it to be "decompiled as a real class"; there is
  no such thing in this project.
* **A class you were about to give a defined destructor** — stop, you will get `SIZE/OVERGEN` (§2C).
* **`*dst = obj->Method()` for a struct-returning method** — same, use `const S&`.
* **Multiple inheritance / this-adjusting thunks** — not observed; single inheritance puts the base
  at offset 0 with no adjustment (`og7.cpp`), so nothing special is needed.

---

## FRESH MATCHES PRODUCED (all via `wgate.py`, all first- or second-try)

| addr | ov | file | what it proves |
|---|---|---|---|
| `0215c448` | 004 | `SP/inv/cpp/ct.cpp` | ctor body + vtable-ptr store, no class |
| `0215c474` | 004 | `SP/inv/cpp/dt.cpp` | dtor body, returns `this` |
| `022a29e4` | 034 | `SP/inv/cpp/ct2.cpp` | same ctor recipe, different overlay |
| `0218e790` | 016 | `SP/inv/cpp/ct3.cpp` | bare returns-this init |
| `0218f6a0` | 016 | `SP/inv/cpp/vt1.cpp` | virtual dispatch, index 5 |
| `02156fd4` | 004 | `SP/inv/cpp/vt2.cpp` | virtual dispatch, index 58, under register pressure |

Also re-gated the committed `src/Combat/Overlay_23/CallVTableFnAt4Loop_021f698c.cpp` → MATCH
(the dummy-virtuals template is live in the tree today).

Labs: `v1.cpp` class-vs-explicit · `v2/v3/v4/v5.cpp` staging hunt · `mm.cpp` method-call forms ·
`vt.cpp` dispatch forms · `vi.cpp` index/inheritance/sret/const · `og1-og7.cpp` OVERGEN matrix ·
`mg.cpp` mangling sensitivity · `c1/c2a/c2b.cpp` the c30c attempts.
