"""Per-case size delta for every case in 02061c04, from ONE compile.

casediff.py answers "what is wrong inside case X" but recompiles per case, so surveying 134 cases
costs 134 compiles. This compiles once and prints every case whose body size differs, which is the
only way to see OVER-emission: a case nobody has looked at can be too long, and the function total
then hides a short case behind it.

Usage: python casetable.py [src.cpp] [--show c1,c2,...]

--show prints the full side-by-side body of each named case from the SAME compile, so surveying and
reading N cases costs one compile instead of N + 1.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import casediff

args = [a for a in sys.argv[1:] if not a.startswith("--")]
show = []
for a in sys.argv[1:]:
    if a.startswith("--show"):
        show = [int(x, 16) for x in a.split("=", 1)[1].split(",")]

src = args[0] if args else f"{casediff.SP}/c04work/c04.cpp"
rom, ours = casediff.rom_text(), casediff.our_text(src)
for c in show:
    casediff.show(c, rom, ours)
    print()
rb, ob = casediff.bodies(rom), casediff.bodies(ours)

short = over = 0
for case in sorted(rb):
    rl = rb[case][1]
    ol = ob.get(case, (0, 0))[1]
    if rl == ol:
        continue
    d = ol - rl
    short += -d if d < 0 else 0
    over += d if d > 0 else 0
    print("case 0x%02x  ROM %4d  ours %4d  %+d" % (case, rl, ol, d))
print("total ROM 0x%x  ours 0x%x   short %d  over %d" %
      (len(rom), sum(b[1] for b in ob.values()), short, over))
