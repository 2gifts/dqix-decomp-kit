"""Attribute a candidate's BYTEDIFF to individual cases.

`wgate` prints one total and the first eight offsets, so a residue spread over four cases reads as
one number and nobody knows which case is worth working. This buckets the differing bytes by the
case body they fall in, masking the words `wgate` masks (branch-with-link, i.e. relocations).

Usage: python bytemap.py <ABS src>
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff

rom = casediff.rom_text()
ours = casediff.our_text(sys.argv[1])
rb, ob = casediff.bodies(rom), casediff.bodies(ours)

owner = {}
for case, (off, ln) in rb.items():
    for k in range(off, off + ln):
        owner.setdefault(k, case)

per = {}
total = 0
for i in range(0, min(len(rom), len(ours)), 4):
    a, b = rom[i:i + 4], ours[i:i + 4]
    if a == b:
        continue
    if a[3] & 0x0F == 0x0B or b[3] & 0x0F == 0x0B:      # bl -> relocation, masked
        continue
    n = sum(1 for x, y in zip(a, b) if x != y)
    key = owner.get(i, "head/pool")
    per[key] = per.get(key, 0) + n
    total += n

for k, v in sorted(per.items(), key=lambda kv: -kv[1]):
    print("%-10s %4d bytes" % (("case 0x%x" % k) if isinstance(k, int) else k, v))
print("total %d bytes over %d region(s)" % (total, len(per)))
