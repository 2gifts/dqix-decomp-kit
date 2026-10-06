"""Show what each colorsweep rule proposes for a source, as a one-line diff per candidate.

A rule that fires on comments, or that captures the wrong sub-expression, wastes the whole sweep
budget silently. This makes the first few candidates of every rule readable before a sweep runs.

Usage: python rulecheck.py <file.cpp> [rule-name-substring] [n]
"""
import difflib
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import colorsweep as cs

src = open(sys.argv[1], encoding="utf-8").read()
want = sys.argv[2] if len(sys.argv) > 2 else ""
n = int(sys.argv[3]) if len(sys.argv) > 3 else 3

for rule in cs.RULES:
    if want and want not in rule.__name__:
        continue
    cands = rule(src)
    print("%-26s %d candidates" % (rule.__name__, len(cands)))
    for label, cand in cands[:n]:
        rows = [l for l in difflib.unified_diff(src.split("\n"), cand.split("\n"), lineterm="", n=0)
                if l.startswith(("+", "-")) and not l.startswith(("+++", "---"))]
        print("   %-22s %s" % (label, " | ".join(r.strip()[:58] for r in rows[:3])))
