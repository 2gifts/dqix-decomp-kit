"""Case-0xd3 PRODUCT grid, built on what cracked 0xbf.

0xbf fell to three simultaneous changes, and the one nobody would have guessed was taking the loop
counter from the LITERAL instead of from the variable that held the same value -- breaking a
dependency changes which values are live together, and that is what renames the registers.

0xd3's `one` is used twice: as the shift base and as the return value. This grid splits that into
two independent literals and sweeps where each is declared, crossed with the read-modify-write
spellings.

Usage: python gen_d3prod.py <out-dir>
"""
import itertools
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

CALL = "void *p0 = func_02012fe4();"
BIT = "int bit = msg->p1;"
ONE = "int one = 1;"

RV = {
    "none": (None, "return one;"),
    "lit_first": ("int rv = 1;", "return rv;"),
    "lit_after_bit": ("int rv = 1;", "return rv;"),
    "lit_after_one": ("int rv = 1;", "return rv;"),
    "lit_after_call": ("int rv = 1;", "return rv;"),
}
RVPOS = {"lit_first": 0, "lit_after_bit": 1, "lit_after_one": 2, "lit_after_call": 3}

BODY = {
    "plain": ["char *base = (char *)p0 + 0x1840;",
              "unsigned int *w = (unsigned int *)(base + 0xb4c);",
              "*w = *w | (one << bit);"],
    "split": ["char *base = (char *)p0 + 0x1840;",
              "unsigned int *w = (unsigned int *)(base + 0xb4c);",
              "unsigned int v = *w;",
              "unsigned int m = one << bit;",
              "v = v | m;",
              "*w = v;"],
    "shiftfirst": ["unsigned int m = one << bit;",
                   "char *base = (char *)p0 + 0x1840;",
                   "unsigned int *w = (unsigned int *)(base + 0xb4c);",
                   "*w = *w | m;"],
}

n = 0
for rvn, (bodyn, body) in itertools.product(RV, BODY.items()):
    decl, ret = RV[rvn]
    lines = [BIT, ONE, CALL]
    if decl:
        lines.insert(RVPOS[rvn], decl)
    lines += body + [ret]
    open(os.path.join(OUT, "to_%s_%s.txt" % (rvn, bodyn)), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines))
    n += 1

# The mirror: keep `return one;` but take the SHIFT base from a second literal.
for pos, tag in [(0, "sh_first"), (2, "sh_after_one"), (3, "sh_after_call")]:
    for bodyn, body in BODY.items():
        lines = [BIT, ONE, CALL]
        lines.insert(pos, "int sh = 1;")
        b = [l.replace("one << bit", "sh << bit") for l in body]
        lines += b + ["return one;"]
        open(os.path.join(OUT, "to_%s_%s.txt" % (tag, bodyn)), "w", encoding="utf-8").write(
            "".join("        %s\n" % l for l in lines))
        n += 1
print("wrote %d variants to %s" % (n, OUT))
