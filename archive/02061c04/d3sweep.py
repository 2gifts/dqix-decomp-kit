"""Brute-force the SOURCE axis for 02061c04 case 0xd3's scratch pair.

Build, flags and pragmas are exhausted (24 mwccarm builds, 29 flag sets, 40 pragmas -- none moves
it). This emits every combination of the syntactic choices available for a read-modify-write of one
word at base+0x1840+0xb4c into ONE translation unit, so the whole grid costs a single compile, and
reports which variants put the ADDRESS in r1 (the ROM's choice) rather than r2.

Usage: MWCC=2.0/sp2p2 python d3sweep.py
"""
import itertools
import os
import re
import subprocess
import sys

import capstone
from elftools.elf.elffile import ELFFile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import casediff

BITS = ["int bit = m->p1;", "unsigned int bit = m->p1;", "int bit = (int)m->p1;",
        "short bit = m->p1;"]
ONES = ["int one = 1;", "unsigned int one = 1;", "const int one = 1;", "volatile int one = 1;"]
ADDR = [
    "char *base = (char *)p0 + 0x1840;\n    unsigned int *w = (unsigned int *)(base + 0xb4c);",
    "unsigned int *w = (unsigned int *)((char *)p0 + 0x1840 + 0xb4c);",
    "unsigned int *w = (unsigned int *)&((char *)p0)[0x1840 + 0xb4c];",
    "unsigned int *w = (unsigned int *)((char *)p0 + 0x1840) + 0xb4c / 4;",
]
RMW = [
    "*w = *w | (one << bit);",
    "*w |= one << bit;",
    "*w = (one << bit) | *w;",
    "{ unsigned int v = *w; *w = v | (one << bit); }",
]
RET = ["return one;", "return 1;"]

parts = ["#pragma opt_propagation off\n",
         "struct Msg { unsigned short cmd, p1; };\n",
         'extern "C" void *GetP(void);\n']
names = []
for i, (b, o, a, r, t) in enumerate(itertools.product(BITS, ONES, ADDR, RMW, RET)):
    n = "v%03d" % i
    names.append(n)
    parts.append('extern "C" int %s(Msg *m)\n{\n    %s\n    %s\n    void *p0 = GetP();\n'
                 "    %s\n    %s\n    %s\n}\n" % (n, b, o, a, r, t))

src = os.path.join(HERE, "_d3sweep.cpp")
obj = os.path.join(HERE, "_d3sweep.o")
open(src, "w", encoding="utf-8").write("\n".join(parts))
r = subprocess.run([casediff.CC] + casediff.FLAGS + ["-c", src, "-o", obj],
                   capture_output=True, text=True, cwd=casediff.REPO)
if r.returncode:
    sys.exit(r.stdout[-3000:] + r.stderr[-3000:])

md = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_ARM)
hits = []
with open(obj, "rb") as fh:
    elf = ELFFile(fh)
    label = {}
    for sec in elf.iter_sections():
        if sec.header["sh_type"] != "SHT_SYMTAB":
            continue
        for s in sec.iter_symbols():
            if s.name.startswith("v") and re.fullmatch(r"v\d{3}", s.name):
                label[s["st_shndx"]] = s.name
    for i, sec in enumerate(elf.iter_sections()):
        if not sec.name.startswith(".text") or not sec.header.sh_size or i not in label:
            continue
        text = "\n".join("%s %s" % (x.mnemonic, x.op_str) for x in md.disasm(sec.data(), 0))
        m = re.search(r"add (r\d+), r0, #0x1840\n(?:.*\n)?ldr (r\d+), \[\1, #0xb4c\]", text)
        if m and m.group(1) == "r1":
            hits.append((label[i], len(sec.data())))

print("%d variants compiled, %d put the address in r1" % (len(names), len(hits)))
for n, ln in hits:
    print("  %s  len=0x%x" % (n, ln))
