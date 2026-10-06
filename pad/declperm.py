import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__))))
import kitpaths as _kp
import json
import os
import random
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

SP = _kp.SP
REPO = _kp.REPO
HERE = os.path.dirname(os.path.abspath(__file__))
MOD, ADDR = sys.argv[1], sys.argv[2]
TEMPLATE = open(sys.argv[3], encoding="utf-8").read()
decls = json.load(open(sys.argv[4]))
INDENT = sys.argv[5] if len(sys.argv) > 5 else "        "
OUT = os.path.join(HERE, "_dp")
os.makedirs(OUT, exist_ok=True)
counter = [0]


def render(order):
    return TEMPLATE.replace("/*DECLS*/", "\n".join(INDENT + d for d in order)).replace("// USA: func_", "// SCRATCH-USA: func_")


def score(order):
    counter[0] += 1
    path = os.path.join(OUT, "c%d_%d.cpp" % (os.getpid(), counter[0]))
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(render(order))
    p = subprocess.run([sys.executable, os.path.join(SP, "wdiff.py"), MOD, ADDR, path],
                       capture_output=True, text=True, cwd=REPO)
    os.remove(path)
    head = (p.stdout or "").strip().splitlines()[:1]
    head = head[0] if head else ""
    if head.startswith("MATCH"):
        return 0
    if head.startswith("BYTEDIFF"):
        return int(head.split()[1])
    return 10 ** 6


def neighbours(order):
    n = len(order)
    out = []
    for i in range(n - 1):
        o = list(order)
        o[i], o[i + 1] = o[i + 1], o[i]
        out.append(o)
    for i in range(1, n):
        o = list(order)
        x = o.pop(i)
        out.append([x] + o)
    for i in range(n - 1):
        o = list(order)
        x = o.pop(i)
        out.append(o + [x])
    return out


best = list(decls)
best_s = score(best)
print("base", best_s, flush=True)
seen = {tuple(best)}
with ThreadPoolExecutor(int(os.environ.get("DP_JOBS", "8"))) as pool:
    while best_s:
        cands = [c for c in neighbours(best) if tuple(c) not in seen]
        for c in cands:
            seen.add(tuple(c))
        if not cands:
            break
        scores = list(pool.map(score, cands))
        i = min(range(len(cands)), key=lambda k: scores[k])
        if scores[i] >= best_s:
            ties = [cands[k] for k in range(len(cands)) if scores[k] == best_s]
            print("plateau at", best_s, "ties", len(ties), flush=True)
            break
        best, best_s = cands[i], scores[i]
        print("step", best_s, json.dumps(best), flush=True)
        with open(os.path.join(HERE, "declperm_best.cpp"), "w", encoding="utf-8", newline="\n") as fh:
            fh.write(render(best).replace("// SCRATCH-USA: func_", "// USA: func_"))
print("final", best_s, json.dumps(best), flush=True)
