"""Re-gate every parked candidate with its IR-optimizer pragmas stripped and colorsweep r62 applied (one
word-aligned extern declared aligned(4)), which switches mwcc's IR optimizer off. One compile each, no model.

    IRO_SHARD=i/n python irosweep.py      disjoint shards by address hash
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
import hashlib
import os
import re
import shutil
import subprocess
import sys

SP = _kp.SP
KIT = _kp.KIT
sys.path.insert(0, KIT)
import colorsweep  # noqa: E402

_src = open(f"{KIT}/repairsweep.py", encoding="utf-8").read()
_ns = {"__file__": f"{KIT}/repairsweep.py", "__name__": "repairsweep_pool"}
os.environ.pop("REPAIR_SHARD", None)
os.environ.pop("REPAIR_ONLY", None)
exec(compile(_src[:_src.index("\ndef gate(")], "repairsweep_pool", "exec"), _ns)
REPO = _ns["REPO"]

_sh = os.environ.get("IRO_SHARD", "1/1")
SHARD, NSHARD = (int(x) for x in _sh.split("/"))
TAG = f"_{SHARD}of{NSHARD}"
WORK = f"{SP}/iro_work{TAG}"
os.makedirs(WORK, exist_ok=True)

jobs = [j for j in _ns["jobs"] if int(hashlib.md5(j[1].encode()).hexdigest(), 16) % NSHARD == SHARD - 1]
print(f"{len(jobs)} candidates in shard {SHARD}/{NSHARD}", flush=True)

IRO_PRAGMA = re.compile(r"(?m)^[ \t]*#pragma[ \t]+(?:opt_propagation|opt_common_subs|opt_dead_assignments|"
                        r"opt_loop_invariants|opt_lifetimes|opt_strength_reduction|push|pop)\b[^\n]*\n")
log = open(f"{SP}/wlog/irosweep{TAG}.log", "a", encoding="utf-8")
hits = 0
for i, (mod, addr, f) in enumerate(jobs):
    text = IRO_PRAGMA.sub("", open(f, encoding="utf-8", errors="ignore").read())
    if "#pragma" in text and not re.search(r"#pragma\s+(?:define_section|section)\b", text):
        continue
    got = colorsweep.r62_iro_align(text)
    if not got:
        continue
    tag, new = got[0]
    work = f"{WORK}/{os.path.basename(f)}"
    open(work, "w", encoding="utf-8", newline="").write(new)
    r = subprocess.run([sys.executable, f"{KIT}/wgate.py", mod, addr, work], capture_output=True,
                       text=True, cwd=REPO, stdin=subprocess.DEVNULL)
    verdict = ((r.stdout or "") + (r.stderr or "")).strip().splitlines()
    verdict = verdict[-1] if verdict else "NO-OUTPUT"
    log.write(f"{mod} {addr} {tag} {f} :: {verdict[:160]}\n")
    log.flush()
    if verdict.startswith("MATCH"):
        hits += 1
        print(f"HIT {mod} {addr} {tag} {f}", flush=True)
        hold = f"{SP}/iro_hits"
        os.makedirs(hold, exist_ok=True)
        shutil.copy(work, f"{hold}/{os.path.basename(f)}")
    os.remove(work)
    if i % 50 == 49:
        print(f"  ...{i+1}/{len(jobs)}, {hits} hits", flush=True)
shutil.rmtree(WORK, ignore_errors=True)
print(f"done: {hits} hits", flush=True)
