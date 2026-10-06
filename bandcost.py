"""Cost per band from the worker logs, in $/match AND $/matched-byte.

Coverage is 79.99% by function but 37.68% by byte, so $/match flatters small functions by ~an order
of magnitude. This prints both so a band decision is made on the metric that matches the goal.
"""
import collections
import glob
import os
import re

SP = os.path.dirname(os.path.abspath(__file__))
CLAIM = re.compile(r"claim ([0-9a-fA-F]{8}) (\d+)B")
VERD = re.compile(r"\b(MATCH|miss) ([0-9a-fA-F]{8}) \$?([0-9.]+)")

BANDS = [(64, "small<=64"), (256, "med 65-256"), (1024, "large 257-1024"),
         (2048, "l+ 1025-2048"), (4096, "xl 2049-4096"), (1 << 30, "massive 4097+")]


def band(n):
    for hi, name in BANDS:
        if n <= hi:
            return name
    return BANDS[-1][1]


sizes, rows = {}, []
for p in sorted(glob.glob(os.path.join(SP, "wlog", "pull_*_s*.log"))):
    for ln in open(p, encoding="utf-8", errors="ignore"):
        m = CLAIM.search(ln)
        if m:
            sizes[m.group(1).lower()] = int(m.group(2))
        m = VERD.search(ln)
        if m:
            rows.append((m.group(1), m.group(2).lower(), float(m.group(3))))

agg = collections.defaultdict(lambda: [0, 0, 0.0, 0])
unknown = 0
for verdict, addr, cost in rows:
    s = sizes.get(addr)
    if not s:
        unknown += 1
        continue
    a = agg[band(s)]
    a[0] += 1
    a[2] += cost
    if verdict == "MATCH":
        a[1] += 1
        a[3] += s

print("%-15s %6s %8s %9s %11s %14s %10s" % ("band", "tried", "matched", "spend", "$/match",
                                            "$/matched-byte", "bytes"))
tt = tm = 0
ts = 0.0
tb = 0
for _hi, name in BANDS:
    if name not in agg:
        continue
    t, mt, sp, by = agg[name]
    tt += t
    tm += mt
    ts += sp
    tb += by
    print("%-15s %6d %8d %9.2f %11s %14s %10d"
          % (name, t, mt, sp, ("$%.2f" % (sp / mt)) if mt else "-",
             ("$%.5f" % (sp / by)) if by else "-", by))
print("%-15s %6d %8d %9.2f %11s %14s %10d"
      % ("ALL", tt, tm, ts, ("$%.2f" % (ts / tm)) if tm else "-",
         ("$%.5f" % (ts / tb)) if tb else "-", tb))
if unknown:
    print("(%d verdict line(s) had no claim line for their size and were skipped)" % unknown)
