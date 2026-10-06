"""Hoist every case-local of 02061c04 to function scope, in a chosen order.

The target's stack layout puts the two shared structs at the BOTTOM of the frame (sp+0x10, sp+0x20)
while ours puts them at the top, which costs an `add rX, sp, #imm` on every access in five cases.
Our build lays locals out first-declared-highest, so the only way to push a function-scope object
down is for every other local to be declared before it -- i.e. hoisted.

Usage: python hoist_c04.py <in.cpp> <out.cpp>
"""
import re
import sys

src, dst = sys.argv[1], sys.argv[2]
lines = open(src, encoding="utf-8").read().split("\n")

# (line-matching declaration text, new name, old name) in the order they should be declared.
HOIST = [
    ("        char local[0xb8];", "char cdLocal[0xb8];", "local"),
    ("        struct { int a0, a1, a2; } sa;", "struct { int a0, a1, a2; } sa;", None),
    ("        struct { int b0, b1, b2; } sb;", "struct { int b0, b1, b2; } sb;", None),
    ("        struct Src021cd6d8 local;", "struct Src021cd6d8 c0Local;", "local"),
    ("        unsigned short deck[14];", "unsigned short deck[14];", None),
    ("        struct { int a, b, c; } local;", "struct { int a, b, c; } d8Local;", "local"),
    ("        SafeAllocator local;", "SafeAllocator dbAlloc;", "local"),
    ("        struct { unsigned char f0, f1; short f2; unsigned char f3, f4; } local;",
     "struct { unsigned char f0, f1; short f2; unsigned char f3, f4; } d5Local;", "local"),
]


def clause_range(i):
    """The [start, end) line range of the `case` clause containing line i."""
    s = i
    while s >= 0 and not re.match(r"^    case 0x", lines[s]):
        s -= 1
    e = i + 1
    while e < len(lines) and not re.match(r"^    case 0x|^    default:", lines[e]):
        e += 1
    return s, e


hoisted = []
# The five CmdMsg64 locals are all spelled the same, so name them by their clause.
n = 0
for i, l in enumerate(list(lines)):
    if l.strip() == "CmdMsg64 local;":
        s, _ = clause_range(i)
        name = "m%s" % re.search(r"case (0x[0-9a-f]+)", lines[s]).group(1)[2:]
        HOIST.insert(3 + n, ("        CmdMsg64 local;@%d" % i, "CmdMsg64 %s;" % name, "local"))
        n += 1

for decl, newdecl, old in HOIST:
    at = None
    if "@" in decl:
        decl, at = decl.split("@")
        at = int(at)
    idx = at if at is not None else next(i for i, l in enumerate(lines) if l.strip() == decl.strip())
    s, e = clause_range(idx)
    new = re.match(r"\s*(?:struct|CmdMsg64|SafeAllocator|char|unsigned short)\s", newdecl)
    name = re.search(r"(\w+)(?:\[[^\]]*\])?;$", newdecl).group(1)
    if old:
        for j in range(s, e):
            if j != idx:
                lines[j] = re.sub(r"\b%s\b" % old, name, lines[j])
    lines[idx] = "@@DROP@@"
    hoisted.append("    " + newdecl)

lines = [l for l in lines if l != "@@DROP@@"]
head = next(i for i, l in enumerate(lines) if l == "    struct SharedFrame frame;")
lines[head:head] = hoisted
open(dst, "w", encoding="utf-8", newline="\n").write("\n".join(lines))
print("hoisted %d declarations" % len(hoisted))
