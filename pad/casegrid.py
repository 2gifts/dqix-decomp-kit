"""Sweep replacement bodies for ONE case against the REAL function, scored per case.

A standalone probe is not faithful: case 0xbf's winning shape came out of a probe that omitted the
case's own tail, and applying it made the case WORSE. The real function also enters every case with
obj/msg/param3 already colouring r6/r4/r5, which no small probe reproduces. This costs one full
compile per variant (~10 s) and is worth it.

Usage: python casegrid.py <ABS src> <case-hex> <from.txt> <to-dir>
       every `<to-dir>/to_*.txt` is tried in place of <from.txt>.
"""
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff

src, case_s, ffile, todir = sys.argv[1:5]
case = int(case_s, 16)
base_text = open(src, encoding="utf-8").read()
old = open(ffile, encoding="utf-8").read().rstrip("\n")
if base_text.count(old) != 1:
    sys.exit("from-block occurs %d times, expected 1" % base_text.count(old))

rom = casediff.rom_text()
rb = casediff.bodies(rom)
tmp = os.path.join(os.path.dirname(src), "_casegrid_%x.cpp" % case)


def case_bytes(path):
    try:
        ours = casediff.our_text(path)
    except SystemExit:
        return None, None
    ob = casediff.bodies(ours)
    if case not in ob or case not in rb:
        return None, None
    ro, rl = rb[case]
    oo, ol = ob[case]
    if ol != rl:
        return None, ol - rl
    n = 0
    for k in range(0, rl, 4):
        a, b = rom[ro + k:ro + k + 4], ours[oo + k:oo + k + 4]
        if a == b or a[3] & 0x0F == 0x0B or b[3] & 0x0F == 0x0B:
            continue
        n += sum(1 for x, y in zip(a, b) if x != y)
    return n, 0


nb, _ = case_bytes(src)
print("base  %s bytes" % nb)
best = (nb, None)
for f in sorted(glob.glob(os.path.join(todir, "to_*.txt"))):
    new = open(f, encoding="utf-8").read().rstrip("\n")
    open(tmp, "w", encoding="utf-8").write(base_text.replace(old, new))
    n, delta = case_bytes(tmp)
    tag = "%s bytes" % n if n is not None else ("SIZE %+d" % delta if delta else "compile failed")
    mark = "   <-- BETTER" if n is not None and nb is not None and n < nb else ""
    print("%-28s %s%s" % (os.path.basename(f), tag, mark))
    if n is not None and (best[0] is None or n < best[0]):
        best = (n, f)
print("BEST %s %s" % (best[1] or "(base)", best[0]))
