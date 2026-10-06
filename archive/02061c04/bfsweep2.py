"""Faithful sweep of 02061c04 case 0xbf's copy loop -- the whole case body, not just the loop.

bfsweep.py found six shapes that give the ROM's src=r3/dst=r2, but applying the best of them to the
real function made 0xbf WORSE (4 bytes of residue to 7): in the real case `count` stays live through
a five-iteration tail with a memmove, so the loop's registers are chosen under different pressure.
This probe carries that tail, and the frame is padded so `deck` lands at a non-zero sp offset the
way it does in the real function.

Usage: MWCC=2.0/sp2p2 python bfsweep2.py
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
extern "C" void *GetGlobal(void);
extern "C" void *FindEntry(void *, int);
extern "C" void *GetRandom(void);
extern "C" int NextRandomMax(void *, int);
extern "C" void *memmove(void *, const void *, unsigned int);
struct Msg { unsigned short cmd, p1; };
"""

DECL = [
    ("src_first", "unsigned short *src = (unsigned short *)data_src;\n"
                  "    unsigned short *dst = deck;"),
    ("dst_first", "unsigned short *dst = deck;\n"
                  "    unsigned short *src = (unsigned short *)data_src;"),
    ("src_const", "const unsigned short *src = (const unsigned short *)data_src;\n"
                  "    unsigned short *dst = deck;"),
    ("dst_first_const", "unsigned short *dst = deck;\n"
                        "    const unsigned short *src = (const unsigned short *)data_src;"),
]

LOOP = [
    ("postinc", "do { *dst++ = *src++; } while (--n);"),
    ("preinc", "do { *dst = *src; ++src; ++dst; } while (--n);"),
    ("preinc_dst", "do { *dst = *src; ++dst; ++src; } while (--n);"),
    ("whiledec", "while (n--) *dst++ = *src++;"),
    ("temp", "do { unsigned short t = *src++; *dst++ = t; } while (--n);"),
    ("forcount", "for (int i = 0; i < n; i++) dst[i] = src[i];"),
]

NDECL = [
    ("n_after", "int n = count;"),
    ("n_before", ""),
]

TAIL = """    void *entry = FindEntry(GetGlobal(), m->p1);
    if (!entry)
        return 0;
    for (int j = 0; j < 5; j++) {
        int idx = NextRandomMax(GetRandom(), count);
        *(short *)((char *)entry + j * 2 + 4) = deck[idx];
        if (idx < count - 1)
            memmove(&deck[idx], &deck[idx + 1], count - (idx + 1));
        count--;
        deck[count] = 0;
    }
    return 1;
"""

parts = [PRELUDE]
names = {}
for i, ((dn, decl), (ln, loop), (nn, ndecl)) in enumerate(itertools.product(DECL, LOOP, NDECL)):
    n = "c%03d" % i
    names[n] = "%s/%s/%s" % (dn, ln, nn)
    head = "    unsigned short count = 14;\n"
    if nn == "n_before":
        head += "    int n = 14;\n    %s\n" % decl
    else:
        head += "    %s\n    %s\n" % (decl, ndecl)
    parts.append('extern "C" int %s(Msg *m)\n{\n    char pad[0x84];\n'
                 "    unsigned short deck[14];\n%s    %s\n%s    (void)pad;\n}\n"
                 % (n, head, loop, TAIL))

src = os.path.join(HERE, "_bfsweep2.cpp")
obj = os.path.join(HERE, "_bfsweep2.o")
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
            if re.fullmatch(r"c\d{3}", s.name):
                label[s["st_shndx"]] = s.name
    for i, sec in enumerate(elf.iter_sections()):
        if not sec.name.startswith(".text") or not sec.header.sh_size or i not in label:
            continue
        text = "\n".join("%s %s" % (x.mnemonic, x.op_str) for x in md.disasm(sec.data(), 0))
        ld = re.search(r"ldrh r0, \[(r\d+)\], #2", text)
        st = re.search(r"strh r0, \[(r\d+)\], #2", text)
        cnt = re.search(r"mov (r\d+), #0xe", text)
        nn = re.search(r"subs (r\d+), \1, #1", text)
        if not (ld and st):
            continue
        rom = (ld.group(1) == "r3" and st.group(1) == "r2"
               and cnt and cnt.group(1) == "r5" and nn and nn.group(1) == "r1")
        if rom:
            print("%-6s %-34s ROM SHAPE (src=r3 dst=r2 count=r5 n=r1)" % (label[i], names[label[i]]))
