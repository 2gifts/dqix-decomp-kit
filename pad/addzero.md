## `add rD, rS, #0` where you emit `mov rD, rS` — SOLVED, write the zero accumulator
`add rX, sp, #0` is the address of a LOCAL (take the address of a stack object). A REGISTER-to-
register `add r4, r0, #0` is a redundant copy that clean C never emits — and the construct that
produces it is a zero-initialised accumulator: mwcc folds the zero but still emits the arithmetic.

```c
int n = 0;          // NOT `int n = f(...);`
n = n + f(a, b);    // -> bl f ; add r0, r0, #0
```

Committed proof: `func_ov000_02153e40` (`src/Combat/Overlay_0/Name_02153e40.cpp`) is exactly this,
and `SumKeyedLookups02086bf4` is the `short` case. This is the same function core.md once cited as
having "resisted ~73 configurations" — it had not resisted the right one. Do NOT report `add-zero`
and move on.

Both forms are automatic in colorsweep: `r12_zero_accumulator` rewrites a DECLARATION
(`T x = e;` -> `T x = 0; x = x + e;`) and `r15_zero_accumulator_assign` rewrites a plain ASSIGNMENT
to an integer local already declared above. `x = 0; x = e - x` gives the `sub rD, rS, #0` variant.
Integer locals only — the rewrite is not meaning-preserving for pointers or floats.

If the copy feeds an expression rather than a variable (`r7 + r4*2`, `r6 - r4`), give the value a
named integer local first and then apply the accumulator to that local.
