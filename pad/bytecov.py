"""Coverage measured in BYTES OF CODE, not in function count.

`cov.py` counts functions, and the two diverge hard: the medium band is 26% of the remaining
FUNCTIONS but only 7% of the remaining BYTES. If the goal is code completion, the function count
flatters small work and the byte figure is the one to steer by.
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__))))
import kitpaths as _kp
import re
import sys

sys.path.insert(0, _kp.KIT)
import claim

BANDS = [("small  <=64", 0, 64), ("medium 65-256", 65, 256), ("l- 257-512", 257, 512),
         ("l  513-1024", 513, 1024), ("l+ 1025-2048", 1025, 2048), ("xl 2049-4096", 2049, 4096),
         ("massive 4097+", 4097, 1 << 30)]

tot_b = tot_n = done_b = done_n = 0
left = {b[0]: [0, 0] for b in BANDS}
for mod in claim.all_modules():
    try:
        cfg = claim.cfg_for(mod)
        sym = open(cfg + "/symbols.txt", encoding="utf-8", errors="ignore").read()
        dl = open(cfg + "/delinks.txt", encoding="utf-8", errors="ignore").read()
    except OSError:
        continue
    ranges = [(int(a, 16), int(b, 16)) for a, b in re.findall(
        r"(?m)^\s*\.(?:text|init) start:0x([0-9a-fA-F]+) end:0x([0-9a-fA-F]+)\s*$", dl)]
    skip = claim.skiplist()
    for m in re.finditer(r"(?m)^(\S+)\s+kind:function\((?:arm|thumb),size=0x([0-9a-fA-F]+)\)"
                         r"\s+addr:0x([0-9a-fA-F]+)", sym):
        size, addr = int(m.group(2), 16), int(m.group(3), 16)
        if not size or m.group(3).lower() in skip:
            continue
        tot_b += size
        tot_n += 1
        if any(s <= addr < e for s, e in ranges):
            done_b += size
            done_n += 1
            continue
        for name, lo, hi in BANDS:
            if lo <= size <= hi:
                left[name][0] += 1
                left[name][1] += size
                break

rem_b = sum(v[1] for v in left.values())
rem_n = sum(v[0] for v in left.values())
print("BYTES    matched %8d / %8d = %5.2f%%" % (done_b, tot_b, 100.0 * done_b / tot_b))
print("FUNCS    matched %8d / %8d = %5.2f%%" % (done_n, tot_n, 100.0 * done_n / tot_n))
print()
print("%-15s %7s %8s %9s %9s" % ("remaining band", "funcs", "func%", "bytes", "byte%"))
for name, _lo, _hi in BANDS:
    n, b = left[name]
    print("%-15s %7d %7.1f%% %9d %8.1f%%" % (
        name, n, 100.0 * n / rem_n if rem_n else 0, b, 100.0 * b / rem_b if rem_b else 0))
print("%-15s %7d %7s %9d" % ("TOTAL LEFT", rem_n, "", rem_b))
