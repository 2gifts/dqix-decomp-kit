import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__))))
import kitpaths as _kp
import json
import os
import sys

sys.path.insert(0, (_kp.KIT + "/frida"))
import colorforce as cf  # noqa: E402

OLD_ORDER = """  if (flipHere && CFG.kind === 'order' && CFG.pos + 1 < nodes.length) {"""
NEW_ORDER = """  if (flipHere && CFG.moves) {
    for (const mv of CFG.moves) {
      let at = -1;
      for (let k = 0; k < nodes.length; k++) if (nodes[k].add(0x28).readS16() === mv[0]) at = k;
      if (at >= 0) { const t = nodes.splice(at, 1)[0]; nodes.splice(mv[1], 0, t); }
    }
    for (let i = 0; i < nodes.length; i++) nodes[i].writePointer(i + 1 < nodes.length ? nodes[i + 1] : ptr(0));
  }
  if (flipHere && CFG.kind === 'order' && CFG.pos + 1 < nodes.length) {"""
OLD_CHOICE = """if (flipHere && CFG.kind === 'choice' && CFG.idx === idx && cands.length > 1) pick = 1;"""
NEW_CHOICE = """if (flipHere && CFG.choices && CFG.choices.indexOf(idx) >= 0 && cands.length > 1) pick = 1;"""
assert OLD_ORDER in cf.JS and OLD_CHOICE in cf.JS
cf.JS = cf.JS.replace(OLD_ORDER, NEW_ORDER).replace(OLD_CHOICE, NEW_CHOICE)

src, mod, addr, size, pool_from = sys.argv[1], sys.argv[2], int(sys.argv[3], 16), int(sys.argv[4], 0), int(sys.argv[5], 16)
cfg = json.loads(sys.argv[6])
cfg.setdefault("call", -1)
cfg.setdefault("kind", "multi")
R = cf.forcereal.rom(mod, addr, size)
out = os.path.splitext(src)[0] + ".cfm"
os.makedirs(out, exist_ok=True)
obj = out + "/multi.o"
ev = cf.run(src, cfg, obj)
bad, err = cf.score(obj, R, size)
calls = [e for e in ev if e.get("ev") == "color"]
with open(out + "/trace.json", "w") as fh:
    json.dump(calls, fh)
last = calls[-1]
print("err", err, "real", None if bad is None else len([b for b in bad if b < pool_from]),
      None if bad is None else ["0x%x" % b for b in bad if b < pool_from])
if "--order" in sys.argv:
    for ci, c in enumerate(calls[:-1]):
        print("call", ci, "spilled", [n[0] for n in c["nodes"] if n[1] == -1])
    for pos, n in enumerate(last["nodes"][:16]):
        print(pos, "idx", n[0], "reg", n[1], "nc", n[2])
