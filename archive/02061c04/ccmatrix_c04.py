"""Compile 02061c04 with every mwccarm build and report length, frame size and byte distance.

The function was size-exact under the compiler of the day but differed in the frame and the register
ladder; both are whole-compiler decisions, so the cheapest test of "is this the wrong compiler build"
is to run every build and compare rather than keep rewriting the source. That question is settled for
the project as a whole -- the ROM builds with 2.0/sp2p2 -- so re-measure before quoting a number here.

Usage: python ccmatrix_c04.py [src.cpp]
"""
import glob
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff
from elftools.elf.elffile import ELFFile

src = sys.argv[1] if len(sys.argv) > 1 else f"{casediff.SP}/c04work/c04.cpp"
rom = casediff.rom_text()
builds = sorted(glob.glob(f"{casediff.REPO}/tools/mwccarm/*/*/mwccarm.exe"))
print("%-16s %-8s %-8s %s" % ("build", "size", "frame", "bytes differing"))
for cc in builds:
    name = "/".join(cc.replace("\\", "/").split("/")[-3:-1])
    obj = f"{casediff.SP}/c04work/_cc_probe.o"
    r = subprocess.run([cc] + casediff.FLAGS + ["-c", src, "-o", obj],
                       capture_output=True, text=True, cwd=casediff.REPO)
    if r.returncode:
        print("%-16s FAILED  %s" % (name, (r.stdout + r.stderr).strip().splitlines()[-1][:60]))
        continue
    with open(obj, "rb") as fh:
        secs = [s.data() for s in ELFFile(fh).iter_sections() if s.name.startswith(".text")]
    text = max(secs, key=len)
    frame = "?"
    for ins in casediff.MD.disasm(text[:16], 0):
        if ins.mnemonic == "sub" and ins.op_str.startswith("sp, sp, #"):
            frame = ins.op_str.split("#")[1]
    diff = sum(1 for i in range(min(len(rom), len(text))) if rom[i] != text[i]) \
        + abs(len(rom) - len(text))
    print("%-16s 0x%-6x %-8s %d" % (name, len(text), frame, diff))
