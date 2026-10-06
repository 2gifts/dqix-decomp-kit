"""How many unmatched functions a module still has. One number, on stdout.

`run_module.sh` uses this to tell a DRAINED module from one that merely had a weak wave: a deep
pool means cool down and come back, a shallow one means genuinely done. The script reads the number
with `2>/dev/null`, so when this file went missing the value silently became 0 and every module was
declared drained after its first weak wave -- the exact failure its own comment warns about
("stopping on raw yield abandoned a module with 436 unmatched").

Usage: python poolsize.py <module>      # module is `main` or a 3-digit overlay, e.g. 031
"""
import sys

import claim

if len(sys.argv) < 2:
    sys.exit("usage: poolsize.py <module>")

mod = sys.argv[1]
mod = "main" if mod == "main" else mod.lstrip("o").lstrip("v").zfill(3)
try:
    print(len(claim.unmatched(mod)))
except Exception as exc:                      # never let a counter break a wave
    print(0)
    print(f"poolsize: {exc}", file=sys.stderr)
