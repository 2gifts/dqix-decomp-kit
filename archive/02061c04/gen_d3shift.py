"""Case-0xd3 with the technique that just moved 0xe4: bind the OTHER operand before the memory read.

0xe4 went 11 -> 9 bytes by binding the shifted value to its own local before reading the byte, which
put that temp in the ROM's register. 0xd3 has the same shape -- a memory word OR'd with a shifted
value -- so sweep the same axis: where `one << bit` is bound relative to the pointer and the load.

Usage: python gen_d3shift.py <out-dir>
"""
import itertools
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

HEAD = ["int bit = msg->p1;", "int one = 1;", "void *p0 = func_02012fe4();"]
BASE = "char *base = (char *)p0 + 0x1840;"
W = "unsigned int *w = (unsigned int *)(base + 0xb4c);"
M = "unsigned int m = one << bit;"
B = "unsigned int b = *w;"

FORMS = {
    "p_m_b": [BASE, W, M, B, "*w = b | m;"],
    "p_m_inlineb": [BASE, W, M, "*w = *w | m;"],
    "m_p_b": [M, BASE, W, B, "*w = b | m;"],
    "p_b_m": [BASE, W, B, M, "*w = b | m;"],
    "p_m_b_acc": [BASE, W, M, B, "m = b | m;", "*w = m;"],
    "p_m_b_accb": [BASE, W, M, B, "b = b | m;", "*w = b;"],
    "nobase_m_b": ["unsigned int *w = (unsigned int *)((char *)p0 + 0x1840) + 0xb4c / 4;", M, B,
                   "*w = b | m;"],
    "p_m_b_swap": [BASE, W, M, B, "*w = m | b;"],
}

n = 0
for name, body in FORMS.items():
    lines = HEAD + body + ["return one;"]
    open(os.path.join(OUT, "to_%s.txt" % name), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines))
    n += 1

# and the same forms with the shift bound before the call, as 0xe4's winner has its value early
for name, body in FORMS.items():
    lines = ["int bit = msg->p1;", "int one = 1;", M.replace("one << bit", "one << bit"),
             "void *p0 = func_02012fe4();"] + [l for l in body if l != M] + ["return one;"]
    open(os.path.join(OUT, "to_early_%s.txt" % name), "w", encoding="utf-8").write(
        "".join("        %s\n" % l for l in lines))
    n += 1
print("wrote %d variants to %s" % (n, OUT))
