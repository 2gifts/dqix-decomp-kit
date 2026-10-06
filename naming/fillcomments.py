import json, re, subprocess, sys

from namingpaths import LABEL as REPO

plan = json.load(open(sys.argv[1]))

filled = 0
for item in plan:
    try:
        body = subprocess.check_output(
            ["git", "-C", REPO, "show", "labeling-pass:" + item["labeling_source"]]
        ).decode("utf-8", "replace")
    except subprocess.CalledProcessError:
        item["comment"] = "// USA: %s" % item["current_name"]
        continue
    lines = body.split("\n")
    # the definition is the last line naming the function and opening a body
    di = None
    for i, ln in enumerate(lines):
        if "(" in ln and "{" in "".join(lines[i:i + 2]) and not ln.lstrip().startswith("//"):
            if re.search(r"\bARM\b|\bTHUMB\b", ln) or ln.startswith(("extern", "static", "void", "int")):
                di = i
    block = []
    if di is not None:
        j = di
        while j > 0 and lines[j - 1].lstrip().startswith("//"):
            j -= 1
        block = [l.strip() for l in lines[j:di]]
    if not block:
        block = ["// USA: %s" % item["current_name"]]
    else:
        filled += 1
    item["comment"] = "\n".join(block)

json.dump(plan, open(sys.argv[2], "w"), indent=1)
print("entries:", len(plan), " with an existing comment block:", filled)
