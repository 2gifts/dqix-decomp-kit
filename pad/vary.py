"""Make a variant of a source by replacing the text in FROM_FILE with the text in TO_FILE.

Usage: python vary.py <src> <out> <from_file> <to_file>

The replacement must occur exactly once; anything else is an error rather than a silent
half-edit. Written because a shell heredoc mangles C source and a failed replace looks
like a codegen result.
"""
import sys

src, out, ffile, tfile = sys.argv[1:5]
s = open(src, encoding="utf-8").read()
a = open(ffile, encoding="utf-8").read().rstrip("\n")
b = open(tfile, encoding="utf-8").read().rstrip("\n")
n = s.count(a)
if n != 1:
    sys.exit("replace pattern occurs %d times, expected 1" % n)
open(out, "w", encoding="utf-8").write(s.replace(a, b))
print("wrote", out)
