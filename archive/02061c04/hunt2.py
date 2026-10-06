"""Second bounded search on 02061c04: the (battle -> level) register chain, and more pragmas.

hunt_d3.py established that every 0xd3 form pays for the register fix with either the wrong peel or
an extra instruction, and that prop/cse/peephole pragmas are inert. This one attacks the OTHER end
of case 0xe7: the ROM recycles battle's register for `level` (`mov r5,r0` ... `mov r5,#0`), so the
r4/r5 decision may belong to that chain rather than to battle itself.

Baseline: 600 = shape 0, reg 24 (4 in 0xd3, 20 in 0xe7), sp 0, unaligned 9.

Usage: python pad/hunt2.py [--only pragma|level|d3]
"""
import os
import subprocess
import sys

SP = os.path.dirname(os.path.dirname(os.path.abspath(__file__))).replace(chr(92), "/")
BASE = f"{SP}/c04work/c04.cpp"
OUT = f"{SP}/handwork/hunt2"
os.makedirs(OUT, exist_ok=True)

LEVEL_DECL = "            int level = 0;"
LEVEL_SET = "                level = *(*(unsigned short **)((char *)comb + 0x134) + 0x18);"
LEVEL_USE = "            snap->d_hi = level;"
COMB = "            void *comb = _Z24GetCombatantAtField0x3acP12BattleStruct(battle);"
SNAP = "        struct SnapshotE7 *snap = (struct SnapshotE7 *)(rec + snapOff);"
D3 = "        ((Foo::Bar *)((char *)func_02012fe4() + 0x1840))->num |= (one << bit);"

# One more pragma round. opt_dead_assignments is the lever that closed ov017:021d4e38, and it has
# never been tried here; the rest are the remaining mwcc knobs that can move allocation.
PRAGMAS = {
    "dead_assign_off": "#pragma opt_dead_assignments off\n",
    "strength_off": "#pragma opt_strength_reduction off\n",
    "lifetimes_off": "#pragma opt_lifetimes off\n",
    "unroll_off": "#pragma opt_unroll_loops off\n",
    "vector_off": "#pragma opt_vectorize_loops off\n",
    "inline_zero": "#pragma inline_max_size(0)\n",
    "no_register_col": "#pragma register_coloring off\n",
}

# The (battle -> level) chain: same code, different shape for `level`.
LEVEL = {
    "level_ushort": (LEVEL_DECL, "            unsigned short level = 0;"),
    "level_uint": (LEVEL_DECL, "            unsigned int level = 0;"),
    "level_short": (LEVEL_DECL, "            short level = 0;"),
    "level_uncommitted": (LEVEL_DECL, "            int level;"),
    "level_after_comb": (LEVEL_DECL, ""),          # paired with an insert below
}

D3_MORE = {
    # index-scaled address: the outer offset as an array step of the INNER type
    "d3_index": """        ((Foo::Bar *)((char *)func_02012fe4() + 0x1840))->num |= (one << bit);
        if (bit == 0x7fffffff) return 0;""",
    # the loaded value parked in a local that survives the store
    "d3_keep_value": """        Foo::Bar *p = (Foo::Bar *)((char *)func_02012fe4() + 0x1840);
        unsigned int v = p->num | (one << bit);
        p->num = v;
        if (v == 0x7fffffff) return 0;""",
    # named intermediate for the outer step only
    "d3_mid": """        char *mid = (char *)func_02012fe4() + 0x1800;
        ((Foo::Bar *)(mid + 0x40))->num |= (one << bit);""",
}


def score(path):
    r = subprocess.run([sys.executable, f"{SP}/pad/casescore.py", path],
                       capture_output=True, text=True, cwd=f"{SP}/..", stdin=subprocess.DEVNULL)
    out = ((r.stdout or "") + (r.stderr or "")).strip().splitlines()
    return out[-1] if out else "(no output)"


base = open(BASE, encoding="utf-8").read()
only = sys.argv[sys.argv.index("--only") + 1] if "--only" in sys.argv else ""
print("baseline           ", score(BASE), flush=True)

if only in ("", "pragma"):
    for name, prag in PRAGMAS.items():
        text = base.replace("// USA: func_02061c04", prag + "// USA: func_02061c04", 1)
        p = f"{OUT}/c04_{name}.cpp"
        open(p, "w", encoding="utf-8", newline="\n").write(text)
        print("%-18s %s" % (name, score(p)), flush=True)

if only in ("", "level"):
    for name, (old, new) in LEVEL.items():
        text = base.replace(old, new, 1)
        if name == "level_after_comb":
            text = text.replace(COMB, COMB + "\n            int level = 0;", 1)
        if name == "level_uncommitted":
            text = text.replace(LEVEL_USE, "            snap->d_hi = comb ? level : 0;", 1)
        p = f"{OUT}/c04_{name}.cpp"
        open(p, "w", encoding="utf-8", newline="\n").write(text)
        print("%-18s %s" % (name, score(p)), flush=True)
    # snap computed from battle instead of rec -- costs an add, but may flip the pair
    text = base.replace(SNAP, "        struct SnapshotE7 *snap = (struct SnapshotE7 *)(battle + 0x75d0);", 1)
    p = f"{OUT}/c04_snap_from_battle.cpp"
    open(p, "w", encoding="utf-8", newline="\n").write(text)
    print("%-18s %s" % ("snap_from_battle", score(p)), flush=True)

if only in ("", "d3"):
    for name, body in D3_MORE.items():
        text = base.replace(D3, body, 1)
        p = f"{OUT}/c04_{name}.cpp"
        open(p, "w", encoding="utf-8", newline="\n").write(text)
        print("%-18s %s" % (name, score(p)), flush=True)
