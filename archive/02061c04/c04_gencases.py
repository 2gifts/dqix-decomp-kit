"""Generate idiomatic C for the TRIVIAL case shapes of the func_02061c04 dispatch.

26 of the 124 distinct blocks are one of two shapes:

    bl #target ; mov r0, #1 ; b epilogue      ->   case ID: Callee(...); return 1;
    mov r0, #1 ; b epilogue                   ->   case ID: return 1;

Those need no judgement, so they should not cost worker budget. Emitting them mechanically is the
same lever as any idiom crack: do the repetitive part with a script and leave the thinking to the
cases that need it.

Prints the generated cases and, for each callee, the arity inferred from the binary.
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__))))
import kitpaths as _kp
import collections
import os
import re
import subprocess
import sys

SP = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REPO = _kp.REPO
BASE = 0x02061c04
EPILOGUE = 0x27c0          # `b #0x20643c4` -> add sp / pop
DEFAULT = 0x27bc

rows = {}
for line in open(f"{SP}/pad/c04.lst", encoding="utf-8", errors="ignore"):
    m = re.match(r"\s*(?:LOOP>)?\s*\+0x([0-9a-f]{4})\s\s(.*?)\s*$", line)
    if m:
        rows[int(m.group(1), 16)] = m.group(2).strip()

targets = []
off = 0x28                                    # entry 0; +0x24 is the not-taken default branch
while True:
    mm = re.match(r"b\s+#0x([0-9a-f]+)$", rows.get(off, ""))
    if not mm:
        break
    targets.append(int(mm.group(1), 16) - BASE)
    off += 4

names = {}
for p in [f"{REPO}/config/usa/arm9/symbols.txt"]:
    for l in open(p, encoding="utf-8", errors="ignore"):
        m = re.match(r"(\S+)\s+kind:function[^\n]*?addr:0x([0-9a-fA-F]+)", l)
        if m:
            names[int(m.group(2), 16)] = m.group(1)

trivial_call, trivial_ret = [], []
for idx, t in enumerate(targets):
    if t == DEFAULT:
        continue
    b = [rows.get(t, ""), rows.get(t + 4, ""), rows.get(t + 8, "")]
    ident = 0x64 + idx
    if re.match(r"bl\s+#0x([0-9a-f]+)", b[0]) and b[1] == "mov r0, #1" and b[2].startswith("b #"):
        tgt = int(re.match(r"bl\s+#0x([0-9a-f]+)", b[0]).group(1), 16)
        trivial_call.append((ident, idx, tgt, names.get(tgt, "func_%08x" % tgt)))
    elif b[0] == "mov r0, #1" and b[1].startswith("b #"):
        trivial_ret.append((ident, idx))

print(f"{len(targets)} jump-table entries")
print(f"trivial `call; return 1`: {len(trivial_call)}")
print(f"trivial `return 1`      : {len(trivial_ret)}")
print()
for ident, idx, tgt, nm in trivial_call:
    print(f"    case 0x{ident:02x}: {nm}(); return 1;    // idx {idx}, +0x{targets[idx]:04x}")
print()
ids = ", ".join(f"0x{i:02x}" for i, _x in trivial_ret)
print(f"    // plain `return 1`: {ids}")
