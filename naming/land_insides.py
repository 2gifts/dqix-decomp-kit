import json, os, subprocess, sys

from namingpaths import LABEL as REPO, NAMING as SP

PLAN = sys.argv[1]
OUT = sys.argv[2] if len(sys.argv) > 2 else PLAN.replace(".json", "_kept.json")


def run(argv, cwd=REPO):
    return subprocess.call(argv, cwd=cwd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


def attempt(items):
    json.dump(items, open(SP + "/try_insides.json", "w", encoding="utf-8"))
    run(["git", "checkout", "-q", "--", "src", "include", "config"])
    if run([sys.executable, SP + "/rename_apply.py", SP + "/try_insides.json", "--apply"]) != 0:
        return False
    return run(["ninja", "check"]) == 0


plan = json.load(open(PLAN, encoding="utf-8"))
dropped = []
while True:
    if attempt(plan):
        break
    lo, hi = 0, len(plan)
    while lo < hi:
        mid = (lo + hi) // 2
        print("  prefix", mid, flush=True)
        if attempt(plan[:mid]):
            lo = mid + 1
        else:
            hi = mid
    if lo == 0 or lo > len(plan):
        print("bisect failed to isolate", flush=True)
        break
    bad = plan[lo - 1]
    print("DROP", bad["file"], flush=True)
    dropped.append(bad)
    plan = [p for p in plan if p["file"] != bad["file"]]
    if not plan:
        break

json.dump(plan, open(OUT, "w", encoding="utf-8"), indent=1)
json.dump(dropped, open(OUT.replace(".json", "_dropped.json"), "w", encoding="utf-8"), indent=1)
print("kept", len(plan), "dropped", len(dropped))
for d in dropped:
    print("   ", d["file"])
