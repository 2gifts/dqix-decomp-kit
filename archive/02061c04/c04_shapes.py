"""Classify every case block of the func_02061c04 mega-switch by its instruction SHAPE.

134 cases is not 134 problems if they collapse into a handful of templates: crack one member of each
shape and the rest are generated. This reads the wlist listing, follows the jump table, cuts each
case block, and reports how many distinct shapes there really are.
"""
import collections
import os
import re
import sys

SP = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BASE = 0x02061c04
LST = f"{SP}/pad/c04.lst"

rows = {}
order = []
for line in open(LST, encoding="utf-8", errors="ignore"):
    m = re.match(r"\s*(?:LOOP>)?\s*\+0x([0-9a-f]{4})\s\s(.*?)\s*$", line)
    if not m:
        continue
    off = int(m.group(1), 16)
    rows[off] = m.group(2).split(";")[0].strip()
    order.append(off)

# jump table: the `b` instructions right after `addls pc, pc, r3, lsl #2`
tbl_start = 0x24
targets = []
off = tbl_start
while True:
    t = rows.get(off, "")
    mm = re.match(r"b\s+#0x([0-9a-f]+)$", t)
    if not mm:
        break
    targets.append(int(mm.group(1), 16) - BASE)
    off += 4
print(f"jump table: {len(targets)} entries at +0x{tbl_start:x}..+0x{off - 4:x}")

default = collections.Counter(targets).most_common(1)[0][0]
print(f"default target: +0x{default:04x} (used by {targets.count(default)} ids)")

starts = sorted(set(targets))


def block(start):
    """Instructions from `start` until control leaves the block."""
    out = []
    o = start
    while o in rows and len(out) < 60:
        txt = rows[o]
        out.append(txt)
        if re.match(r"b\s+#", txt) or txt.startswith("pop ") or txt.startswith("bx "):
            break
        o += 4
    return out


def shape(ins):
    """Mnemonic-only signature, so operands do not split identical shapes."""
    return " ".join(i.split()[0] for i in ins if i and not i.startswith(".word"))


shapes = collections.Counter()
examples = {}
for s in starts:
    if s == default:
        continue
    b = block(s)
    sh = shape(b)
    shapes[sh] += 1
    examples.setdefault(sh, (s, b))

print(f"\n{len(starts) - 1} distinct case blocks -> {len(shapes)} distinct shapes\n")
for sh, n in shapes.most_common(12):
    s, b = examples[sh]
    print(f"{n:3d}x  +0x{s:04x}  {sh[:88]}")
    for i in b[:4]:
        print(f"          {i}")
    print()
cov = sum(n for _s, n in shapes.most_common(8))
print(f"top 8 shapes cover {cov} of {sum(shapes.values())} blocks")
