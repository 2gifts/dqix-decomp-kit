"""Per-case body sizes, target versus ours, from a wdiff jump table dump.

Input lines are `<idx> <rom_target_hex> <our_target_hex>`. A body's size is the distance to the
next distinct target in its own layout, so a case that shares a body with its neighbour reads as
one body in one column and several in the other -- which is the whole point.
"""
import sys

rows = [line.split() for line in open(sys.argv[1]) if line.strip()]
rows = [(int(a), int(b, 16), int(c, 16)) for a, b, c in rows]


def sizes(targets):
    uniq = sorted(set(targets))
    return {t: (uniq[i + 1] - t if i + 1 < len(uniq) else 0) for i, t in enumerate(uniq)}


rom_sz, our_sz = sizes([r[1] for r in rows]), sizes([r[2] for r in rows])
short = 0
for idx, rt, ot in rows:
    r, o = rom_sz[rt], our_sz[ot]
    if r != o:
        short += r - o
        print("case 0x%02x  rom 0x%04x (%4d B)  ours 0x%04x (%4d B)  %+d"
              % (idx + 0x64, rt, r, ot, o, r - o))
print("net %+d bytes across %d cases" % (short, len(rows)))
