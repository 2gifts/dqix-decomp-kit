import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__)))))
import kitpaths as _kp
import json
import os
import subprocess
import sys

FRIDA = (_kp.SP + "/frida")
REPO = _kp.REPO
OPS = {0x6d: "add", 0x7a: "cmp", 0x7b: "mov", 0x80: "pool", 0xa5: "ldr", 0xa9: "ldrb", 0xad: "ldrsh",
       0xb2: "shift", 0x76: "bl", 0x71: "b", 0xee: "str", 0xbd: "mvn"}

src = os.path.abspath(sys.argv[1]).replace("\\", "/")
out = src[:-4] + ".jsonl"
if not os.path.exists(out) or os.path.getmtime(out) < os.path.getmtime(src):
    subprocess.run([sys.executable, FRIDA + "/schedtrace.py", src, out], cwd=REPO, check=True, capture_output=True)
watch = {int(v) for v in sys.argv[2:]}
for line in open(out):
    e = json.loads(line)
    if e["ev"] == "block" and e["pass"] == 1:
        rows = []
        for i in e["insns"]:
            regs = [o[3] for o in i["ops"] if o[0] == 0]
            txt = " ".join("%s%d" % ("=" if o[2] & 2 else "", o[3]) for o in i["ops"] if o[0] == 0)
            imm = [o[4] for o in i["ops"] if o[0] != 0]
            if not watch or watch & set(regs):
                rows.append("  %-6s %-24s %s" % (OPS.get(i["op"], "%02x" % i["op"]), txt, imm[:2]))
        if rows:
            print("block", e["idx"])
            print("\n".join(rows))
