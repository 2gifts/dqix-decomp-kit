#!/usr/bin/env python3
"""Re-check every staged `asm` file against HAND-written provenance only.

    python audit_asm.py <module> [--apply]

An `asm` block is only allowed to stay if a reference decompilation implements the SAME routine as
assembly *that a person wrote* -- `asm void` in a .c, or a .s with plain labels -- and our
instruction sequence matches it exactly. Splitter dumps no longer count as references, so files
justified by one have lost their evidence and must go back to being decompiled as C.

Without --apply this only reports. With it, unevidenced files move to _unevidenced/<module>/ where
they stay readable; nothing is deleted.
"""
import os
import re
import shutil
import sys

import sdkident

SP = os.path.dirname(os.path.abspath(__file__))


def main():
    mod = sys.argv[1] if len(sys.argv) > 1 else "main"
    apply = "--apply" in sys.argv
    stage = f"{SP}/staging/{mod}"
    out = f"{SP}/_unevidenced/{mod}"
    idx = sdkident.load_index()
    fns = {a: (a, s, n, i) for a, s, n, i in sdkident.our_functions(mod)}

    keep, drop, untagged = [], [], []
    for fn in sorted(os.listdir(stage)) if os.path.isdir(stage) else []:
        if not fn.endswith(".cpp"):
            continue
        path = f"{stage}/{fn}"
        text = open(path, encoding="utf-8", errors="replace").read()
        if not re.search(r"\basm\b", text):
            continue
        m = re.search(r"// USA: func_([0-9a-fA-F]{8})", text)
        if not m:
            untagged.append(fn)
            continue
        addr = int(m.group(1), 16)
        if addr not in fns:
            drop.append((fn, "address is not an unmatched function any more"))
            continue
        hits = sdkident.report(mod, *fns[addr], idx=idx, quiet=True)
        good = [h for h in hits if h["sim"] >= 0.999 and h.get("kind") in ("HAND", "C")]
        if good:
            keep.append((fn, good[0]))
        else:
            best = hits[0] if hits else None
            drop.append((fn, f"best is {best['kind']} {best['sim']*100:.0f}% {best['ref']}"
                              if best else "no reference implements it"))

    for fn, h in keep:
        print(f"KEEP    {fn:<44} {h['kind']} {h['ref']} <- {h['proj']}/{h['file']}")
    for fn, why in drop:
        print(f"PULL    {fn:<44} {why}")
    for fn in untagged:
        print(f"UNTAGGED {fn:<43} no // USA: tag -- integrator would drop it silently")

    if apply and (drop or untagged):
        os.makedirs(out, exist_ok=True)
        for fn, _ in drop:
            shutil.move(f"{stage}/{fn}", f"{out}/{fn}")
        for fn in untagged:
            shutil.move(f"{stage}/{fn}", f"{out}/{fn}")
        print(f"\nmoved {len(drop) + len(untagged)} to {out}")
    print(f"\n{len(keep)} evidenced, {len(drop)} unevidenced, {len(untagged)} untagged")


if __name__ == "__main__":
    main()
