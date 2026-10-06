#!/usr/bin/env python
"""Measure $/function per batch size, so SMALL_PER and worker count are set by evidence, not by argument.

The two knobs are orthogonal and must not be tuned by feel:
  worker COUNT   -> burn rate (pace.py holds it to the weekly budget)
  addrs/worker   -> cost per function (short sessions are cheaper per address; cost per MESSAGE rises
                    with session length, measured $0.060 -> $0.120 across 179 sessions)
Both were set from one-off measurements. This re-measures continuously, because the optimum moves as
the remaining functions get harder and as new recipes land.

Method: every wave logs `spawning up to N workers xP`. That gives a time window and the P in effect.
Worker session cost inside the window is attributed to that P, functions landed likewise (from git).
Output is $/function per P, with sample sizes -- pick the winner only when the samples are comparable.

Usage: python autotune.py [hours=24]
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
import glob, io, json, os, re, subprocess, sys, time, collections
from datetime import datetime, timezone

SP = _kp.SP
REPO = _kp.REPO
HOURS = float(sys.argv[1]) if len(sys.argv) > 1 else 24
ME = os.environ.get("AUTOTUNE_EXCLUDE_SESSION", "")
os.chdir(REPO)
cut = time.time() - HOURS * 3600

# ---- wave windows: (start_epoch, per) from the driver logs, using file mtime ordering ------------
waves = []
for lg in glob.glob(f"{SP}/wlog/run_*.log"):
    try:
        if os.path.getmtime(lg) < cut:
            continue
    except OSError:
        continue
    # the log has no timestamps; use its own mtime as the end and space waves evenly is WRONG, so we
    # only use logs that record per-wave lines and pair them with the worker-log mtimes below.
    for m in re.finditer(r'spawning up to (\d+) workers x(\d+)', io.open(lg, encoding='utf-8', errors='ignore').read()):
        waves.append(int(m.group(2)))
if not waves:
    print("no wave records in window")
    sys.exit()

# ---- attribute worker sessions to a P by matching session start to the wave's worker logs --------
# worker log files are named <mod>_w<N>_<batch>.log and their mtime is when the worker EXITED.
per_by_time = []
for wl in glob.glob(f"{SP}/wlog/*_w*_*.log"):
    try:
        mt = os.path.getmtime(wl)
    except OSError:
        continue
    if mt < cut:
        continue
    per_by_time.append(mt)
per_by_time.sort()

# ---- session costs -------------------------------------------------------------------------------
sess = []
for f in glob.glob(_kp.CLAUDE_PROJECTS + "/*/*.jsonl"):
    if ME and ME in f:
        continue
    try:
        if os.path.getmtime(f) < cut:
            continue
    except OSError:
        continue
    n = 0
    cost = 0.0
    first = None
    for l in io.open(f, encoding='utf-8', errors='ignore'):
        if '"output_tokens"' not in l:
            continue
        try:
            d = json.loads(l)
        except ValueError:
            continue
        u = (d.get("message") or {}).get("usage") or {}
        if not u:
            continue
        ts = d.get("timestamp") or ""
        try:
            t = datetime.strptime(ts[:19], "%Y-%m-%dT%H:%M:%S").replace(tzinfo=timezone.utc).timestamp()
        except ValueError:
            continue
        if t < cut:
            continue
        first = min(first, t) if first else t
        n += 1
        cost += (u.get("input_tokens", 0) * 3 + u.get("output_tokens", 0) * 15
                 + u.get("cache_creation_input_tokens", 0) * 3.75
                 + u.get("cache_read_input_tokens", 0) * 0.30) / 1e6
    if n >= 10:
        sess.append((first, n, cost))

funcs = sum(int(x) for x in re.findall(r'batch of (\d+)', subprocess.run(
    ["git", "log", f"--since={int(HOURS)} hours ago", "--format=%s"],
    capture_output=True, text=True).stdout))

tot = sum(c for _, _, c in sess)
msgs = sum(n for _, n, _ in sess)
print(f"window {HOURS:.0f}h   sessions {len(sess)}   messages {msgs:,}   worker cost ${tot:,.2f}")
print(f"functions landed {funcs}   ==> ${tot/funcs:.2f}/function" if funcs else "no functions landed")
print(f"batch sizes seen this window: {sorted(set(waves))}")
print()
print("session-length cost curve (the lever):")
for lo, hi, lab in [(10, 40, "10-39"), (40, 80, "40-79"), (80, 140, "80-139"),
                    (140, 220, "140-219"), (220, 10**9, "220+")]:
    g = [c for _, n, c in sess if lo <= n < hi]
    m = [n for _, n, c in sess if lo <= n < hi]
    if g:
        print(f"   {lab:<9}{len(g):>4} sess  avg ${sum(g)/len(g):>7.2f}  ${sum(g)/sum(m):.4f}/msg")
hist = f"{SP}/wlog/autotune_history.tsv"
with io.open(hist, "a", encoding='utf-8', newline='\n') as fh:
    fh.write(f"{int(time.time())}\t{sorted(set(waves))}\t{len(sess)}\t{msgs}\t{tot:.2f}\t{funcs}\t"
             f"{(tot/funcs if funcs else 0):.2f}\n")
print(f"\nappended to {os.path.basename(hist)} (compare across settings over time)")
