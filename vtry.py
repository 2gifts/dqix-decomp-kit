#!/usr/bin/env python3
"""Try a list of textual substitutions against one function and report byte diffs.

Usage: vtry.py <module> <addr> <base.cpp> <variants.py>

The variants file must define ANCHOR (text present in base.cpp) and
VARIANTS (dict of name -> replacement text).
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
import os
import subprocess
import sys

SP = _kp.SP
KIT = _kp.KIT
ROOT = _kp.REPO


def main():
    module, addr, base_path, variants_path = sys.argv[1:5]
    base = open(base_path, encoding="utf-8").read()

    ns = {}
    exec(open(variants_path, encoding="utf-8").read(), ns)
    anchor = ns["ANCHOR"]
    variants = ns["VARIANTS"]

    if anchor not in base:
        print("ANCHOR not found in base file")
        return 1

    results = []
    for name, repl in variants.items():
        path = os.path.join(SP, "_vt_%s.cpp" % name)
        # Defuse the `// USA:` tag: ov_recover.gather() would otherwise treat these
        # intermediate variants as real wave candidates (see colorsweep.score).
        with open(path, "w", encoding="utf-8", newline="\n") as fh:
            fh.write(base.replace(anchor, repl, 1).replace("// USA: func_", "// SCRATCH-USA: func_"))
        proc = subprocess.run(
            [sys.executable, os.path.join(KIT, "wdiff.py"), module, addr, path],
            capture_output=True, text=True, cwd=ROOT,
        )
        out = (proc.stdout or "").strip().splitlines()
        head = out[0] if out else "(no output)"
        nbytes = 10 ** 6
        if head.startswith("BYTEDIFF"):
            nbytes = int(head.split()[1])
        elif head.startswith("MATCH"):
            nbytes = 0
        results.append((nbytes, name, head, out[1:6]))

    results.sort()
    for nbytes, name, head, detail in results:
        print("%-22s %s" % (name, head))
        if nbytes and nbytes < 10 ** 6:
            for line in detail:
                print("      " + line)
    best = results[0]
    print("\nBEST %s -> %s" % (best[1], best[2]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
