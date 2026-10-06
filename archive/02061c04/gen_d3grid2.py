"""Second case-0xd3 grid: orders and types the first grid did not reach.

Grid one proved the ORDER of the two integer declarations decides the case (bit,one = 7 bytes;
one,bit = 11; one before the call = 14) and that the read-modify-write's spelling does not matter at
all. So keep sweeping order: where the call result is bound, whether the return value is its own
variable, the integer types, and a spare declaration to shift the allocation.

Usage: python gen_d3grid2.py <out-dir>
"""
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

CALL = "void *p0 = func_02012fe4();"
RMW = ["char *base = (char *)p0 + 0x1840;",
       "unsigned int *w = (unsigned int *)(base + 0xb4c);",
       "*w = *w | (one << bit);"]

V = {
    "p0_first": ["void *p0;", "int bit = msg->p1;", "int one = 1;",
                 "p0 = func_02012fe4();"] + RMW + ["return one;"],
    "call_first": [CALL, "int bit = msg->p1;", "int one = 1;"] + RMW + ["return one;"],
    "rv": ["int bit = msg->p1;", "int one = 1;", CALL] + RMW + ["int rv = one;", "return rv;"],
    "rv_early": ["int bit = msg->p1;", "int one = 1;", "int rv = one;", CALL] + RMW + ["return rv;"],
    "uone": ["int bit = msg->p1;", "unsigned int one = 1;", CALL] + RMW + ["return one;"],
    "ubit": ["unsigned int bit = msg->p1;", "int one = 1;", CALL] + RMW + ["return one;"],
    "sbit": ["short bit = msg->p1;", "int one = 1;", CALL] + RMW + ["return one;"],
    "castbit": ["int bit = (int)msg->p1;", "int one = 1;", CALL] + RMW + ["return one;"],
    "spare_before": ["int spare = msg->p2;", "int bit = msg->p1;", "int one = 1;", CALL] + RMW +
                    ["return one + spare * 0;"],
    "bit_decl_split": ["int bit;", "int one = 1;", "bit = msg->p1;", CALL] + RMW + ["return one;"],
    "one_decl_split": ["int bit = msg->p1;", "int one;", "one = 1;", CALL] + RMW + ["return one;"],
    "both_decl_split": ["int bit;", "int one;", "bit = msg->p1;", "one = 1;", CALL] + RMW +
                       ["return one;"],
    "one_after_base": ["int bit = msg->p1;", CALL, "char *base = (char *)p0 + 0x1840;",
                       "int one = 1;",
                       "unsigned int *w = (unsigned int *)(base + 0xb4c);",
                       "*w = *w | (one << bit);", "return one;"],
    "shift_var_first": ["int bit = msg->p1;", "int one = 1;", "unsigned int m = one << bit;", CALL,
                        "char *base = (char *)p0 + 0x1840;",
                        "unsigned int *w = (unsigned int *)(base + 0xb4c);",
                        "*w = *w | m;", "return one;"],
}

for name, lines in V.items():
    open(os.path.join(OUT, "to_%s.txt" % name), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines))
print("wrote %d variants to %s" % (len(V), OUT))
