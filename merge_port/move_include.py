import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__))))
import kitpaths as _kp
import os
import re

REPO = _kp.REPO
INC = re.compile(r'^[ \t]*#\s*include\s*"GameState/GameState\.h"[^\r\n]*\r?\n', re.M)
USE = re.compile(r"\bGame(?:State|Object)\b")
ANY_INC = re.compile(r"^[ \t]*#\s*include\b[^\r\n]*\r?\n", re.M)

moved = 0
for root, _, files in os.walk(os.path.join(REPO, "src")):
    for f in files:
        if not f.endswith((".c", ".cpp")):
            continue
        p = os.path.join(root, f)
        text = open(p, encoding="utf-8", errors="surrogateescape", newline="").read()
        m = INC.search(text)
        if not m:
            continue
        rest = text[:m.start()] + text[m.end():]
        use = USE.search(rest)
        if not use or use.start() >= m.start():
            continue
        before = [i for i in ANY_INC.finditer(rest) if i.end() <= use.start()]
        at = before[-1].end() if before else 0
        text = rest[:at] + m.group(0) + rest[at:]
        open(p, "w", encoding="utf-8", errors="surrogateescape", newline="").write(text)
        moved += 1
print("moved GameState include above first use in %d files" % moved)
