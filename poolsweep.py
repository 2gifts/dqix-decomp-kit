"""Gate every .cpp lying in a scratchpad directory the pipeline never gathers from, and stage the
matches.

Sources accumulate in ad-hoc working directories (libc/, wave_ov023/, pad/ ...). Nothing globs
those, so a byte-exact function sitting in one is invisible work that a worker is eventually paid to
redo. This walks them, gates each file, and copies a MATCH into staging/<module>/ -- the one pool
finish_wave takes from.

The address comes from the `// USA:` tag, never from the first func_ symbol in the file: every
source declares its callees with the same prefix and those are usually already committed.

Usage: python poolsweep.py [--apply] [dir ...]     (default: every ungathered pool)
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
import glob
import os
import re
import subprocess
import sys

SP = os.path.dirname(os.path.abspath(__file__)).replace(chr(92), "/")
REPO = _kp.REPO
GATHERED = ("staging", "hold_", "_stage", "_reclaim", "quarantine", "repair_work", "_archive",
            "gated", "attempts", "wlog", "inv", "worker_src", "claims", "skiplisted_hold")
TAG = re.compile(r"//\s*USA:\s*func_(?:ov(\d{3})_)?([0-9a-fA-F]{8})")
APPLY = "--apply" in sys.argv
dirs = [a for a in sys.argv[1:] if not a.startswith("--")]

if not dirs:
    dirs = [d for d in glob.glob(f"{SP}/*/")
            if glob.glob(d + "*.cpp") and not any(g in os.path.basename(d.rstrip("/\\")) or
                                                  g in d for g in GATHERED)]

COLORSWEEP_MAX_BYTES = int(os.environ.get("COLORSWEEP_MAX_BYTES", "24"))
WORK = os.path.join(SP, "repair_work")
os.makedirs(WORK, exist_ok=True)

# A skiplisted address is a decided question -- hand asm the placement policy refuses, or a measured
# dead end. Re-gating one produces a HIT the integrator then discards, which reads as a free match
# and is not one: the 2026-08-25 run staged 020c19b8 and 020ca594, both SDK hand asm, both already
# skiplisted. repairsweep has skipped these since 2026-08-26; this sweep did not.
SKIPLISTED = set()
for _p in (f"{SP}/skiplist_main.txt", f"{SP}/skiplist_ov.txt"):
    try:
        for _line in open(_p, encoding="utf-8", errors="ignore"):
            _m = re.match(r"\s*([0-9a-fA-F]{8})\b", _line)
            if _m:
                SKIPLISTED.add(_m.group(1).lower())
    except OSError:
        pass


def gate(mod, addr, path):
    r = subprocess.run([sys.executable, f"{SP}/wgate.py", mod, addr, os.path.abspath(path)],
                       capture_output=True, text=True, cwd=REPO, stdin=subprocess.DEVNULL)
    return (((r.stdout or "") + (r.stderr or "")).strip().splitlines() or ["(none)"])[0]


seen, hits, verdicts, moved = set(), [], {}, {}
for d in sorted(dirs):
    for f in sorted(glob.glob(os.path.join(d, "*.cpp"))):
        txt = open(f, encoding="utf-8", errors="ignore").read()
        m = TAG.search(txt)
        if not m:
            verdicts["NO-TAG"] = verdicts.get("NO-TAG", 0) + 1
            continue
        mod = m.group(1) or "main"
        addr = m.group(2).lower()
        key = (mod, addr, hash(txt))
        if key in seen:
            continue
        seen.add(key)
        if addr in SKIPLISTED:
            verdicts["SKIPLISTED"] = verdicts.get("SKIPLISTED", 0) + 1
            if APPLY:
                dest = f"{SP}/skiplisted_hold"
                os.makedirs(dest, exist_ok=True)
                try:
                    os.replace(f, os.path.join(dest, os.path.basename(f)))
                    moved["SKIPLISTED"] = moved.get("SKIPLISTED", 0) + 1
                except OSError:
                    pass
            continue
        src_path = f
        work = os.path.join(WORK, "%s_%s.cpp" % (mod, addr))
        open(work, "w", encoding="utf-8", newline="\n").write(txt)
        head = gate(mod, addr, work)
        # WRONG-SYMBOL MEANS THE BYTES ALREADY MATCH. Only the exported name is wrong, which is what
        # autorepair rebinds -- so treating it as a miss throws away a finished function. The first
        # run of this sweep reported 19 of them and staged none.
        if "WRONG-SYMBOL" in head or head.startswith("UNDEF-SYM"):
            subprocess.run([sys.executable, f"{SP}/autorepair.py", mod, addr, work],
                           capture_output=True, text=True, cwd=REPO, stdin=subprocess.DEVNULL)
            head = gate(mod, addr, work)
        # A near miss is a colouring away, and the sweep already knows how to close those.
        m2 = re.match(r"BYTEDIFF: (\d+) ", head)
        if m2 and int(m2.group(1)) <= COLORSWEEP_MAX_BYTES:
            subprocess.run([sys.executable, f"{SP}/colorsweep.py", mod, addr, work,
                            "--depth", "3", "--budget", "150", "--apply"],
                           capture_output=True, text=True, cwd=REPO, stdin=subprocess.DEVNULL)
            head = gate(mod, addr, work)
        f, txt = work, open(work, encoding="utf-8", errors="ignore").read()
        tag = head.split(":")[0].split()[0]
        verdicts[tag] = verdicts.get(tag, 0) + 1
        if APPLY:
            # EMPTY THE POOL AS WE GO, or the next sweep re-gates the same files forever and the
            # directory stays stranded. A near miss goes to quarantine/, which repairsweep DOES
            # gather, so a future colorsweep rule gets another shot at it. Everything else is
            # archived where nothing globs it.
            dest = (f"{SP}/quarantine" if tag in ("BYTEDIFF", "SIZE/OVERGEN", "RELOC-WRONG")
                    else f"{SP}/_archive/swept_pools")
            os.makedirs(dest, exist_ok=True)
            try:
                os.replace(src_path, os.path.join(dest, os.path.basename(src_path)))
                moved[tag] = moved.get(tag, 0) + 1
            except OSError:
                pass
        if head.startswith("MATCH"):
            hits.append((mod, addr, f))
            print(f"HIT {mod}:{addr} {os.path.relpath(f, SP)}", flush=True)
            if APPLY:
                stage = f"{SP}/staging/" + ("main" if mod == "main" else f"ov{mod}")
                os.makedirs(stage, exist_ok=True)
                open(f"{stage}/{os.path.basename(f)}", "w", encoding="utf-8",
                     newline="\n").write(txt)

print("scanned %d file(s) in %d pool(s): %s" % (len(seen), len(dirs), verdicts))
print("%d match(es)%s" % (len(hits), " staged" if APPLY else " (dry run, use --apply)"))
if moved:
    print("cleared out of the stranded pools: %s" % moved)
