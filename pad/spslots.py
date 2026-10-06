"""Every distinct sp+N referenced by the ROM function and by a candidate, side by side.

stackmap.py reports only each case's LOWEST slot, so an object that no case reaches at its
base is invisible to it -- which is exactly the shape of a missing local.

Usage: python spslots.py <src>
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff

SP = re.compile(r"sp, #(0x[0-9a-f]+|\d+)|\[sp, #(0x[0-9a-f]+|\d+)\]")


def slots(text):
    out = set()
    for ins in casediff.MD.disasm(text, 0):
        m = SP.search(ins.op_str)
        if m:
            out.add(int(m.group(1) or m.group(2), 0))
    return out


rom = slots(casediff.rom_text())
ours = slots(casediff.our_text(sys.argv[1]))
allslots = sorted(rom | ours)
for s in allslots:
    tag = "both" if s in rom and s in ours else ("ROM-only" if s in rom else "ours-only")
    if tag != "both":
        print("0x%03x  %s" % (s, tag))
print("rom %d slots, ours %d" % (len(rom), len(ours)))
