"""Side-by-side of the function head (prologue + dispatch) — the region casediff cannot reach.

Usage: python headdiff.py [src.cpp] [count]
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff

src = sys.argv[1] if len(sys.argv) > 1 else f"{casediff.SP}/c04work/c04.cpp"
n = int(sys.argv[2]) if len(sys.argv) > 2 else 12
rom, ours = casediff.rom_text(), casediff.our_text(src)
a = list(casediff.MD.disasm(rom[:n * 4], 0))
b = list(casediff.MD.disasm(ours[:n * 4], 0))
for i in range(max(len(a), len(b))):
    l = "%-38s" % ("%s %s" % (a[i].mnemonic, a[i].op_str) if i < len(a) else "")
    r = "%s %s" % (b[i].mnemonic, b[i].op_str) if i < len(b) else ""
    print("  %s%s| %s" % ("*" if l.strip() != r.strip() else " ", l, r))
