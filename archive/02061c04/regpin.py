"""Reserve one register file-wide with a GCC-style global register variable, and score.

mwcc accepts `register T x asm("rN")` but ONLY at file scope before any code is generated (the
error on a local is "cannot declare global register variables after code has been generated").
Reserving a register changes which ones the allocator has left, which is the one axis that moves
register NAMING without touching the source of any case.

Usage: MWCC=2.0/sp2p2 python regpin.py <ABS src>
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casescore

src = sys.argv[1]
text = open(src, encoding="utf-8").read()
tmp = os.path.join(os.path.dirname(src), "_regpin.cpp")

print("base   %s %s" % casescore.score(src))
for reg in ["r1", "r2", "r3", "r7", "r8", "r9", "r10", "r11"]:
    open(tmp, "w", encoding="utf-8").write(
        'register int _pin_%s asm("%s");\n' % (reg, reg) + text)
    s, detail = casescore.score(tmp)
    print("pin %-4s %-8s %s" % (reg, s, detail))
