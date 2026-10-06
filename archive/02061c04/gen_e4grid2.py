"""Second case-0xe4 grid: how the POOL SYMBOL is written, and where the type lives.

Grid one moved the boolean and the pointer around and nothing shifted. The remaining freedom is how
the symbol reference itself is spelled -- a duplicate pool literal is a different IR node, an array
index scales, and a file-scope struct type is parsed before the function body rather than inside it.

Usage: python gen_e4grid2.py <out-dir>
"""
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

TY = "struct BitAt2Flag_02109bf4 { unsigned char lo2 : 2, eqFlag : 1, hi5 : 5; };"
ABS = "((__typeof__(&data_02109bf4))0x02109BF4)"

V = {
    "dupliteral": [TY,
                   "((BitAt2Flag_02109bf4 *)((char *)%s + 0xc8))->eqFlag = (msg->p1 == 0);" % ABS],
    "dupliteral_ptr": [TY,
                       "BitAt2Flag_02109bf4 *f = (BitAt2Flag_02109bf4 *)((char *)%s + 0xc8);" % ABS,
                       "f->eqFlag = (msg->p1 == 0);"],
    "arrayidx": [TY,
                 "((BitAt2Flag_02109bf4 *)&data_02109bf4)[0xc8].eqFlag = (msg->p1 == 0);"],
    "arrayidx_ptr": [TY,
                     "BitAt2Flag_02109bf4 *f = (BitAt2Flag_02109bf4 *)&data_02109bf4;",
                     "f[0xc8].eqFlag = (msg->p1 == 0);"],
    "charidx": [TY,
                "((BitAt2Flag_02109bf4 *)&((char *)&data_02109bf4)[0xc8])->eqFlag "
                "= (msg->p1 == 0);"],
    "wrapper": ["struct WrapE4 { char pad[0xc8]; unsigned char lo2 : 2, eqFlag : 1, hi5 : 5; };",
                "((WrapE4 *)&data_02109bf4)->eqFlag = (msg->p1 == 0);"],
    "wrapper_ptr": ["struct WrapE4 { char pad[0xc8]; unsigned char lo2 : 2, eqFlag : 1, hi5 : 5; };",
                    "WrapE4 *f = (WrapE4 *)&data_02109bf4;",
                    "f->eqFlag = (msg->p1 == 0);"],
    "wrapper_bool": ["struct WrapE4 { char pad[0xc8]; unsigned char lo2 : 2, eqFlag : 1, hi5 : 5; };",
                     "WrapE4 *f = (WrapE4 *)&data_02109bf4;",
                     "int v = msg->p1 == 0;",
                     "f->eqFlag = v;"],
    "deref": [TY,
              "BitAt2Flag_02109bf4 &f = *(BitAt2Flag_02109bf4 *)((char *)&data_02109bf4 + 0xc8);",
              "f.eqFlag = (msg->p1 == 0);"],
}

for name, lines in V.items():
    open(os.path.join(OUT, "to_%s.txt" % name), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines) + "        return 1;\n")
print("wrote %d variants to %s" % (len(V), OUT))
