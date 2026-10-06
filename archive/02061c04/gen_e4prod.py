"""Case-0xe4 PRODUCT grid: every value bound or inline, in every order.

The case is one statement, so the only freedom is which of its three sub-expressions -- the message
field, the object pointer, the boolean -- become named locals, and in what order. 0xbf's crack came
from exactly that kind of rearrangement done all at once rather than one change at a time.

Usage: python gen_e4prod.py <out-dir>
"""
import itertools
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

TY = "struct BitAt2Flag_02109bf4 { unsigned char lo2 : 2, eqFlag : 1, hi5 : 5; };"
PTRDECL = "BitAt2Flag_02109bf4 *f = (BitAt2Flag_02109bf4 *)((char *)&data_02109bf4 + 0xc8);"
PTREXPR = "((BitAt2Flag_02109bf4 *)((char *)&data_02109bf4 + 0xc8))"

n = 0
for pbind, bbind, order in itertools.product(
        ["inline", "local"], ["inline", "uchar", "ushort"], ["p_first", "b_first"]):
    lines = [TY]
    pexpr = PTREXPR
    bexpr = "(msg->p1 == 0)"
    decls = []
    if pbind == "local":
        decls.append(("p", PTRDECL))
        pexpr = "f"
    if bbind == "uchar":
        decls.append(("b", "unsigned char v = (msg->p1 == 0);"))
        bexpr = "v"
    elif bbind == "ushort":
        decls.append(("b", "unsigned short v = (msg->p1 == 0);"))
        bexpr = "v"
    if order == "b_first":
        decls.reverse()
    lines += [d for _, d in decls]
    lines.append("%s->eqFlag = %s;" % (pexpr, bexpr))
    lines.append("return 1;")
    open(os.path.join(OUT, "to_%s_%s_%s.txt" % (pbind, bbind, order)), "w",
         encoding="utf-8").write("".join("        %s\n" % l for l in lines))
    n += 1

# The message field bound to a local as well, crossed with the same choices.
for pbind, bbind in itertools.product(["inline", "local"], ["inline", "uchar"]):
    for mfirst in [True, False]:
        lines = [TY]
        pexpr = PTREXPR
        pre = ["int pv = msg->p1;"]
        if pbind == "local":
            pre.append(PTRDECL)
            pexpr = "f"
        if not mfirst:
            pre.reverse()
        bexpr = "(pv == 0)"
        if bbind == "uchar":
            pre.append("unsigned char v = (pv == 0);")
            bexpr = "v"
        lines += pre
        lines.append("%s->eqFlag = %s;" % (pexpr, bexpr))
        lines.append("return 1;")
        open(os.path.join(OUT, "to_msg_%s_%s_%s.txt"
                          % (pbind, bbind, "mfirst" if mfirst else "mlast")), "w",
             encoding="utf-8").write("".join("        %s\n" % l for l in lines))
        n += 1
print("wrote %d variants to %s" % (n, OUT))
