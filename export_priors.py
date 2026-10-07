"""Gate every saved attempt at an unmatched function and keep the closest one per address in priors/.

    python export_priors.py <source-sp> [-j N] [--dry-run]

<source-sp> is a pipeline directory with attempt history (its hold_*, *_stage, staging, quarantine,
attempts, gated, clsbest and wip pools); this kit's own priors/ is always included. Each candidate is
gated with this kit's wgate.py against $DQIX_REPO. The best per address (MATCH, then a size-exact
residue, then a wrong-length one, smallest metric first) is written to priors/<main|ovNNN>/<addr>.cpp
and listed in priors/INDEX.tsv. A MATCH is also printed: it is a free landing for whoever runs this.
"""
import argparse
import concurrent.futures
import glob
import os
import re
import shutil
import subprocess
import sys

import kitpaths
import resumable

SP = kitpaths.SP
KIT = kitpaths.KIT
REPO = kitpaths.REPO
ADDR = re.compile(r"(0[0-9a-fA-F]{7})")
WRONG_LENGTH = {"OVERGEN", "UNDERGEN", "SIZE"}
UNUSABLE = {"NO-COMPILE", "PRAGMA", "BAD-NAME", "ALREADY-COMMITTED", "UNKNOWN"}


def candidates(src):
    pats = resumable.POOLS + ["clsbest/*.cpp", "wip/*/*.cpp"]
    files = [f for p in pats for f in glob.glob(f"{src}/{p}")]
    files += glob.glob(f"{KIT}/priors/*/*.cpp")
    by = {}
    for f in files:
        f = f.replace("\\", "/")
        m = ADDR.search(os.path.basename(f))
        if not m:
            continue
        a = m.group(1).lower()
        if resumable.matched(a):
            continue
        mod = module_of(f, a)
        if mod:
            by.setdefault((mod, a), []).append(f)
    return by


def module_of(f, addr):
    d = os.path.basename(os.path.dirname(f))
    m = re.fullmatch(r"(?:hold_|ov)?(\d{3})(?:_stage)?", d)
    if m:
        return m.group(1)
    if d in ("main", "hold_main", "main_stage"):
        return "main"
    return resumable.module_of(addr)


def gate(mod, addr, f):
    try:
        out = subprocess.run([sys.executable, f"{KIT}/wgate.py", mod, addr, f], cwd=REPO,
                             capture_output=True, text=True, timeout=300).stdout
    except subprocess.TimeoutExpired:
        return (9, 0, "TIMEOUT", 0)
    if re.search(r"(?m)^MATCH\b", out):
        return (0, 0, "MATCH", 0)
    res = re.findall(r"(?m)^RESIDUE (\S+) (-?\d+)", out)
    if not res:
        return (9, 0, "UNKNOWN", 0)
    cls, metric = res[-1][0], int(res[-1][1])
    if cls in UNUSABLE:
        return (9, metric, cls, metric)
    tier = 2 if cls in WRONG_LENGTH else 1
    return (tier, metric, cls, metric)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("source_sp")
    ap.add_argument("-j", type=int, default=4)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()
    src = os.path.abspath(args.source_sp).replace("\\", "/")
    by = candidates(src)
    jobs = [(mod, a, f) for (mod, a), fs in sorted(by.items()) for f in fs]
    print(f"{len(by)} unmatched addresses, {len(jobs)} candidates", flush=True)
    best = {}
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.j) as ex:
        futs = {ex.submit(gate, *j): j for j in jobs}
        for n, fut in enumerate(concurrent.futures.as_completed(futs), 1):
            mod, a, f = futs[fut]
            score = fut.result()
            if score[2] == "MATCH":
                print(f"MATCH {mod} {a} {f}", flush=True)
            if score[0] < 9 and ((mod, a) not in best or score[:2] < best[(mod, a)][0][:2]):
                best[(mod, a)] = (score, f)
            if n % 50 == 0:
                print(f"  {n}/{len(jobs)} gated", flush=True)
    if args.dry_run:
        for (mod, a), (score, f) in sorted(best.items()):
            print(f"{mod}\t{a}\t{score[2]}\t{score[3]}\t{f}")
        return
    out = f"{KIT}/priors"
    staged = f"{SP}/priors.new"
    shutil.rmtree(staged, ignore_errors=True)
    rows = []
    for (mod, a), (score, f) in sorted(best.items()):
        d = f"{staged}/{'main' if mod == 'main' else 'ov' + mod}"
        os.makedirs(d, exist_ok=True)
        shutil.copyfile(f, f"{d}/{a}.cpp")
        rows.append(f"{mod}\t{a}\t{score[2]}\t{score[3]}\n")
    with open(f"{staged}/INDEX.tsv", "w", encoding="utf-8", newline="\n") as fh:
        fh.write("module\taddr\tresidue\tmetric\n")
        fh.writelines(rows)
    shutil.rmtree(out, ignore_errors=True)
    os.replace(staged, out)
    print(f"wrote {len(rows)} priors to {out}")


main()
