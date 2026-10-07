"""Do the two gates agree about what is staged? Run this before a wave, not after.

wgate and classify are independent implementations and they HAVE disagreed: ov000:0215858c gated
MATCH under wgate and was thrown out by its own wave as BYTEDIFF, because classify could not see the
per-file compiler override. A wave costs ~6 minutes and a rejection looks like a bad match.

    python pad/preflight.py [<module>]     default: every file under staging/
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__))))
import kitpaths as _kp
import glob
import os
import re
import subprocess
import sys

SP = _kp.SP
KIT = _kp.KIT
REPO = _kp.REPO
sys.path.insert(0, KIT)
from classify import classify

want = sys.argv[1] if len(sys.argv) > 1 else None
rows = []
for d in sorted(glob.glob(f"{SP}/staging/*")):
    lbl = os.path.basename(d)
    mod = "main" if lbl == "main" else lbl[2:]
    if want and mod != want:
        continue
    for f in sorted(glob.glob(f"{d}/*.cpp")):
        txt = open(f, encoding="utf-8", errors="ignore").read()
        m = re.search(r"//\s*(?:SCRATCH-)?USA: func_(?:ov\d+_)?([0-9a-fA-F]{8})", txt)
        if not m:
            rows.append((lbl, os.path.basename(f), "NO-TAG", "-"))
            continue
        addr = m.group(1).lower()
        r = subprocess.run([sys.executable, f"{KIT}/wgate.py", mod, addr, f],
                           capture_output=True, text=True, cwd=REPO)
        out = ((r.stdout or "") + (r.stderr or "")).strip().splitlines()
        gate = next((l.split(":")[0] for l in reversed(out) if l[:1].isupper()), "?")
        cls = classify(mod, {addr: txt}, names={addr: os.path.basename(f)}).get(addr, "?")
        rows.append((lbl, addr, gate, cls))

bad = 0
for lbl, addr, gate, cls in rows:
    ok = (gate == "MATCH") == (cls == "TRUSTED")
    bad += not ok
    print("%-8s %-10s wgate=%-12s classify=%-10s %s"
          % (lbl, addr, gate, cls, "" if ok else "<-- DISAGREE"))
print("%d staged, %d disagreement(s)" % (len(rows), bad))
