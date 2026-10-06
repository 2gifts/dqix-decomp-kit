## MASSIVE FUNCTIONS (>4KB): THE METHOD

Established 2026-08-21 on `func_02061c04` (10204 bytes, 2551 instructions, 134-case dispatch).

### 1. Check the listing is complete BEFORE anything else
capstone `disasm()` STOPS at the first undecodable bytes, and ARM requires literal pools within
±4KB of their `ldr`, so every function above ~4KB has one mid-body. Nine tools had no `skipdata` and
silently truncated: `wlist` showed 1278 of 2551 instructions with no marker saying the rest was
missing. All nine are fixed; if a listing ever looks half-data again, suspect this first.

    python $SP/wlist.py <mod> <addr> | grep -c '+0x'      # must equal size/4

### 2. Run the literal translator FIRST — it is free
`translate.py` transliterates ARM to `goto`-structured C. On a mega-switch it now emits the whole
function: the computed jump (`addls pc, pc, rN, lsl #2`) becomes a real `switch` whose cases `goto`
the block labels, sp-relative addressing becomes `stk[]`, and each callee is declared once at its
widest arity. Zero tokens, and the output COMPILES:

    python $SP/translate.py main 02061c04         # "partial" = UNTRANSLATED markers remain

To find what is blocking it, list the markers by form -- that list IS the work queue, and every form
taught to the translator applies to every future function:

    python $SP/pad/c04_untranslated.py

### 3. Expect structure, not bytes
The transliteration reproduces control flow, calls and field accesses exactly, but colours registers
its own way (r7/r8/sb where the ROM used r4/r5/r6), so it lands a few percent over size -- 164 bytes
on 10204. **Declaration order does not fix it**: all four orders of the register locals emit the
identical 0x2880, because mwcc allocates by USE order. Do not spend budget permuting it.

### 4. Land it by CONTINUATION, not by one big session
Measured: cost per byte is constant across sessions (~$0.0035-0.005), cost per message is NOT --
$0.062/msg under 40 messages against $0.178 past 220. So several short sessions beat one long one,
and `--max-budget-usd` should stay near $10.

    bash $SP/resume_one.sh main 02061c04 10       # hands over the best prior automatically

Continuation demonstrably accumulates AND self-corrects: session 2 on 02061c04 found a case-label
off-by-one misrouting jump-table slots 13 and 24, and proved `func_0202ae18` takes zero args --
"silently wrong in every prior case". Session 2 on 0205faf4 (8420B) went from 41 to 49 cases and
fixed two symbol-linkage bugs. Each session must leave a header comment saying what is done, what is
drafted-but-unverified, and what is untouched.

### 5. Two priors, two purposes
* `attempts/Trans_<addr>.cpp` — the complete transliteration. Use it as the per-case REFERENCE: it
  has correct C for every block, including the ~58 cases past the point where the old listing died.
* `attempts/<Semantic>_<addr>.cpp` — the idiomatic partial. This is what actually lands, because each
  case can be verified byte-exact on its own.

The transliteration tells you WHAT each case does; the idiomatic file is where it gets written so it
matches. Neither alone finishes a 10KB function.
