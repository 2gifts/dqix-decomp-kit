"""Generate the case-0xd3 replacement grid for pad/casegrid.py.

The lesson from 0xbf: what decides the register naming is the ORDER the values are defined plus
whether a compound operation is split into separate statements. This grid varies exactly that --
the position of every declaration relative to the call, and how far the read-modify-write is
broken up -- rather than the cast spellings the standalone sweep already exhausted.

Usage: python gen_d3grid.py <out-dir>
"""
import itertools
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

# (name, the declarations before the call, the declarations after it)
ORDER = [
    ("bo", ["int bit = msg->p1;", "int one = 1;"], []),
    ("ob", ["int one = 1;", "int bit = msg->p1;"], []),
    ("b_o", ["int bit = msg->p1;"], ["int one = 1;"]),
    ("o_b", ["int one = 1;"], ["int bit = msg->p1;"]),
    ("_bo", [], ["int bit = msg->p1;", "int one = 1;"]),
    ("bo_w", ["int bit = msg->p1;", "int one = 1;", "unsigned int *w;"], []),
    ("w_bo", ["unsigned int *w;", "int bit = msg->p1;", "int one = 1;"], []),
]

BODY = [
    ("plain", ["char *base = (char *)p0 + 0x1840;",
               "unsigned int *w = (unsigned int *)(base + 0xb4c);",
               "*w = *w | (one << bit);"]),
    ("split3", ["char *base = (char *)p0 + 0x1840;",
                "unsigned int *w = (unsigned int *)(base + 0xb4c);",
                "unsigned int v = *w;",
                "v = v | (one << bit);",
                "*w = v;"]),
    ("shiftfirst", ["unsigned int m = one << bit;",
                    "char *base = (char *)p0 + 0x1840;",
                    "unsigned int *w = (unsigned int *)(base + 0xb4c);",
                    "*w = *w | m;"]),
    ("basefirst", ["char *base = (char *)p0 + 0x1840;",
                   "unsigned int v = *(unsigned int *)(base + 0xb4c);",
                   "*(unsigned int *)(base + 0xb4c) = v | (one << bit);"]),
    ("nobase", ["unsigned int *w = (unsigned int *)((char *)p0 + 0x1840) + 0xb4c / 4;",
                "*w = *w | (one << bit);"]),
    ("assignw", ["char *base = (char *)p0 + 0x1840;",
                 "w = (unsigned int *)(base + 0xb4c);",
                 "*w = *w | (one << bit);"]),
]

n = 0
for (on, pre, post), (bn, body) in itertools.product(ORDER, BODY):
    hoisted = "unsigned int *w;" in "".join(pre)
    if (bn == "assignw") != hoisted:
        continue                                   # `assignw` needs the hoisted declaration
    lines = list(pre) + ["void *p0 = func_02012fe4();"] + list(post) + list(body) + ["return one;"]
    open(os.path.join(OUT, "to_%s_%s.txt" % (on, bn)), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines))
    n += 1
print("wrote %d variants to %s" % (n, OUT))
