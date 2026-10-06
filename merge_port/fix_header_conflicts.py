import collections
import re
import sys

LOG = sys.argv[1]

HEADER = {
    "Vector3fix_Subtract": ("void", ["const Vector3fix*", "const Vector3fix*", "Vector3fix*"]),
    "Vector3fix_Add": ("void", ["const Vector3fix*", "const Vector3fix*", "Vector3fix*"]),
    "Vector3fix_Distance": ("fix32_t", ["const Vector3fix*", "const Vector3fix*"]),
    "Vector3fix_Normalize": ("void", ["const Vector3fix*", "Vector3fix*"]),
    "Vector3fix_Length": ("fix32_t", ["const Vector3fix*"]),
    "Vector3fix_InnerProduct": ("fix32_t", ["const Vector3fix*", "const Vector3fix*"]),
    "fix32_Divide": ("fix32_t", ["fix32_t", "fix32_t"]),
    "fix32_Atan2": ("fix32_t", ["fix32_t", "fix32_t"]),
    "sprintf": ("int", ["char*", "const char*"]),
    "memset": ("void*", ["void*", "int", "unsigned int"]),
    "Mat4x3_ApplyToVector": ("void", ["const Vector3fix*", "const Matrix4x3*", "Vector3fix*"]),
    "Mat3x3_WriteRotationY": ("void", ["Matrix3x3*", "fix32_t", "fix32_t"]),
    "Mat3x3_ApplyToVector": ("void", ["const Vector3fix*", "const Matrix3x3*", "Vector3fix*"]),
}
SAME_RET = {"fix32_t": "int"}
SIMPLE = re.compile(r"^[A-Za-z_]\w*(?:(?:->|\.)[A-Za-z_]\w*|\[[^\[\]]+\])*$|^&[A-Za-z_]\w*(?:(?:->|\.)[A-Za-z_]\w*|\[[^\[\]]+\])*$")


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


def split_args(inner):
    args, depth, cur = [], 0, []
    for ch in inner:
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == "," and depth == 0:
            args.append("".join(cur).strip())
            cur = []
        else:
            cur.append(ch)
    tail = "".join(cur).strip()
    if tail or args:
        args.append(tail)
    return args


def norm(t):
    t = " ".join(t.replace("*", " * ").split()).replace(" *", "*")
    t = re.sub(r"^(?:struct|class)\s+", "", t)
    return SAME_RET.get(t, t)


sites = collections.defaultdict(set)
log = open(LOG, encoding="utf-8", errors="replace").read()
for m in re.finditer(r"^([^\s:]+\.(?:c|cpp)):(\d+): illegal function overloading", log, re.M):
    sites[m.group(1).replace("\\", "/")].add(int(m.group(2)))

stats = collections.Counter()
for path, lines in sorted(sites.items()):
    text = open(path, encoding="utf-8", errors="surrogateescape", newline="").read()
    rows = text.split("\n")
    syms = {}
    for n in lines:
        row = rows[n - 1]
        found = [s for s in HEADER if re.search(r"\b%s\s*\(" % s, row)]
        if not found:
            print("UNKNOWN %s:%d: %s" % (path, n, row.strip()[:140]))
            continue
        s = found[0]
        m = re.match(r"^\s*(?:(?:extern\s+\"C\"|extern|ARM|THUMB|static)\s+)*(.*?)\b%s\s*\((.*)\)\s*;" % s, row)
        if not m:
            print("NOT A PROTOTYPE %s:%d: %s" % (path, n, row.strip()[:140]))
            continue
        local_params = [p for p in split_args(m.group(2)) if p and p != "void"]
        if len(local_params) != len(HEADER[s][1]) and s != "sprintf":
            print("ARITY %s:%d: %s takes %d here, %d in the header" % (path, n, s, len(local_params), len(HEADER[s][1])))
            continue
        syms[s] = m.group(1).strip()
        rows[n - 1] = None
    rows = [r for r in rows if r is not None]
    text = "\n".join(rows)
    for s, local_ret in syms.items():
        hret, hparams = HEADER[s]
        call = re.compile(r"(?<![\w.>:])%s\s*\(" % s)
        pos = 0
        while True:
            m = call.search(text, pos)
            if not m:
                break
            close = find_close(text, m.end() - 1)
            args = split_args(text[m.end():close])
            cast = []
            for i, a in enumerate(args):
                if i < len(hparams):
                    a = "(%s)%s" % (hparams[i], a if SIMPLE.match(a) else "(" + a + ")")
                cast.append(a)
            new = "%s(%s)" % (s, ", ".join(cast))
            if norm(local_ret) not in ("void", norm(hret)):
                new = "((%s)%s)" % (local_ret, new)
            text = text[:m.start()] + new + text[close + 1:]
            pos = m.start() + len(new)
            stats["calls " + s] += 1
        stats["prototypes " + s] += 1
    open(path, "w", encoding="utf-8", errors="surrogateescape", newline="").write(text)

print("files: %d" % len(sites))
for k, v in sorted(stats.items()):
    print("  %-40s %d" % (k, v))
