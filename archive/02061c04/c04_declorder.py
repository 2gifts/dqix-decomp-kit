"""Does the ORDER of the register-variable declarations steer mwcc's allocation?

The transliteration of func_02061c04 is structurally right -- same jump table, same calls -- but 164
bytes over, because it colours r7/r8/sb where the target uses r4/r5/r6 and mwcc inserts moves to
reconcile. mwcc hands out callee-saved registers in REVERSE declaration order (recipe #9), so the
declaration block is the only lever a C source has over that mapping.

Tries each order, reports emitted size against the slot.
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__))))
import kitpaths as _kp
import os
import re
import subprocess
import sys

SP = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REPO = _kp.REPO
SRC = open(f"{SP}/pad/c04_draft.cpp", encoding="utf-8").read()
NL = chr(10)

decls = re.findall(r"^    unsigned int (r\d+) = 0;$", SRC, re.M)
print(f"register locals: {' '.join(decls)}")
block = NL.join(f"    unsigned int {r} = 0;" for r in decls)
assert block in SRC, "declaration block not contiguous"

ORDERS = {
    "as-is": decls,
    "reversed": list(reversed(decls)),
    "high-first": sorted(decls, key=lambda r: -int(r[1:])),
    "low-first": sorted(decls, key=lambda r: int(r[1:])),
}

for name, order in ORDERS.items():
    new = NL.join(f"    unsigned int {r} = 0;" for r in order)
    p = f"{SP}/pad/c04_ord_{name}.cpp"
    open(p, "w", encoding="utf-8", newline="\n").write(SRC.replace(block, new, 1))
    r = subprocess.run([sys.executable, f"{SP}/wgate.py", "main", "02061c04", p],
                       capture_output=True, text=True, encoding="utf-8", errors="replace",
                       cwd=REPO, stdin=subprocess.DEVNULL)
    out = ((r.stdout or "") + (r.stderr or "")).strip()
    m = re.search(r"total=0x([0-9a-f]+)", out)
    size = int(m.group(1), 16) if m else None
    line = out.splitlines()[0][:60] if out else "(no output)"
    if size:
        print(f"{name:12s} 0x{size:x}  ({size - 0x27dc:+d} vs slot)")
    else:
        print(f"{name:12s} {line}")
