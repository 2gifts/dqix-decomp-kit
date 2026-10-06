"""Exit 0 when the address lies inside some file's delinked range in any module, 1 otherwise.

    python delinked.py <addr_hex> [module]      module: main or NNN; default every module
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
import glob
import re
import sys

REPO = _kp.REPO
addr = int(sys.argv[1], 16)
mod = sys.argv[2] if len(sys.argv) > 2 else None
paths = glob.glob(f"{REPO}/config/usa/arm9/delinks.txt") + glob.glob(f"{REPO}/config/usa/arm9/overlays/*/delinks.txt")
if mod:
    paths = [p for p in paths if (mod == "main") == ("overlays" not in p) and (mod == "main" or f"ov{mod}" in p)]
for p in paths:
    for m in re.finditer(r"(?m)^\s*\.text\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)[ \t]*\r?$",
                         open(p, encoding="utf-8", errors="ignore").read()):
        if int(m.group(1), 16) <= addr < int(m.group(2), 16):
            sys.exit(0)
sys.exit(1)
