"""Second brute-force grid for 02061c04 case 0xd3 -- the shapes the first 512 did not cover.

Grid one varied the cast style, the RMW spelling, the integer types and the return expression.
This one varies the LVALUE KIND (pointer, reference, struct member, bitfield, union member, array
element) and where the shift is computed, because the residue is which of the address and the loaded
value is allocated first, and that is an expression-tree property.

Usage: MWCC=2.0/sp2p2 python d3sweep2.py
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

PRELUDE = """#pragma opt_propagation off
struct Msg { unsigned short cmd, p1; };
extern "C" void *GetP(void);
struct Inner { unsigned int pad[0xb4c / 4]; unsigned int f; };
struct Outer { char pad[0x1840]; Inner in; };
union Uni { unsigned int w; unsigned char b[4]; };
struct Bits { unsigned int f; };
struct OuterU { char pad[0x1840]; char pad2[0xb4c]; Uni u; };
struct OuterArr { char pad[0x1840]; unsigned int a[0xb4c / 4 + 1]; };
"""

LVAL = [
    ("ptr", "unsigned int *w = (unsigned int *)((char *)p0 + 0x1840) + 0xb4c / 4;", "*w"),
    ("ref", "unsigned int &w = *(unsigned int *)((char *)((char *)p0 + 0x1840) + 0xb4c);", "w"),
    ("member", "Inner *ip = &((Outer *)p0)->in;", "ip->f"),
    ("memref", "Inner &ir = ((Outer *)p0)->in;", "ir.f"),
    ("union", "Uni *up = &((OuterU *)p0)->u;", "up->w"),
    ("unionref", "Uni &ur = ((OuterU *)p0)->u;", "ur.w"),
    ("arr", "unsigned int *ap = ((OuterArr *)p0)->a;", "ap[0xb4c / 4]"),
    ("arrref", "OuterArr &ar = *(OuterArr *)p0;", "ar.a[0xb4c / 4]"),
]

SHIFT = [
    ("inline", "{L} = {L} | (one << bit);"),
    ("compound", "{L} |= one << bit;"),
    ("pretmp", "unsigned int t = one << bit;\n    {L} = {L} | t;"),
    ("posttmp", "unsigned int v = {L};\n    v |= one << bit;\n    {L} = v;"),
    ("swapped", "{L} = (one << bit) | {L};"),
]

parts = [PRELUDE]
names = {}
for i, ((ln, decl, lv), (sn, tpl)) in enumerate(itertools.product(LVAL, SHIFT)):
    n = "w%03d" % i
    names[n] = "%s/%s" % (ln, sn)
    parts.append('extern "C" int %s(Msg *m)\n{\n    int bit = m->p1;\n    int one = 1;\n'
                 "    void *p0 = GetP();\n    %s\n    %s\n    return one;\n}\n"
                 % (n, decl, tpl.format(L=lv)))

src = os.path.join(HERE, "_d3sweep2.cpp")
obj = os.path.join(HERE, "_d3sweep2.o")
open(src, "w", encoding="utf-8").write("\n".join(parts))
r = subprocess.run([casediff.CC] + casediff.FLAGS + ["-c", src, "-o", obj],
                   capture_output=True, text=True, cwd=casediff.REPO)
if r.returncode:
    sys.exit(r.stdout[-4000:] + r.stderr[-4000:])

md = capstone.Cs(capstone.CS_ARCH_ARM, capstone.CS_MODE_ARM)
with open(obj, "rb") as fh:
    elf = ELFFile(fh)
    label = {}
    for sec in elf.iter_sections():
        if sec.header["sh_type"] != "SHT_SYMTAB":
            continue
        for s in sec.iter_symbols():
            if re.fullmatch(r"w\d{3}", s.name):
                label[s["st_shndx"]] = s.name
    hits = 0
    for i, sec in enumerate(elf.iter_sections()):
        if not sec.name.startswith(".text") or not sec.header.sh_size or i not in label:
            continue
        text = "\n".join("%s %s" % (x.mnemonic, x.op_str) for x in md.disasm(sec.data(), 0))
        m = re.search(r"add (r\d+), r0, #0x1840", text)
        if not m:
            continue
        base = m.group(1)
        if re.search(r"ldr r\d+, \[%s, #0xb4c\]" % base, text):
            mark = "  <-- ROM shape" if base == "r1" else ""
            print("%-12s %-16s add %s  len=0x%x%s"
                  % (label[i], names[label[i]], base, len(sec.data()), mark))
            hits += base == "r1"
print("%d variant(s) reached the ROM's r1" % hits)
