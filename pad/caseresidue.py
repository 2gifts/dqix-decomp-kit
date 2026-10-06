"""Which CASE bodies still differ, and on which instruction pairs.

`casescore.py` returns one number for the whole function, which is the right objective for a sweep
and useless for deciding what to edit next. This prints the same comparison per case: the case
label, how many rows differ, and each differing pair with its offset -- so a 27-byte residue in a
10KB function points at two case bodies instead of at 134.

Usage: python pad/caseresidue.py [src.cpp] [--all]
       (default src: $SP/c04work/c04.cpp; --all also lists cases whose LENGTH differs)
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff
import fulldiff

SRC = next((a for a in sys.argv[1:] if not a.startswith("--")), f"{casediff.SP}/c04work/c04.cpp")
SHOW_ALL = "--all" in sys.argv

rom, ours = casediff.rom_text(), casediff.our_text(SRC)
rb, ob = casediff.bodies(rom), casediff.bodies(ours)

wrong_len, total, hit_cases = [], 0, 0
for case in sorted(rb):
    ro, rl = rb[case]
    if case not in ob:
        wrong_len.append((case, "missing"))
        continue
    if ob[case][1] != rl:
        wrong_len.append((case, "len 0x%x vs 0x%x" % (ob[case][1], rl)))
        continue
    oo = ob[case][0]
    a = list(casediff.MD.disasm(rom[ro:ro + rl], ro))
    b = list(casediff.MD.disasm(ours[oo:oo + rl], ro))
    rows = []
    for i in range(min(len(a), len(b))):
        x, y = a[i], b[i]
        if x.mnemonic == y.mnemonic and x.op_str == y.op_str:
            continue
        k = fulldiff.kind(x, y, ro, ro + rl)
        if k == "pool":
            continue
        rows.append((x.address, k, "%s %s" % (x.mnemonic, x.op_str), "%s %s" % (y.mnemonic, y.op_str)))
    if rows:
        total += len(rows)
        hit_cases += 1
        print("case 0x%02x  %d row(s)  [body 0x%x..0x%x]" % (case, len(rows), ro, ro + rl))
        for addr, k, t, m in rows:
            print("    +0x%04x  %-5s ROM %-34s ours %s" % (addr, k, t, m))

if wrong_len and (SHOW_ALL or not total):
    print("\nwrong-length bodies (compared as whole, not row by row):")
    for case, why in wrong_len:
        print("    case 0x%02x  %s" % (case, why))
print("\n%d differing row(s) across %d case body(ies); %d body(ies) are the wrong length"
      % (total, hit_cases, len(wrong_len)))
