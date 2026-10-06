"""Print the RAW ROM words of one 02061c04 case beside ours, with the encodings.

Everything downstream reads capstone's rendering; when a residue survives every source form, build,
flag and pragma, the next thing to doubt is the rendering itself.

Usage: python romwords.py <case-hex> [ABS src]
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff

case = int(sys.argv[1], 16)
rom = casediff.rom_text()
rb = casediff.bodies(rom)
off, ln = rb[case]
ours = casediff.our_text(sys.argv[2]) if len(sys.argv) > 2 else None
ob = casediff.bodies(ours) if ours else None

print("case 0x%x  ROM +0x%04x  %d bytes" % (case, off, ln))
for i in range(off, off + ln, 4):
    w = int.from_bytes(rom[i:i + 4], "little")
    line = "  +0x%04x  %08x" % (i - off, w)
    if ob and case in ob:
        oo = ob[case][0]
        ow = int.from_bytes(ours[oo + (i - off):oo + (i - off) + 4], "little")
        line += "   ours %08x%s" % (ow, "" if ow == w else "   <-- differs")
    for ins in casediff.MD.disasm(rom[i:i + 4], 0):
        line += "   %s %s" % (ins.mnemonic, ins.op_str)
    print(line)
