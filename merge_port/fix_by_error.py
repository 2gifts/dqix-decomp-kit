import collections
import re
import sys

LOG, IDENT, PATTERN, REPL = sys.argv[1:5]

sites = collections.defaultdict(set)
for m in re.finditer(r"^([^\s:]+\.(?:c|cpp|h)):(\d+): undefined identifier '" + re.escape(IDENT) + "'",
                     open(LOG, encoding="utf-8", errors="replace").read(), re.M):
    sites[m.group(1).replace("\\", "/")].add(int(m.group(2)))

pat = re.compile(PATTERN)
changed = missed = 0
for path, lines in sorted(sites.items()):
    text = open(path, encoding="utf-8", errors="surrogateescape", newline="").read()
    rows = text.split("\n")
    for n in sorted(lines):
        new, c = pat.subn(REPL, rows[n - 1])
        if c:
            rows[n - 1] = new
            changed += c
        else:
            missed += 1
            print("MISS %s:%d: %s" % (path, n, rows[n - 1].strip()[:120]))
    open(path, "w", encoding="utf-8", errors="surrogateescape", newline="").write("\n".join(rows))
print("%s: %d files, %d replacements, %d lines missed" % (IDENT, len(sites), changed, missed))
