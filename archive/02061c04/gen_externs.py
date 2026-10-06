"""Vary the EXTERN SIGNATURES the stuck cases call, not the locals.

Every grid so far changed statements and local types inside the case. The declared return and
parameter types of the callees are equally ours to choose (the ROM only fixes the mangled name), and
they change the IR node the result is bound to -- which is the level at which mwcc names registers.

Usage: python gen_externs.py d3|e7 <out-dir>
       the emitted blocks replace the extern DECLARATION line, so casegrid's from-file is that line.
"""
import os
import sys

which, OUT = sys.argv[1], sys.argv[2]
os.makedirs(OUT, exist_ok=True)

if which == "d3":
    RET = ["void *", "char *", "unsigned char *", "int *", "unsigned int *", "long *",
           "struct S02012fe4 *"]
    pre = "struct S02012fe4;\n"
    for i, r in enumerate(RET):
        decl = 'extern "C" %sfunc_02012fe4(void);' % r
        if r.startswith("struct"):
            decl = pre + decl
        open(os.path.join(OUT, "to_ret%d.txt" % i), "w", encoding="utf-8").write(decl + "\n")
elif which == "e7":
    ARG = ["void *", "char *", "unsigned char *", "int", "struct S12BattleStruct *"]
    pre = "struct S12BattleStruct;\n"
    for i, a in enumerate(ARG):
        decl = ('extern "C" void *_Z24GetCombatantAtField0x3acP12BattleStruct(%s battle);' % a)
        if a.startswith("struct"):
            decl = pre + decl
        open(os.path.join(OUT, "to_arg%d.txt" % i), "w", encoding="utf-8").write(decl + "\n")
else:
    sys.exit("which must be d3 or e7")
print("wrote variants to", OUT)
