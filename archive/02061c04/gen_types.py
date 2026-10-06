"""Type grids: the one IR-visible axis the earlier sweeps never touched.

Every 0xd3 grid varied the spelling of the read-modify-write and the integer types of `bit` and
`one`, but never the POINTED-TO type of the word being updated; every 0xe7 grid moved the
fold-defeating constant around but never changed its type. A different type is a different IR node,
which is exactly the level at which the register naming is decided.

Usage: python gen_types.py d3|e7 <out-dir>
"""
import itertools
import os
import sys

which, OUT = sys.argv[1], sys.argv[2]
os.makedirs(OUT, exist_ok=True)
n = 0

if which == "d3":
    PTR = ["unsigned int", "int", "unsigned long", "long", "volatile unsigned int"]
    BASE = ["char", "unsigned char", "signed char"]
    for pt, bt in itertools.product(PTR, BASE):
        lines = ["int bit = msg->p1;", "int one = 1;", "void *p0 = func_02012fe4();",
                 "%s *base = (%s *)p0 + 0x1840;" % (bt, bt),
                 "%s *w = (%s *)(base + 0xb4c);" % (pt, pt),
                 "*w = *w | (one << bit);", "return one;"]
        name = "to_%s_%s.txt" % (pt.replace(" ", ""), bt.replace(" ", ""))
        open(os.path.join(OUT, name), "w", encoding="utf-8").write(
            "".join("        %s\n" % l for l in lines))
        n += 1
elif which == "e7":
    OFFT = ["int", "unsigned int", "short", "unsigned short", "long", "char", "unsigned char"]
    RECT = ["char", "unsigned char"]
    for ot, rt in itertools.product(OFFT, RECT):
        lines = ["void *battle = _Z15GetBattleStructv();",
                 "%s *e7base = (%s *)battle + 0x104;" % (rt, rt),
                 "%s *rec = e7base + 0x7400;" % rt,
                 "%s snapOff = 0xcc;" % ot,
                 "struct SnapshotE7 *snap = (struct SnapshotE7 *)(rec + snapOff);"]
        name = "to_%s_%s.txt" % (ot.replace(" ", ""), rt.replace(" ", ""))
        open(os.path.join(OUT, name), "w", encoding="utf-8").write(
            "".join("        %s\n" % l for l in lines))
        n += 1
else:
    sys.exit("which must be d3 or e7")
print("wrote %d variants to %s" % (n, OUT))
