"""Score a candidate for 02061c04 by per-case aligned, noise-classified instruction diffs.

`wdiff` aligns the whole function, so on a 134-case switch one wrong-length body makes every later
instruction read as a diff and the score stops tracking progress. This aligns each case body against
its own jump-table entry and counts only the differences that are real work:

    reg    same instruction, different registers   -> colouring, fixable
    sp     same instruction, different sp offset   -> stack layout
    shape  anything else                           -> a wrong C construct

Relocation and literal-pool differences are excluded: they resolve at link time and counting them
buries the signal (533 of 617 rows on the current candidate).

Score = shape*4 + reg + sp + 64 per case whose body is the wrong LENGTH. A wrong-length body is
weighted because it also hides every diff inside it.

Usage: python casescore.py <file.cpp>     ->  prints "<score> shape=N reg=N sp=N unaligned=N"
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff
import fulldiff


def score(path):
    try:
        ours = casediff.our_text(path)
    except SystemExit:
        return 10 ** 6, "compile failed"
    rom = casediff.rom_text()
    rb, ob = casediff.bodies(rom), casediff.bodies(ours)
    counts = {"reg": 0, "sp": 0, "shape": 0}
    unaligned = 0
    for case in sorted(rb):
        ro, rl = rb[case]
        if case not in ob or ob[case][1] != rl:
            unaligned += 1
            continue
        oo = ob[case][0]
        a = list(casediff.MD.disasm(rom[ro:ro + rl], ro))
        b = list(casediff.MD.disasm(ours[oo:oo + rl], ro))
        for i in range(min(len(a), len(b))):
            x, y = a[i], b[i]
            if x.mnemonic == y.mnemonic and x.op_str == y.op_str:
                continue
            k = fulldiff.kind(x, y, ro, ro + rl)
            if k != "pool":
                counts[k] += 1
    total = counts["shape"] * 4 + counts["reg"] + counts["sp"] + unaligned * 64
    return total, "shape=%d reg=%d sp=%d unaligned=%d" % (
        counts["shape"], counts["reg"], counts["sp"], unaligned)


if __name__ == "__main__":
    n, detail = score(sys.argv[1])
    print("%d %s" % (n, detail))
