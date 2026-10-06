"""Is 02061c04's register naming a WHOLE-FUNCTION property or a per-case one?

The four remaining residues are register names in cases scattered through the switch, and no local
rewrite moves any of them. If mwcc's allocation is whole-function, perturbing an unrelated case will
shift them -- and the real defect is a register-only local we are missing somewhere else in the
function, which no stack map can see. If nothing shifts, each case is independently blocked.

Perturbs one case at a time and reports which of 0xbf / 0xd3 / 0xe4 / 0xe7 changed register names.

Usage: MWCC=2.0/sp2p2 python coupling.py <ABS src>
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff

WATCH = [0xBF, 0xD3, 0xE4, 0xE7]


def names(path):
    text = casediff.our_text(path)
    bodies = casediff.bodies(text)
    out = {}
    for c in WATCH:
        if c not in bodies:
            out[c] = None
            continue
        off, ln = bodies[c]
        ins = list(casediff.MD.disasm(text[off:off + ln], 0))
        out[c] = " ".join(re.sub(r"#[-\dxa-f]+", "#", i.op_str) for i in ins)
    return out


src = sys.argv[1]
text = open(src, encoding="utf-8").read()
tmp = os.path.join(os.path.dirname(src), "_coupling.cpp")
base = names(src)

# Each perturbation adds a live value to ONE case, changing that case's bytes and nothing else.
CASES = ["0x64", "0x66", "0x7f", "0x8f", "0x9b", "0xa1", "0xc5", "0xcb", "0xd6", "0xe8"]
for c in CASES:
    anchor = "    case %s: {\n" % c
    if anchor not in text:
        print("%-6s (no brace-form clause)" % c)
        continue
    inj = anchor + "        { int _c = msg->p1 * 3; _c += msg->p2; func_020732cc(_c); }\n"
    open(tmp, "w", encoding="utf-8").write(text.replace(anchor, inj, 1))
    try:
        got = names(tmp)
    except SystemExit:
        print("%-6s compile failed" % c)
        continue
    moved = [hex(k) for k in WATCH if got[k] != base[k]]
    print("%-6s changed: %s" % (c, ", ".join(moved) if moved else "none"))
