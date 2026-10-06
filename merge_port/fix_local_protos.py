import collections
import re
import sys

LOG = sys.argv[1]
SYM = sys.argv[2]
NO_CAST = set(sys.argv[3].split(",")) if len(sys.argv) > 3 else set()

if LOG == "-":
    files = sys.argv[4:]
else:
    files = sorted(set(
        m.group(1) for m in re.finditer(
            r"^([^\s:]+\.(?:c|cpp)):\d+: illegal overloading '" + re.escape(SYM) + r"\(",
            open(LOG, encoding="utf-8", errors="replace").read(), re.M)))

PROTO = re.compile(
    r"^[ \t]*(?:(?:extern\s+\"C\"|extern|ARM|THUMB|static)\s+)*"
    r"(?P<ret>(?:(?:const|volatile|unsigned|signed|struct|class)\s+)*[A-Za-z_]\w*(?:\s*\*+)?)\s*"
    + re.escape(SYM) + r"\s*\((?P<params>[^()]*)\)\s*;[ \t]*(?://[^\r\n]*)?\r?\n",
    re.M)
CALL = re.compile(r"(?<![\w.>:])" + re.escape(SYM) + r"\s*\(")


def find_close(text, i):
    depth = 0
    for j in range(i, len(text)):
        if text[j] == "(":
            depth += 1
        elif text[j] == ")":
            depth -= 1
            if depth == 0:
                return j
    return -1


stats = collections.Counter()
for path in files:
    path = path.replace("\\", "/")
    text = open(path, encoding="utf-8", errors="surrogateescape", newline="").read()
    rets = [" ".join(m.group("ret").split()) for m in PROTO.finditer(text)]
    if not rets:
        print("NO PROTOTYPE FOUND: " + path)
        continue
    ret = rets[0]
    text = PROTO.sub("", text)
    stats["prototypes"] += len(rets)
    if ret.replace(" ", "") not in NO_CAST:
        pos = 0
        while True:
            m = CALL.search(text, pos)
            if not m:
                break
            close = find_close(text, m.end() - 1)
            call = text[m.start():close + 1]
            new = "((%s)%s)" % (ret, call)
            text = text[:m.start()] + new + text[close + 1:]
            pos = m.start() + len(new)
            stats["casts"] += 1
    stats["ret " + ret] += 1
    open(path, "w", encoding="utf-8", errors="surrogateescape", newline="").write(text)

print("files: %d" % len(files))
for k, v in sorted(stats.items()):
    print("  %-40s %d" % (k, v))
