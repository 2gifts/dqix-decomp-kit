"""Generate the case-0xe7 replacement grid for pad/casegrid.py.

Residue: ROM has battle=r5 / rec=r4, ours battle=r4 / rec=r5, and `rec` is derived from `battle` so
it can never be defined first. Case 0xbf fell to a change in the ORDER values are defined plus a
split of a compound statement, so this grid moves every declaration in the chain -- including the
constant that defeats the address fold, which in 0xbf's winner had to come FIRST.

Usage: python gen_e7grid.py <out-dir>
"""
import itertools
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

CALL = "void *battle = _Z15GetBattleStructv();"
SNAP = "struct SnapshotE7 *snap = (struct SnapshotE7 *)(rec + snapOff);"
OFF = "int snapOff = 0xcc;"

CHAIN = {
    "e7base": ["char *e7base = (char *)battle + 0x104;", "char *rec = e7base + 0x7400;"],
    "direct": ["char *rec = (char *)battle + 0x104 + 0x7400;"],
    "recdecl": ["char *rec;", "rec = (char *)battle + 0x104 + 0x7400;"],
    "twostep": ["char *e7base = (char *)battle + 0x104;", "char *rec;",
                "rec = e7base + 0x7400;"],
}

# where the snapOff constant goes relative to the call and the chain
OFFPOS = ["first", "after_call", "before_snap"]

# whether snap is a plain declaration or declared then assigned
SNAPF = {
    "init": [SNAP],
    "decl": ["struct SnapshotE7 *snap;", "snap = (struct SnapshotE7 *)(rec + snapOff);"],
}

n = 0
for (cn, chain), op, (sn, snapl) in itertools.product(CHAIN.items(), OFFPOS, SNAPF.items()):
    lines = []
    if op == "first":
        lines.append(OFF)
    lines.append(CALL)
    if op == "after_call":
        lines.append(OFF)
    lines += chain
    if op == "before_snap":
        lines.append(OFF)
    lines += snapl
    open(os.path.join(OUT, "to_%s_%s_%s.txt" % (cn, op, sn)), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines))
    n += 1
print("wrote %d variants to %s" % (n, OUT))
