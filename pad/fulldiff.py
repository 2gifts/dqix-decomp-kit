"""Classify every differing instruction in a size-exact function.

Once the compiled function is the right LENGTH, instruction i of the ROM lines up with instruction i
of ours, and the residue splits into kinds that need completely different work:

  reg    same mnemonic and shape, different register names        -> whole-function colouring
  sp     differs only in an sp-relative displacement              -> frame layout
  pool   a pc-relative load or a branch displacement              -> noise, resolves on its own
  shape  anything else                                            -> a real per-site difference

Counting them is the only way to know whether "5183 bytes differ" means one colouring decision or a
hundred separate bugs.

Usage: python fulldiff.py [src.cpp] [--list KIND] [--limit N]
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff

REG = re.compile(r"\b(r\d+|sb|sl|fp|ip|lr|pc|sp)\b")
SPOFF = re.compile(r"\[sp,? #?(?:0x)?[0-9a-fA-F]+\]|sp, #(?:0x)?[0-9a-fA-F]+")
HEX = re.compile(r"#-?(?:0x)?[0-9a-fA-F]+")


def _target(ins):
    """Absolute branch target, or None when the operand is not a plain displacement."""
    m = re.match(r"^#(-?)(?:0x)?([0-9a-fA-F]+)$", ins.op_str.strip())
    return int(m.group(2), 16) * (-1 if m.group(1) else 1) if m else None


def kind(a, b, lo=None, hi=None):
    if a.mnemonic != b.mnemonic:
        return "shape"
    # A `bl` displacement is masked by its relocation, and a branch that leaves the case body lands
    # on shared tail code whose address moves with the literal pools. But a branch whose target is
    # INSIDE the body is control flow, and a difference there is a wrong C construct -- case 0x86
    # guarded its inner switch that the target leaves unguarded, and the only visible symptom was
    # one `beq` landing 0x13c further on.
    if re.match(r"^(b|bl|blx)(eq|ne|cs|cc|hs|lo|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|al)?$", a.mnemonic) \
            or "[pc" in a.op_str or "[pc" in b.op_str:
        if lo is not None and a.mnemonic != "bl":
            ta, tb = _target(a), _target(b)
            if ta is not None and tb is not None and lo <= ta < hi and lo <= tb < hi:
                return "pool" if ta == tb else "shape"
        return "pool"
    if REG.sub("R", a.op_str) == REG.sub("R", b.op_str):
        return "reg"
    if "sp" in a.op_str and "sp" in b.op_str and HEX.sub("#N", a.op_str) == HEX.sub("#N", b.op_str):
        return "sp"
    return "shape"


if __name__ == "__main__":
    src = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith("--") \
        else f"{casediff.SP}/c04work/c04.cpp"
    want = None
    limit = 40
    for i, a in enumerate(sys.argv):
        if a == "--list":
            want = sys.argv[i + 1]
        if a == "--limit":
            limit = int(sys.argv[i + 1])

    rom, ours = casediff.rom_text(), casediff.our_text(src)
    rb, ob = casediff.bodies(rom), casediff.bodies(ours)
    # Align PER CASE. A single case of the wrong length shifts every later instruction, and a global
    # index alignment then reports the whole tail as structural noise (measured: 1602 fake "shape"
    # rows). Cases whose lengths differ are counted as unaligned instead of misread.
    counts = {}
    unaligned = []
    shown = 0
    for case in sorted(rb):
        ro, rl = rb[case]
        if case not in ob or ob[case][1] != rl:
            unaligned.append(case)
            continue
        oo = ob[case][0]
        a = list(casediff.MD.disasm(rom[ro:ro + rl], ro))
        b = list(casediff.MD.disasm(ours[oo:oo + rl], ro))
        for i in range(min(len(a), len(b))):
            x, y = a[i], b[i]
            if x.mnemonic == y.mnemonic and x.op_str == y.op_str:
                continue
            k = kind(x, y, ro, ro + rl)
            counts[k] = counts.get(k, 0) + 1
            if want and k == want and shown < limit:
                shown += 1
                print("  case 0x%02x +0x%04x  %-34s | %s"
                      % (case, x.address, "%s %s" % (x.mnemonic, x.op_str),
                         "%s %s" % (y.mnemonic, y.op_str)))
    print("rom %d instrs, ours %d" % (len(list(casediff.MD.disasm(rom, 0))),
                                      len(list(casediff.MD.disasm(ours, 0)))))
    print("aligned cases: %d   unaligned (wrong length): %s"
          % (len(rb) - len(unaligned), " ".join("0x%02x" % c for c in unaligned)))
    print("differing: " + "  ".join("%s=%d" % (k, v) for k, v in sorted(counts.items())))
