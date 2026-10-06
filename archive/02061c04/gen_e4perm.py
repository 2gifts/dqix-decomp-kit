"""Case-0xe4: every legal interleaving of the four bindings.

With `s` pinned to r2, the pool and the byte are allocated over {r1, r3} and we take them the wrong
way round. The only remaining freedom is the order the four values are created in, subject to
`p` before `b` and `v` before `s` -- six interleavings, of which only three have ever been measured.

Usage: python gen_e4perm.py <out-dir>
"""
import itertools
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

STMT = {
    "p": "unsigned char *p = (unsigned char *)&data_02109bf4 + 0xc8;",
    "b": "unsigned int b = *p;",
    "v": "unsigned int v = (unsigned int)(msg->p1 == 0);",
    "s": "unsigned int s = (v << 31) >> 29;",
}
STORE = "*p = (unsigned char)((b & ~4u) | s);"

n = 0
for order in itertools.permutations("pbvs"):
    if order.index("p") > order.index("b") or order.index("v") > order.index("s"):
        continue
    lines = [STMT[k] for k in order] + [STORE, "return 1;"]
    open(os.path.join(OUT, "to_%s.txt" % "".join(order)), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines))
    n += 1

# the same six with the store accumulating into `s` rather than building a fresh value
for order in itertools.permutations("pbvs"):
    if order.index("p") > order.index("b") or order.index("v") > order.index("s"):
        continue
    lines = [STMT[k] for k in order] + ["s = (b & ~4u) | s;", "*p = (unsigned char)s;", "return 1;"]
    open(os.path.join(OUT, "to_acc_%s.txt" % "".join(order)), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines))
    n += 1
print("wrote %d variants to %s" % (n, OUT))
