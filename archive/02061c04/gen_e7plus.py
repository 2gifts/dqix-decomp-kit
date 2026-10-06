"""Case-0xe7's ladder, from the committed source that has it.

`CheckBitFlagsAcrossEntries_0202bd68.cpp` is byte-exact and carries the same shape -- a pointer
derived from a saved value through TWO adds, where the derived pointer gets the LOWER callee-saved
register. It does not write two variables:

    signed char* arr = (signed char*)((char*)self + 0x38);
    int i = 0;
    arr += 0x1000;

ONE variable, the second offset applied by a separate `+=` statement, with an unrelated declaration
in between. We write `e7base` and `rec` as two variables. This sweeps that difference.

Usage: python gen_e7plus.py <out-dir>
"""
import itertools
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

CALL = "void *battle = _Z15GetBattleStructv();"
OFF = "int snapOff = 0xcc;"
SNAP = "struct SnapshotE7 *snap = (struct SnapshotE7 *)(rec + snapOff);"
DECL = "char *rec = (char *)battle + 0x104;"

PLUS = {"pluseq": "rec += 0x7400;", "assign": "rec = rec + 0x7400;"}
BETWEEN = {"none": [], "off": [OFF], "dummy": ["int e7i = 0;"]}

n = 0
for (pn, plus), (bn, between) in itertools.product(PLUS.items(), BETWEEN.items()):
    lines = [CALL, DECL] + between + [plus]
    if bn != "off":
        lines.append(OFF)
    lines.append(SNAP)
    body = "".join("        %s\n" % l for l in lines)
    if bn == "dummy":
        body = body.replace(SNAP, SNAP + "\n        (void)e7i;")
    open(os.path.join(OUT, "to_%s_%s.txt" % (pn, bn)), "w", encoding="utf-8").write(body)
    n += 1

# the same, with the constant that defeats the address fold declared FIRST
for (pn, plus) in PLUS.items():
    lines = [OFF, CALL, DECL, plus, SNAP]
    open(os.path.join(OUT, "to_%s_offfirst.txt" % pn), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines))
    n += 1

# and with the whole chain on one variable from the start
for (pn, plus) in PLUS.items():
    lines = [CALL, "char *rec = (char *)battle;", "rec += 0x104;", plus, OFF, SNAP]
    open(os.path.join(OUT, "to_%s_threestep.txt" % pn), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines))
    n += 1
print("wrote %d variants to %s" % (n, OUT))
