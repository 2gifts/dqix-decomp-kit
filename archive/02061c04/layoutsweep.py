"""Hill-climb the function-scope declaration ORDER of 02061c04 against pad/casescore.py.

mwcc's stack layout is decided by the declaration list, and reading its rule off the target's
addresses failed: within one case (0x7f) two locals land at the top of the frame and a third at the
bottom, so it is neither declaration order nor clause order nor size. It does not have to be
derived. The order is a permutation of a 15-line block, the objective is a number, and one
evaluation is a single compile — so search it.

Neighbours are: every adjacent transposition, plus moving any one declaration to the front or the
back. That reaches any permutation eventually and keeps a round to ~45 evaluations.

Usage: python layoutsweep.py <src.cpp> [--rounds N] [--out best.cpp]

Writes the best source it finds to --out (default <src>.layout.cpp) and prints every improvement, so
a killed run still leaves its progress on disk.
"""
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casescore

SRC = sys.argv[1]
ROUNDS = 8
OUT = os.path.splitext(SRC)[0] + ".layout.cpp"
for i, a in enumerate(sys.argv):
    if a == "--rounds":
        ROUNDS = int(sys.argv[i + 1])
    if a == "--out":
        OUT = sys.argv[i + 1]

text = open(SRC, encoding="utf-8").read()
lines = text.split("\n")
head = next(i for i, l in enumerate(lines) if "ARM int func_02061c04" in l)
start = head + 1
end = next(i for i, l in enumerate(lines) if i > start and l.strip().startswith("switch ("))
BLOCK = lines[start:end]
assert all(l.strip().endswith(";") for l in BLOCK if l.strip()), BLOCK


def build(order):
    return "\n".join(lines[:start] + [BLOCK[i] for i in order] + lines[end:])


def name(i):
    m = re.search(r"(\w+)(?:\[[^\]]*\])?;\s*$", BLOCK[i])
    return m.group(1) if m else "?"


def neighbours(order):
    out = []
    n = len(order)
    for i in range(n - 1):
        o = list(order)
        o[i], o[i + 1] = o[i + 1], o[i]
        out.append(("swap %s/%s" % (name(o[i]), name(o[i + 1])), o))
    for i in range(n):
        for pos, tag in ((0, "front"), (n - 1, "back")):
            if i == pos:
                continue
            o = [x for j, x in enumerate(order) if j != i]
            o.insert(pos, order[i])
            out.append(("%s to %s" % (name(order[i]), tag), o))
    return out


def probe(order):
    path = os.path.join(casescore.casediff.SP, "_layout_probe.cpp")
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(build(order).replace("// USA: func_", "// SCRATCH-USA: func_"))
    return casescore.score(path)


ONLY = None
for i, a in enumerate(sys.argv):
    if a == "--only":
        ONLY = sys.argv[i + 1].split(",")

if ONLY:
    # EXHAUSTIVE over a named subset. Adjacent-transposition hill-climbing stalls as soon as no
    # single swap improves, and the last few stack shifts are a permutation of one small group of
    # objects -- 5! is 120 compiles, which is cheaper than another stalled climb.
    import itertools
    idx = [i for i in range(len(BLOCK)) if name(i) in ONLY]
    assert len(idx) == len(ONLY), (ONLY, [name(i) for i in idx])
    base = list(range(len(BLOCK)))
    best, detail = probe(base)
    print("base %d  %s   permuting %s" % (best, detail, " ".join(name(i) for i in idx)), flush=True)
    for perm in itertools.permutations(idx):
        cand = list(base)
        for slot, val in zip(idx, perm):
            cand[slot] = val
        n, d = probe(cand)
        if n < best:
            best = n
            print("  %-40s -> %d  %s" % (" ".join(name(i) for i in perm), n, d), flush=True)
            with open(OUT, "w", encoding="utf-8", newline="\n") as fh:
                fh.write(build(cand))
    print("RESULT %d" % best, flush=True)
    sys.exit(0)

cur = list(range(len(BLOCK)))
best, detail = probe(cur)
print("base %d  %s" % (best, detail), flush=True)
for rnd in range(ROUNDS):
    moved = False
    for label, cand in neighbours(cur):
        n, d = probe(cand)
        if n < best:
            best, cur, moved = n, cand, True
            print("  %-24s -> %d  %s" % (label, n, d), flush=True)
            with open(OUT, "w", encoding="utf-8", newline="\n") as fh:
                fh.write(build(cur))
            break
    if not moved:
        print("round %d: no improving neighbour" % rnd, flush=True)
        break
print("RESULT %d   order: %s" % (best, " ".join(name(i) for i in cur)), flush=True)
