"""Turn case 0xe7's guarded block into an early return, flattening the body out of the nested scope.

pad/probe_alloc.cpp showed that a long-lived value defined in a NESTED block flips mwcc's whole
callee-saved order from reverse to forward. Every 0xe7 grid so far kept the body inside
`if (!snap->done) { ... }`; this is the one shape that removes the nesting while emitting the same
branch.

Usage: python e7flat.py <src> <out>
"""
import re
import sys

src, out = sys.argv[1:3]
s = open(src, encoding="utf-8").read()
i = s.index("    case 0xe7: {")
j = s.index("    case 0xe8:", i)
body = s[i:j]

if "if (!snap->done) {" not in body:
    sys.exit("guard not found")
body = body.replace("        if (!snap->done) {\n", "        if (snap->done)\n            return 1;\n")

# drop one level of indentation from the guarded lines, and remove the closing brace that used to
# end the guard (the last `        }` before `        return 1;`)
lines = body.split("\n")
outl = []
for ln in lines:
    outl.append(ln)
text = "\n".join(outl)
text = re.sub(r"\n        \}\n        return 1;\n    \}\n$", "\n        return 1;\n    }\n", text)
if text == body:
    sys.exit("closing brace of the guard not found")
open(out, "w", encoding="utf-8").write(s[:i] + text + s[j:])
print("wrote", out)
