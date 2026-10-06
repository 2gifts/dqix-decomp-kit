"""Permute the local DECLARATIONS inside one case of 02061c04, scored by pad/casescore.py.

`colorsweep`'s declaration rules work across the whole file and spread the budget thin; when one
case carries most of the register residue (02061c04's 0xe7 carries 42 of 67), the useful search is
the permutation of THAT case's declarations. Two declarations are swapped only when neither mentions
the other's name, so dependencies cannot be broken.

Usage: python caseperm.py <src.cpp> <case-hex> [--out best.cpp] [--depth N]
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casescore

SRC, CASE = sys.argv[1], sys.argv[2].lower().removeprefix("0x")
OUT = os.path.splitext(SRC)[0] + ".caseperm.cpp"
DEPTH = 6
for i, a in enumerate(sys.argv):
    if a == "--out":
        OUT = sys.argv[i + 1]
    if a == "--depth":
        DEPTH = int(sys.argv[i + 1])

lines = open(SRC, encoding="utf-8").read().split("\n")
s = next(i for i, l in enumerate(lines) if l.startswith("    case 0x%s: {" % CASE))
e = next(i for i, l in enumerate(lines) if i > s and re.match(r"^    (case 0x|default:)", l))

DECL = re.compile(r"^(\s+)(?:struct |union |unsigned |signed |const )*[\w:*]+\s*\*?\s*(\w+)\s*=\s*[^;]+;$")
decls = [(i, DECL.match(lines[i])) for i in range(s + 1, e)]
decls = [(i, m.group(2)) for i, m in decls if m]
print("case 0x%s declarations: %s" % (CASE, " ".join(n for _i, n in decls)), flush=True)


def probe(order):
    new = list(lines)
    for (i, _n), (j, _m) in zip(decls, order):
        new[i] = lines[j]
    path = os.path.join(casescore.casediff.SP, "_caseperm_probe.cpp")
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write("\n".join(new).replace("// USA: func_", "// SCRATCH-USA: func_"))
    return casescore.score(path), new


cur = list(decls)
(best, detail), _ = probe(cur)
print("base %d  %s" % (best, detail), flush=True)
for _round in range(DEPTH):
    moved = False
    for k in range(len(cur) - 1):
        a, b = cur[k], cur[k + 1]
        # only swap independent declarations
        if b[1] in lines[a[0]] or a[1] in lines[b[0]]:
            continue
        cand = list(cur)
        cand[k], cand[k + 1] = cand[k + 1], cand[k]
        (n, d), new = probe(cand)
        if n < best:
            best, cur, moved = n, cand, True
            print("  swap %s/%s -> %d  %s" % (a[1], b[1], n, d), flush=True)
            with open(OUT, "w", encoding="utf-8", newline="\n") as fh:
                fh.write("\n".join(new))
            break
    if not moved:
        break
print("RESULT %d   order: %s" % (best, " ".join(n for _i, n in cur)), flush=True)
