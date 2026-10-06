"""Bind case 0xe7's snapshot pointer as a C++ REFERENCE instead of a pointer.

mwcc folds `snap = rec + 0xcc` into every addressing mode, so the ROM's
`add r6, r4, #0xcc` never appears and case 0xe7 comes out 4 bytes short. A reference is
the one binding form that is not a pointer variable the address-mode selector can fold.

Usage: python e7ref.py <src> <out>
"""
import sys

OLD = "        struct SnapshotE7 *snap = (struct SnapshotE7 *)(rec + 0xcc);"
NEW = "        struct SnapshotE7 &snap = *(struct SnapshotE7 *)(rec + 0xcc);"

src, out = sys.argv[1:3]
s = open(src, encoding="utf-8").read()
if s.count(OLD) != 1:
    sys.exit("anchor not found")
i = s.index(OLD)
j = s.index("    case 0xe8:", i)
body = s[i:j].replace(OLD, NEW).replace("snap->", "snap.")
body = body.replace("(char *)snap + 4", "(char *)&snap + 4").replace("(snap,", "(&snap,")
open(out, "w", encoding="utf-8").write(s[:i] + body + s[j:])
print("wrote", out)
