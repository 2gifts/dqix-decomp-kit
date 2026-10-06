"""Map每 case's stack objects, ROM against ours, so a layout gap can be attributed to one object.

Prints the lowest sp offset each case touches, in address order, for both builds side by side. A
uniform shift between the two lists means one object below them is the wrong size; the row where the
shift changes is the object that is wrong.

Usage: python stackmap.py [src.cpp]
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff

src = sys.argv[1] if len(sys.argv) > 1 else f"{casediff.SP}/c04work/c04.cpp"
rom, ours = casediff.rom_text(), casediff.our_text(src)
rb, ob = casediff.bodies(rom), casediff.bodies(ours)


def used(buf, off, ln):
    out = set()
    for ins in casediff.MD.disasm(buf[off:off + ln], 0):
        for m in re.finditer(r"\[sp, #(0x[0-9a-f]+|\d+)\]", ins.op_str):
            out.add(int(m.group(1), 0))
        m = re.match(r"^\w+, sp, #(0x[0-9a-f]+|\d+)$", ins.op_str)
        if m and ins.mnemonic in ("add", "sub"):
            out.add(int(m.group(1), 0))
    return out


TAIL = (0x6f, 0x70, 0x71, 0x98, 0x9a, 0xa3)
rows = []
for c in sorted(rb):
    if c in TAIL:
        continue
    a = used(rom, *rb[c]) - {0, 4, 8}
    b = (used(ours, *ob[c]) - {0, 4, 8}) if c in ob else set()
    if a:
        rows.append((min(a), c, min(b) if b else None))
for lo, c, mine in sorted(rows):
    d = "" if mine is None else "  %+d" % (mine - lo)
    print("case 0x%02x  rom sp+0x%-5x ours sp+%s%s"
          % (c, lo, ("0x%x" % mine) if mine is not None else "-", d))
