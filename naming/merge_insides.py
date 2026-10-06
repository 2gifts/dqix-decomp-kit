import glob, json, os, re, sys

from namingpaths import NAMING as SP

OUTDIR = sys.argv[1] if len(sys.argv) > 1 else SP + "/inside_out"
FINAL = sys.argv[2] if len(sys.argv) > 2 else OUTDIR + "/final.json"

plans = {}
for p in sorted(glob.glob(OUTDIR + "/plan*.json")):
    try:
        for it in json.load(open(p, encoding="utf-8")):
            plans[it["file"].replace("\\", "/")] = it
    except Exception as e:
        print("   BAD PLAN", os.path.basename(p), e)

verdicts = {}
for p in sorted(glob.glob(OUTDIR + "/verdict*.json")):
    try:
        for it in json.load(open(p, encoding="utf-8")):
            verdicts[it["file"].replace("\\", "/")] = it
    except Exception as e:
        print("   BAD VERDICT", os.path.basename(p), e)

final, unaudited = [], []
nren = nrej = nedit = nedrej = ncom = nfix = 0
for f, it in sorted(plans.items()):
    v = verdicts.get(f)
    if v is None:
        unaudited.append(f)
        continue
    rr = set(v.get("reject_renames") or [])
    re_ = set(x.strip() for x in (v.get("reject_edits") or []))
    fixes = v.get("fix_renames") or []
    fixed = set(f["old"] for f in fixes)
    keep = [r for r in it.get("renames", [])
            if r["old"] not in rr and r["old"] not in fixed]
    keep += [{"old": f["old"], "new": f["new"], "scope": f.get("scope", "")} for f in fixes]
    kedits = [e for e in it.get("edits", []) if e["old"].strip() not in re_]
    nren += len(keep)
    nrej += len([r for r in it.get("renames", []) if r["old"] in rr])
    nfix += len(fixes)
    nedit += len(kedits)
    nedrej += len(it.get("edits", [])) - len(kedits)
    comment = v.get("better_comment") or it.get("comment") or ""
    if v.get("better_comment"):
        ncom += 1
    if not keep and not kedits and not comment:
        continue
    final.append({
        "file": f,
        "function": it.get("function"),
        "renames": [{"old": r["old"], "new": r["new"], "scope": r.get("scope", "")}
                    for r in keep],
        "edits": [{"old": e["old"], "new": e["new"]} for e in kedits],
        "comment": comment,
    })

json.dump(final, open(FINAL, "w", encoding="utf-8"), indent=1)
print("files:", len(final), " renames kept:", nren, "rejected:", nrej,
      " edits kept:", nedit, "rejected:", nedrej, " comments replaced by audit:", ncom, " audit-scoped fixes:", nfix)
if unaudited:
    print("UNAUDITED, dropped:", len(unaudited))
    for f in unaudited[:10]:
        print("   ", f)
print("->", FINAL)
