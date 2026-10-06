#!/usr/bin/env python
"""Repoint #include "..." lines whose header the human branch has MOVED.

Merging a human branch regularly relocates headers (their commit "move Random
from System to Util" broke 61 of our sources at once). The move is invisible to
git -- both sides are textually fine, the build is what fails -- so this runs
right after the merge and before ninja.

Only rewrites when the basename resolves to exactly ONE header under include/,
so an ambiguous move is reported and left for a human rather than guessed at.
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
import os
import re
import sys

REPO = sys.argv[1] if len(sys.argv) > 1 else _kp.REPO
INC = os.path.join(REPO, "include")
INCLUDE = re.compile(r'^(\s*#\s*include\s*")([^"]+)(")', re.M)

by_base = {}
for root, _, files in os.walk(INC):
    for f in files:
        if f.endswith((".h", ".hpp")):
            rel = os.path.relpath(os.path.join(root, f), INC).replace("\\", "/")
            by_base.setdefault(f, []).append(rel)

fixed = ambiguous = 0
for root, _, files in os.walk(os.path.join(REPO, "src")):
    for f in files:
        if not f.endswith((".c", ".cpp", ".h", ".hpp")):
            continue
        p = os.path.join(root, f)
        text = open(p, encoding="utf-8", errors="replace").read()
        out, changed = [], False

        def repl(m):
            global changed, ambiguous
            path = m.group(2)
            if os.path.isfile(os.path.join(INC, path)):
                return m.group(0)
            cands = by_base.get(os.path.basename(path), [])
            if len(cands) == 1:
                changed = True
                return m.group(1) + cands[0] + m.group(3)
            if len(cands) > 1:
                ambiguous += 1
                print("  AMBIGUOUS %s in %s -> %s" % (path, os.path.relpath(p, REPO), cands))
            return m.group(0)

        new = INCLUDE.sub(repl, text)
        if changed:
            open(p, "w", encoding="utf-8", newline="\n").write(new)
            fixed += 1
print("fix_includes: repointed %d files (%d ambiguous)" % (fixed, ambiguous))
