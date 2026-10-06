"""Generate the case-0xe4 replacement grid for pad/casegrid.py.

ROM: pool=r1, byte=r3, shifted=r2. Ours: pool=r3, byte=r2, shifted=r1. Case 0xbf came out by
changing the ORDER values are defined and splitting a compound operation into statements, so this
grid varies exactly those two things for the bitfield store.

Usage: python gen_e4grid.py <out-dir>
"""
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

TY = "struct BitAt2Flag_02109bf4 { unsigned char lo2 : 2, eqFlag : 1, hi5 : 5; };"
PTR = "BitAt2Flag_02109bf4 *f = (BitAt2Flag_02109bf4 *)((char *)&data_02109bf4 + 0xc8);"
RAW = "unsigned char *p = (unsigned char *)&data_02109bf4 + 0xc8;"

V = {
    "base": [TY,
             "((BitAt2Flag_02109bf4 *)((char *)&data_02109bf4 + 0xc8))->eqFlag = (msg->p1 == 0);"],
    "ptr": [TY, PTR, "f->eqFlag = (msg->p1 == 0);"],
    "ptr_then_bool": [TY, PTR, "int v = (msg->p1 == 0);", "f->eqFlag = v;"],
    "ptr_then_boolu": [TY, PTR, "unsigned char v = (msg->p1 == 0);", "f->eqFlag = v;"],
    "bool_then_ptr": [TY, "int v = (msg->p1 == 0);", PTR, "f->eqFlag = v;"],
    "boolu_then_ptr": [TY, "unsigned char v = (msg->p1 == 0);", PTR, "f->eqFlag = v;"],
    "ptr_decl_first": [TY, "BitAt2Flag_02109bf4 *f;",
                       "f = (BitAt2Flag_02109bf4 *)((char *)&data_02109bf4 + 0xc8);",
                       "f->eqFlag = (msg->p1 == 0);"],
    "raw_rmw": [RAW, "unsigned char b = *p;", "b = (b & ~4) | ((msg->p1 == 0) << 2);", "*p = b;"],
    "raw_rmw_bool1": [RAW, "int v = (msg->p1 == 0);", "unsigned char b = *p;",
                      "b = (b & ~4) | (v << 2);", "*p = b;"],
    "raw_rmw_bool2": ["unsigned char b;", RAW, "b = *p;",
                      "b = (b & ~4) | ((msg->p1 == 0) << 2);", "*p = b;"],
    "raw_split": [RAW, "unsigned char b = *p;", "b = b & ~4;",
                  "b = b | ((msg->p1 == 0) << 2);", "*p = b;"],
    "ptr_split": [TY, PTR, "f->eqFlag = 0;", "if (msg->p1 == 0) f->eqFlag = 1;"],
    "ternary": [TY, PTR, "f->eqFlag = msg->p1 ? 0 : 1;"],
    "not": [TY, PTR, "f->eqFlag = !msg->p1;"],
}

for name, lines in V.items():
    open(os.path.join(OUT, "to_%s.txt" % name), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines) + "        return 1;\n")
print("wrote %d variants to %s" % (len(V), OUT))
