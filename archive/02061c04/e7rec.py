"""Rename case 0xe7's local `rec` to the function-scope `e7rec`.

mwcc gives callee-saved registers in DECLARATION order, and `rec` cannot be declared before
`battle` inside the case because it is derived from it. A function-scope declaration is declared
before every case-local, which is the only way to put `rec` ahead of `battle` in the ladder.

Usage: python e7rec.py <src> <out>
"""
import sys

src, out = sys.argv[1:3]
s = open(src, encoding="utf-8").read()
i = s.index("    case 0xe7: {")
j = s.index("    case 0xe8:", i)
body = s[i:j]
body = body.replace("        char *rec = e7base + 0x7400;", "        e7rec = e7base + 0x7400;")
body = body.replace("rec + snapOff", "e7rec + snapOff")
body = body.replace("rec + 0x3c", "e7rec + 0x3c").replace("rec + 0x40", "e7rec + 0x40")
body = body.replace("rec + 0x44", "e7rec + 0x44").replace("rec + 0x48", "e7rec + 0x48")
body = body.replace("rec + 0x4c", "e7rec + 0x4c").replace("rec + 0x7c", "e7rec + 0x7c")
if "char *rec" in body:
    sys.exit("a `rec` declaration survived: " + body[body.index("char *rec"):][:60])
open(out, "w", encoding="utf-8").write(s[:i] + body + s[j:])
print("wrote", out)
