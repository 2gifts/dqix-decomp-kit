"""Fold c04's five low-frame locals into ONE struct laid out at the ROM's offsets.

mwcc ignores both declaration order and block scope when it places these (measured: an
exhaustive permutation and a hoist-to-case-local both moved nothing), so the only way to
reach the ROM's packing is to make the relative offsets a property of a single type.

    sp+0x0c p1   0x10 frame.word0   0x14/16/18 frame.h4..h8
    sp+0x1a d5Local   0x20 fb   0x30 packed

frame and d5Local share one struct because sizeof(SharedFrame) rounds to 12 and the ROM
puts d5Local at frame+10, inside that padding.

Usage: python lowregion.py <src> <out>
"""
import re
import sys

DECL = """    struct PackedRegSlots7f packed;
    struct FrameB fb;
    struct { unsigned char f0, f1; short f2; unsigned char f3, f4; } d5Local;
    struct SharedFrame frame;
    short p1;
"""

NEW = """    struct LowRegion {
        short p1;
        short pad2;
        unsigned int fword0;
        unsigned short fh4, fh6, fh8;
        unsigned char d0, d1;
        short d2;
        unsigned char d3, d4;
        struct FrameB fb;
        struct PackedRegSlots7f packed;
    } lr;
"""

SUBS = [
    ("&packed", "&lr.packed"),
    ("packed.", "lr.packed."),
    ("fb.", "lr.fb."),
    ("&frame", "(void *)&lr.fword0"),
    ("frame.word0", "lr.fword0"),
    ("frame.h4", "lr.fh4"),
    ("frame.h6", "lr.fh6"),
    ("frame.h8", "lr.fh8"),
    ("&d5Local", "(char *)&lr.d0"),
    ("d5Local.f0", "lr.d0"),
    ("d5Local.f1", "lr.d1"),
    ("d5Local.f2", "lr.d2"),
    ("d5Local.f3", "lr.d3"),
    ("d5Local.f4", "lr.d4"),
    ("&p1,", "&lr.p1,"),
    ("        p1 = msg->p1;", "        lr.p1 = msg->p1;"),
]

src, out = sys.argv[1:3]
s = open(src, encoding="utf-8").read()
if DECL not in s:
    sys.exit("declaration block not found verbatim")
head, tail = s.split(DECL, 1)
for a, b in SUBS:
    tail = tail.replace(a, b)
tail = re.sub(r"\blr\.lr\.", "lr.", tail)
open(out, "w", encoding="utf-8").write(head + NEW + tail)
print("wrote", out)
