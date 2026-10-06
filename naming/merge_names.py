import glob, json, os, sys

from namingpaths import NAMING as SP

DIR = sys.argv[1] if len(sys.argv) > 1 else SP + "/chunks4"
OUTDIR = sys.argv[2] if len(sys.argv) > 2 else SP + "/final4"
PLAN = OUTDIR + "/plan.json"
INSIDES = OUTDIR + "/insides.json"

meta = {}
for p in sorted(glob.glob(DIR + "/batch*.json")):
    for it in json.load(open(p, encoding="utf-8")):
        meta[it["address"].lower()] = it

found = {}
for p in sorted(glob.glob(OUTDIR + "/name*.json")):
    try:
        for it in json.load(open(p, encoding="utf-8")):
            found[it["address"].lower()] = it
    except Exception as e:
        print("   BAD NAMES", os.path.basename(p), e)

verdicts = {}
for p in sorted(glob.glob(OUTDIR + "/verdict*.json")):
    try:
        for it in json.load(open(p, encoding="utf-8")):
            verdicts[it["address"].lower()] = it
    except Exception as e:
        print("   BAD VERDICT", os.path.basename(p), e)

plan, insides = [], []
kept = dropped = unaudited = insuff = nren = nrej = 0
for a, it in sorted(found.items()):
    m = meta.get(a)
    if m is None:
        continue
    v = verdicts.get(a)
    if v is None:
        unaudited += 1
        continue
    name = v.get("better_name") or it.get("proposed_name")
    if not v.get("keep") or not name or it.get("confidence") == "insufficient":
        if not name:
            insuff += 1
        else:
            dropped += 1
        continue
    comment = v.get("better_comment") or it.get("comment") or ""
    plan.append({
        "address": m["address"],
        "current_name": m["current_name"],
        "name": name,
        "comment": comment,
        "module": m["module"],
        "labeling_source": m["labeling_source"],
    })
    rr = set(v.get("reject_renames") or [])
    re_ = set(x.strip() for x in (v.get("reject_edits") or []))
    fixes = v.get("fix_renames") or []
    fixed = set(f["old"] for f in fixes)
    keep_r = [{"old": r["old"], "new": r["new"], "scope": r.get("scope", "")}
              for r in (it.get("renames") or [])
              if r["old"] not in rr and r["old"] not in fixed]
    keep_r += [{"old": f["old"], "new": f["new"], "scope": f.get("scope", "")} for f in fixes]
    keep_e = [{"old": e["old"], "new": e["new"]}
              for e in (it.get("edits") or []) if e["old"].strip() not in re_]
    nren += len(keep_r)
    nrej += len(it.get("renames") or []) - len(keep_r)
    if keep_r or keep_e:
        insides.append({
            "file": os.path.dirname(m["labeling_source"]) + "/" + name + ".cpp",
            "function": name,
            "renames": keep_r,
            "edits": keep_e,
        })
    kept += 1

json.dump(plan, open(PLAN, "w", encoding="utf-8"), indent=1)
json.dump(insides, open(INSIDES, "w", encoding="utf-8"), indent=1)
print("named kept:", kept, " audit-dropped:", dropped, " insufficient:", insuff,
      " unaudited:", unaudited)
print("renames kept:", nren, " rejected:", nrej, " files with insides:", len(insides))
print("->", PLAN)
print("->", INSIDES)
