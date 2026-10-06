"""Case-0xe4: the bitfield insert written out BY HAND, sweeping where each value is bound.

pad/probe_morph.cpp bisected the residue to two steps: the bitfield store is what first pushes the
pool pointer off r1 (m1 -> m2), and the return value pushes it again (m4 -> m5). Spelling the ROM's
`lsl #31` / `lsr #29` insert explicitly and storing through a plain byte pointer -- the form both
committed r1/r2 examples use -- gets the pool back to r2 in the probe. These are the same forms
measured against the REAL function, where the surrounding colouring differs.

Usage: python gen_e4explicit.py <out-dir>
"""
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

P = "unsigned char *p = (unsigned char *)&data_02109bf4 + 0xc8;"
PV = "volatile unsigned char *p = (volatile unsigned char *)&data_02109bf4 + 0xc8;"
INS = "*p = (unsigned char)((b & ~4u) | ((v << 31) >> 29));"

V = {
    "p_b_v": [P, "unsigned int b = *p;", "unsigned int v = (unsigned int)(msg->p1 == 0);", INS],
    "p_v_b": [P, "unsigned int v = (unsigned int)(msg->p1 == 0);", "unsigned int b = *p;", INS],
    "v_p_b": ["unsigned int v = (unsigned int)(msg->p1 == 0);", P, "unsigned int b = *p;", INS],
    "b_uchar": [P, "unsigned char b = *p;", "unsigned int v = (unsigned int)(msg->p1 == 0);", INS],
    "vol_p_b_v": [PV, "unsigned int b = *p;", "unsigned int v = (unsigned int)(msg->p1 == 0);",
                  INS],
    "inline_v": [P, "unsigned int b = *p;",
                 "*p = (unsigned char)((b & ~4u) | "
                 "((((unsigned int)(msg->p1 == 0)) << 31) >> 29));"],
    "inline_b": [P, "unsigned int v = (unsigned int)(msg->p1 == 0);",
                 "*p = (unsigned char)((*p & ~4u) | ((v << 31) >> 29));"],
    "result_local": [P, "unsigned int b = *p;",
                     "unsigned int v = (unsigned int)(msg->p1 == 0);",
                     "unsigned int r = (b & ~4u) | ((v << 31) >> 29);",
                     "*p = (unsigned char)r;"],
    "decl_split": ["unsigned char *p;", "unsigned int b;", "unsigned int v;",
                   "p = (unsigned char *)&data_02109bf4 + 0xc8;", "b = *p;",
                   "v = (unsigned int)(msg->p1 == 0);", INS],
    "shift_first": [P, "unsigned int v = (unsigned int)(msg->p1 == 0);",
                    "unsigned int s = (v << 31) >> 29;", "unsigned int b = *p;",
                    "*p = (unsigned char)((b & ~4u) | s);"],
}

for name, lines in V.items():
    open(os.path.join(OUT, "to_%s.txt" % name), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines) + "        return 1;\n")
print("wrote %d variants to %s" % (len(V), OUT))
