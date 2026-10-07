"""Mine worker verdicts for TOOL complaints, not match/miss.

Every large-function fault found on 2026-08-20 was a clause buried in a SKIP that was otherwise about
the function: "found wlist.py capstone dies at +0x1414 on mid-func literal pool" sat inside a verdict
about a 134-case switch, and it was worth more than the function. Workers report these once, in
passing, and then work around them privately -- one shipped its own `redecode.py` rather than saying
the tool was broken.

A verdict naming a TOOL is a bug report. This surfaces them so none is read once and lost.

    python toolgripes.py [--hours N] [--all]
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
import glob
import json
import os
import re
import sys
import time

SP = _kp.SP
KIT = _kp.KIT
HOURS = float(sys.argv[sys.argv.index("--hours") + 1]) if "--hours" in sys.argv else 24.0
ALL = "--all" in sys.argv

# Phrases that mean "the harness got in my way", as opposed to "this function is hard".
SIGNALS = [
    (r"\b(wlist|wdiff|wgate|colorsweep|scaffold|recipe_select|claim|resumable|repairsweep)\b[^.;]{0,80}"
     r"(die|dies|died|crash|fail|broke|broken|bug|wrong|missing|truncat|inert|blocked|refus|cannot|can't)",
     "names a tool and a failure"),
    (r"(capstone|disassembl|listing)[^.;]{0,60}(stop|dies|truncat|wrong|incomplete|missing)",
     "listing or disassembly wrong"),
    (r"(budget|cap)[^.;]{0,40}(exhaust|ran out|too (?:small|low)|truncat)",
     "budget ended the work"),
    (r"(doc|recipe|prompt)[^.;]{0,50}(truncat|missing|incomplete|not (?:included|delivered))",
     "worker doc incomplete"),
    (r"\b(workaround|worked around|wrote my own|hand-rolled)\b",
     "worker built its own tool"),
    (r"(symbol|declaration)[^.;]{0,60}(silently wrong|actually|not one|off-by-one|misrout)",
     "config or signature wrong"),
]

rows = []
for p in glob.glob(f"{SP}/wlog/*.json"):
    age = (time.time() - os.path.getmtime(p)) / 3600.0
    if not ALL and age > HOURS:
        continue
    try:
        d = json.load(open(p, encoding="utf-8", errors="ignore"))
    except Exception:
        continue
    text = str(d.get("result", ""))
    if not text.strip():
        continue
    hits = []
    for pat, why in SIGNALS:
        m = re.search(pat, text, re.I)
        if m:
            hits.append((why, m.group(0)[:120]))
    if hits:
        rows.append((os.path.getmtime(p), os.path.basename(p), d.get("total_cost_usd", 0.0), hits))

rows.sort()
print(f"{len(rows)} verdict(s) naming a tool problem in the last "
      + ("whole log" if ALL else f"{HOURS:g}h") + "\n")
for ts, name, cost, hits in rows:
    print(f"{time.strftime('%m-%d %H:%M', time.localtime(ts))}  {name}  ${cost:.2f}")
    for why, quote in hits:
        print(f"    [{why}] {quote}")
    print()
if rows:
    print("Each of these is a bug report. Fix the tool, then re-gate the function for free with")
    print("`python $KIT/pad/retry_failed.py <mod>:<addr>` before spending another session on it.")
