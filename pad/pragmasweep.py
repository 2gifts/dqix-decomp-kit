"""Score a candidate under each mwcc optimisation PRAGMA, one compile per pragma.

The build (`pad/ccscore.py`) and the source are both searched routinely; the pragma state is not,
even though `opt_propagation off` was itself worth 16 bytes on 02061c04. A pragma that renames
registers without changing the instruction sequence is invisible to every source-level search.

Usage: python pragmasweep.py <ABS src> [anchor-line]
       anchor defaults to the existing `#pragma opt_propagation off`.
"""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import casescore

PRAGMAS = [
    "register_coloring on", "register_coloring off",
    "opt_lifetimes on", "opt_lifetimes off",
    "opt_common_subs on", "opt_common_subs off",
    "opt_dead_assignments on", "opt_dead_assignments off",
    "opt_dead_code on", "opt_dead_code off",
    "opt_loop_invariants on", "opt_loop_invariants off",
    "opt_strength_reduction on", "opt_strength_reduction off",
    "opt_strength_reduction_strict on",
    "opt_vectorize_loops off",
    "peephole on", "peephole off",
    "global_optimizer on", "global_optimizer off",
    "opt_propagation on",
    "optimizewithasm on",
    "no_register_coloring on",
    "optimization_level 0", "optimization_level 1", "optimization_level 2",
    "optimization_level 3", "optimization_level 4",
    "optimize_for_size on", "optimize_for_size off",
    "opt_pointer_analysis on", "opt_pointer_analysis off",
    "ARM_conform on", "ARM_conform off",
    "opt_unroll_loops off",
    "scheduling on", "scheduling off",
    "reverse_bitfields on",
    "ipa off",
]

src = sys.argv[1]
anchor = sys.argv[2] if len(sys.argv) > 2 else "#pragma opt_propagation off"
text = open(src, encoding="utf-8").read()
if text.count(anchor) != 1:
    sys.exit("anchor %r occurs %d times" % (anchor, text.count(anchor)))

base = casescore.score(src)
print("base %s %s" % base)
tmp = os.path.join(os.path.dirname(src), "_pragma_probe.cpp")
best = (base[0], None)
for p in PRAGMAS:
    open(tmp, "w", encoding="utf-8").write(text.replace(anchor, anchor + "\n#pragma " + p))
    s, detail = casescore.score(tmp)
    flag = "  <-- BETTER" if s < base[0] else ""
    print("%-38s %-8s %s%s" % (p, s, detail, flag))
    if s < best[0]:
        best = (s, p)
print("BEST", best[1] or "(none beats base)", best[0])
