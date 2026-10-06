#!/usr/bin/env python3
"""Re-match committed sources WITHOUT `opt_propagation` / `opt_common_subs` pragmas.

The retail build has every pass on, so a source that only matches under one of these is a source
whose residue we papered over. For each committed source carrying either: strip every such line,
gate, and if that alone does not match, hand the stripped source to colorsweep and RE-GATE what it
leaves. One line per address; the pragma-free diff of every winner goes to wlog/proppurge/<addr>.diff,
and `--apply` writes the winner back over the repo source.

    python proppurge.py [--apply] [--budget N] [--only <addr>] [--shard K/N]
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
import difflib
import os
import re
import subprocess
import sys

SP = os.path.dirname(os.path.abspath(__file__)).replace(chr(92), "/")
REPO = _kp.REPO
PRAGMA = re.compile(r"(?m)^[ \t]*#[ \t]*pragma[ \t]+(?:opt_propagation|opt_common_subs)\b[^\n]*\n")
TAG = re.compile(r"(?m)^//\s*USA:\s*func_(?:ov(\d+)_)?([0-9a-fA-F]{8})\b")

APPLY = "--apply" in sys.argv
BUDGET = int(sys.argv[sys.argv.index("--budget") + 1]) if "--budget" in sys.argv else 120
ONLY = sys.argv[sys.argv.index("--only") + 1].lower() if "--only" in sys.argv else None
SHARD = sys.argv[sys.argv.index("--shard") + 1] if "--shard" in sys.argv else "1/1"
K, N = (int(x) for x in SHARD.split("/"))

ENV = dict(os.environ, WGATE_ALLOW_COMMITTED="1")
WORK = f"{SP}/_pp_{os.getpid()}.cpp"
OUT = f"{SP}/wlog/proppurge"


def gate(mod, addr, path):
    r = subprocess.run([sys.executable, f"{SP}/wgate.py", mod, addr, path],
                       capture_output=True, text=True, env=ENV)
    lines = (r.stdout + r.stderr).splitlines()
    if any(l.strip() == "MATCH" for l in lines):
        return "MATCH"
    return next((l for l in lines if l.startswith("RESIDUE")), lines[-1] if lines else "?")


def land(path, old, new, addr):
    with open(f"{OUT}/{addr}.diff", "w", encoding="utf-8", newline="\n") as fh:
        fh.writelines(difflib.unified_diff(old.splitlines(True), new.splitlines(True),
                                           path, path + " (pragma-free)"))
    if APPLY:
        with open(path, "w", encoding="utf-8", newline="\n") as fh:
            fh.write(new)


def main():
    srcs = []
    for root, _dirs, files in os.walk(f"{REPO}/src"):
        for fn in files:
            if fn.endswith((".cpp", ".c")):
                p = os.path.join(root, fn).replace(chr(92), "/")
                text = open(p, encoding="utf-8").read()
                if PRAGMA.search(text):
                    srcs.append((p, text))
    srcs.sort()
    os.makedirs(OUT, exist_ok=True)

    for i, (path, text) in enumerate(srcs):
        if i % N != K - 1:
            continue
        m = TAG.search(text)
        if not m:
            print(f"NO-TAG   {path}", flush=True)
            continue
        mod = "main" if m.group(1) is None else m.group(1)
        addr = m.group(2).lower()
        if ONLY and addr != ONLY:
            continue
        stripped = PRAGMA.sub("", text)
        with open(WORK, "w", encoding="utf-8", newline="\n") as fh:
            fh.write(stripped)
        head = gate(mod, addr, WORK)
        if head == "MATCH":
            print(f"PRAGMA-UNNEEDED {mod:>4} {addr}  {os.path.basename(path)}", flush=True)
            land(path, text, stripped, addr)
            continue

        r = subprocess.run([sys.executable, f"{SP}/colorsweep.py", mod, addr, WORK,
                            "--budget", str(BUDGET), "--apply"],
                           capture_output=True, text=True, env=ENV)
        best = [l.strip() for l in (r.stdout + r.stderr).splitlines()
                if l.strip().startswith(("base:", "best", "RESULT"))]
        after = gate(mod, addr, WORK)
        if after == "MATCH":
            print(f"CRACKED  {mod:>4} {addr}  {os.path.basename(path)}", flush=True)
            land(path, text, open(WORK, encoding="utf-8").read(), addr)
        else:
            print(f"KEEPS    {mod:>4} {addr}  {os.path.basename(path)}  {head}  ->  {after}"
                  f"  | {best[-1] if best else ''}", flush=True)
    if os.path.exists(WORK):
        os.remove(WORK)
    print(f"done: shard {SHARD}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
