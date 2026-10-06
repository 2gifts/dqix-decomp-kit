"""What stops translate.py from finishing func_02061c04, counted by instruction form.

`attempt()` reports only "partial" when the draft contains UNTRANSLATED markers. The translator is
designed to be taught: every form it learns is re-run over the whole pool, so coverage only grows.
This prints which forms are actually blocking, most frequent first -- that is the work list.
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__))))
import kitpaths as _kp
import collections
import importlib.util
import os
import re
import sys

SP = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, SP)
os.chdir(_kp.REPO)

spec = importlib.util.spec_from_file_location("_t", f"{SP}/translate.py")
T = importlib.util.module_from_spec(spec)
spec.loader.exec_module(T)

src = T.translate("main", "02061c04")
if not src:
    print("translate() returned nothing")
    raise SystemExit(1)

out = f"{SP}/pad/c04_draft.cpp"
open(out, "w", encoding="utf-8", newline="\n").write(src)
lines = src.split(chr(10))
bad = [l for l in lines if "UNTRANSLATED" in l]
print(f"draft: {len(lines)} lines -> {out}")
print(f"UNTRANSLATED: {len(bad)} of {sum(1 for l in lines if l.strip())} non-empty lines\n")

forms = collections.Counter()
for l in bad:
    m = re.search(r"UNTRANSLATED[: ]+(.*)", l)
    txt = (m.group(1) if m else l).strip().strip("*/").strip()
    mn = txt.split()[0] if txt.split() else "?"
    forms[mn] += 1
print("blocking mnemonics:")
for mn, n in forms.most_common(20):
    ex = next(l.strip() for l in bad if mn in l)
    print(f"  {n:4d}x  {mn:10s} e.g. {ex[:90]}")
