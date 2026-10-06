"""Case-0xe4, round two: the shift-first form got it to 9 bytes; now swap the last two registers.

With `unsigned int s = (v << 31) >> 29;` bound before the byte read, the shifted value lands in r2
exactly as the ROM has it, and all that is left is that the pool pointer and the loaded byte are the
wrong way round (ours pool=r3 byte=r1, ROM pool=r1 byte=r3). This sweeps where the pointer is bound
relative to the other three values.

Usage: python gen_e4shift.py <out-dir>
"""
import itertools
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

P = "unsigned char *p = (unsigned char *)&data_02109bf4 + 0xc8;"
V = "unsigned int v = (unsigned int)(msg->p1 == 0);"
S = "unsigned int s = (v << 31) >> 29;"
STORE = "*p = (unsigned char)((b & ~4u) | s);"

BTYPE = {"uint": "unsigned int b = *p;", "uchar": "unsigned char b = *p;"}
n = 0
for pos, (bn, B) in itertools.product(range(4), BTYPE.items()):
    lines = [V, S, B]
    lines.insert(pos, P)
    if pos > 2:                                    # p must exist before `b = *p`
        continue
    lines.append(STORE)
    open(os.path.join(OUT, "to_p%d_%s.txt" % (pos, bn)), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines) + "        return 1;\n")
    n += 1

# and the byte read inlined into the store, so `b` never becomes a value of its own
for pos in range(3):
    lines = [V, S]
    lines.insert(pos, P)
    lines.append("*p = (unsigned char)((*p & ~4u) | s);")
    open(os.path.join(OUT, "to_p%d_inlineb.txt" % pos, ), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines) + "        return 1;\n")
    n += 1
print("wrote %d variants to %s" % (n, OUT))
