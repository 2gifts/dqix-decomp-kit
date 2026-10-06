"""Brute-force the loop shape for 02061c04 case 0xbf's copy.

    ROM:  ldr r3,[pc] (src) / add r2,sp,#0x84 (dst) / ldrh r0,[r3],#2 / strh r0,[r2],#2
    ours: the same with src=r2 and dst=r3.

Register naming is per-case (pad/coupling.py: ten perturbations elsewhere in the function move
nothing), so the shape of THIS loop is what decides it. Every combination goes into one translation
unit and costs one compile.

Usage: MWCC=2.0/sp2p2 python bfsweep.py
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
extern "C" short data_src[14];
extern "C" void Sink(void *);
"""

DECL = [
    ("src_first", "unsigned short *src = (unsigned short *)data_src;\n"
                  "    unsigned short *dst = deck;"),
    ("dst_first", "unsigned short *dst = deck;\n"
                  "    unsigned short *src = (unsigned short *)data_src;"),
    ("src_const", "const unsigned short *src = (const unsigned short *)data_src;\n"
                  "    unsigned short *dst = deck;"),
    ("dst_const_src", "unsigned short *dst = deck;\n"
                      "    const unsigned short *src = (const unsigned short *)data_src;"),
    ("src_short", "short *src = data_src;\n"
                  "    short *dst = (short *)deck;"),
]

LOOP = [
    ("postinc", "do { *dst++ = *src++; } while (--n);"),
    ("preinc", "do { *dst = *src; ++src; ++dst; } while (--n);"),
    ("whiledec", "while (n--) *dst++ = *src++;"),
    ("forcount", "for (int i = 0; i < n; i++) dst[i] = src[i];"),
    ("temp", "do { unsigned short t = *src++; *dst++ = t; } while (--n);"),
    ("endptr", "__typeof__(src) end = src + n;\n    do { *dst++ = *src++; } while (src != end);"),
]

COUNT = [
    ("n_from_count", "unsigned short count = 14;\n    int n = count;"),
    ("n_literal", "int n = 14;"),
    ("n_int_count", "int count = 14;\n    int n = count;"),
]

parts = [PRELUDE]
names = {}
for i, ((dn, decl), (ln, loop), (cn, cnt)) in enumerate(itertools.product(DECL, LOOP, COUNT)):
    n = "b%03d" % i
    names[n] = "%s/%s/%s" % (dn, ln, cn)
    body = loop
    if "short *src" in decl:
        body = body.replace("unsigned short t", "short t")
    parts.append('extern "C" int %s(void)\n{\n    unsigned short deck[14];\n    %s\n    %s\n'
                 "    %s\n    Sink(deck);\n    return 1;\n}\n" % (n, cnt, decl, body))

src = os.path.join(HERE, "_bfsweep.cpp")
obj = os.path.join(HERE, "_bfsweep.o")
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
            if re.fullmatch(r"b\d{3}", s.name):
                label[s["st_shndx"]] = s.name
    hits = 0
    for i, sec in enumerate(elf.iter_sections()):
        if not sec.name.startswith(".text") or not sec.header.sh_size or i not in label:
            continue
        text = "\n".join("%s %s" % (x.mnemonic, x.op_str) for x in md.disasm(sec.data(), 0))
        ld = re.search(r"ldrh r0, \[(r\d+)\], #2", text)
        st = re.search(r"strh r0, \[(r\d+)\], #2", text)
        if not ld or not st:
            continue
        rom = ld.group(1) == "r3" and st.group(1) == "r2"
        if rom:
            hits += 1
            print("%-6s %-34s src=%s dst=%s  len=0x%x  <-- ROM shape"
                  % (label[i], names[label[i]], ld.group(1), st.group(1), len(sec.data())))
print("%d of %d variant(s) reached the ROM's src=r3 dst=r2" % (hits, len(names)))
