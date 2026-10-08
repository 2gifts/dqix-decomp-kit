"""Rename symbols in a source file without corrupting another region's address defines.

Inside `#if defined(jpn|eur)` a `#define NAME TARGET` maps the name the shared source uses to that
region's address. Only NAME follows a rename; TARGET never does, and the define is dropped once the
region's config binds the renamed NAME to TARGET itself.
"""
import glob
import re

import kitpaths as _kp

REGION_IF = re.compile(r"\s*#\s*if\s+defined\s*\(\s*(jpn|eur)\s*\)")
REGION_DEFINE = re.compile(r"(\s*#\s*define\s+)(\S+)(\s+)(\S+)(.*)")
_NAMES = {}


def region_bound(region, raw):
    m = re.fullmatch(r"(?:func|data)_(?:ov(\d+)_)?([0-9a-fA-F]{8})", raw)
    if not m:
        return None
    if region not in _NAMES:
        _NAMES[region] = {}
        for p in glob.glob(f"{_kp.REPO}/config/{region}/arm9/**/symbols.txt", recursive=True):
            ov = re.search(r"overlays[/\\]ov(\d+)", p)
            for n, a in re.findall(r"(?m)^(\S+)\s+kind:\w+[^\n]*?addr:0x([0-9a-fA-F]+)",
                                   open(p, encoding="utf-8").read()):
                _NAMES[region][(ov.group(1) if ov else None, int(a, 16))] = n
    return _NAMES[region].get((m.group(1), int(m.group(2), 16)))


def rewrite(text, pat, renames, bound=region_bound):
    lines, n = text.split("\n"), 0
    region, depth, dropped = None, 0, set()
    for k, ln in enumerate(lines):
        if re.match(r"\s*//\s*USA:", ln, re.I):
            continue
        if region:
            if re.match(r"\s*#\s*if", ln):
                depth += 1
            elif re.match(r"\s*#\s*endif", ln):
                depth -= 1
                if depth == 0:
                    region = None
                continue
            elif depth == 1 and re.match(r"\s*#\s*(else|elif)", ln):
                region = None
                continue
            d = REGION_DEFINE.match(ln)
            if d and depth == 1:
                left = pat.sub(lambda m: renames[m.group(1)], d.group(2))
                if left != d.group(2):
                    n += 1
                    if bound(region, d.group(4)) == left:
                        dropped.add(k)
                    else:
                        lines[k] = d.group(1) + left + d.group(3) + d.group(4) + d.group(5)
            continue
        r = REGION_IF.match(ln)
        if r:
            region, depth = r.group(1), 1
            continue
        lines[k], c = pat.subn(lambda m: renames[m.group(1)], ln)
        n += c
    return "\n".join(l for k, l in enumerate(lines) if k not in dropped), n
