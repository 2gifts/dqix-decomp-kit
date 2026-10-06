import json
import sys


def nonmatching(path):
    r = json.load(open(path))
    out = {}
    for u in r["units"]:
        for f in u.get("functions", []):
            pct = f.get("fuzzy_match_percent", 0.0)
            if pct < 100.0:
                out[f["name"]] = (u["name"], pct)
    return r["measures"], out


base_m, base = nonmatching(sys.argv[1])
new_m, new = nonmatching(sys.argv[2])
print("baseline: %.2f%% (%d matched)" % (base_m.get("matched_functions_percent", 0), base_m.get("matched_functions", 0)))
print("merged:   %.2f%% (%d matched)" % (new_m.get("matched_functions_percent", 0), new_m.get("matched_functions", 0)))
regressed = sorted((u, n, p) for n, (u, p) in new.items() if n not in base)
print("non-matching now, not in baseline's non-matching set: %d" % len(regressed))
for u, n, p in regressed:
    print("  %6.2f%%  %-28s %s" % (p, u, n))
