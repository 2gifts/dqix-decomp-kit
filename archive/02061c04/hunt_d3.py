"""Bounded search for the last two residues of 02061c04: file-wide pragmas and 0xd3 shapes.

Each variant is scored with casescore (one compile) against the baseline. Baseline for the current
source is 600 = shape 0, reg 24, sp 0, unaligned 9 -- 4 register rows in case 0xd3 and 20 in 0xe7.

Usage: python pad/hunt_d3.py [--only pragma|shape]
"""
import os
import subprocess
import sys

SP = os.path.dirname(os.path.dirname(os.path.abspath(__file__))).replace(chr(92), "/")
BASE = f"{SP}/handwork/c04_27.cpp"
OUT = f"{SP}/handwork/hunt"
os.makedirs(OUT, exist_ok=True)

D3_CURRENT = "        ((Foo::Bar *)((char *)func_02012fe4() + 0x1840))->num |= (one << bit);"

PRAGMAS = {
    "prop_off": "#pragma opt_propagation off\n",
    "cse_off": "#pragma opt_common_subs off\n",
    "opt4": "#pragma optimization_level 4\n",
    "opt3": "#pragma optimization_level 3\n",
    "peep_off": "#pragma peephole off\n",
}

SHAPES = {
    # the mask precomputed into a local instead of folded into the orr
    "mask_local": """        int mask = one << bit;
        ((Foo::Bar *)((char *)func_02012fe4() + 0x1840))->num |= mask;""",
    # named pointer kept alive after the store -- extra pressure on the pointer's register
    "ptr_live": """        Foo::Bar *p = (Foo::Bar *)((char *)func_02012fe4() + 0x1840);
        unsigned int v = p->num;
        p->num = v | (one << bit);
        if (!p) return 0;""",
    # operand order flipped through one named pointer (no second call)
    "orr_flip": """        Foo::Bar *p = (Foo::Bar *)((char *)func_02012fe4() + 0x1840);
        p->num = (one << bit) | p->num;""",
    # value named, address left to the compiler, store through the same chain
    "value_named": """        Foo *foo = (Foo *)func_02012fe4();
        unsigned int v = foo->bar.num;
        foo->bar.num = v | (one << bit);""",
    # the field as a bitfield write rather than a word OR
    "bitfield": """        struct BarBits { unsigned int w; };
        BarBits *bb = (BarBits *)((char *)func_02012fe4() + 0x1840 + 0xb4c);
        bb->w |= (one << bit);""",
    # address held in a second local so two pointers compete for r1/r2
    "two_ptrs": """        char *raw = (char *)func_02012fe4();
        Foo::Bar *p = (Foo::Bar *)(raw + 0x1840);
        p->num |= (one << bit);
        if (raw == 0) return 0;""",
}


def score(path):
    r = subprocess.run([sys.executable, f"{SP}/pad/casescore.py", path],
                       capture_output=True, text=True, cwd=f"{SP}/..", stdin=subprocess.DEVNULL)
    return ((r.stdout or "") + (r.stderr or "")).strip().splitlines()[-1] if (r.stdout or r.stderr) else "(no output)"


base = open(BASE, encoding="utf-8").read()
if D3_CURRENT not in base:
    print("BASE does not contain the expected 0xd3 statement -- update D3_CURRENT")
    sys.exit(1)
only = sys.argv[sys.argv.index("--only") + 1] if "--only" in sys.argv else ""

print("baseline           ", score(BASE), flush=True)

if only in ("", "pragma"):
    anchor = "// USA: func_02061c04"
    for name, prag in PRAGMAS.items():
        text = base.replace(anchor, prag + anchor, 1)
        p = f"{OUT}/c04_{name}.cpp"
        open(p, "w", encoding="utf-8", newline="\n").write(text)
        print("%-18s %s" % (name, score(p)), flush=True)

if only in ("", "shape"):
    for name, body in SHAPES.items():
        text = base.replace(D3_CURRENT, body, 1)
        p = f"{OUT}/c04_{name}.cpp"
        open(p, "w", encoding="utf-8", newline="\n").write(text)
        print("%-18s %s" % (name, score(p)), flush=True)
