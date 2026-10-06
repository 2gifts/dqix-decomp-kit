"""Case-0xe7 PRODUCT grid: conjunctions, not one axis at a time.

Case 0xbf needed three simultaneous changes -- counter first, destination before source, compound
increment split -- and each on its own left the residue exactly where it was. Every 0xe7 grid so far
moved one axis. This one takes the axes that individually keep the case the right length and forms
their product across the whole block, from the call down to the `level` guard.

Usage: python gen_e7prod.py <out-dir>
"""
import itertools
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

CALL = "void *battle = _Z15GetBattleStructv();"
OFF = "int snapOff = 0xcc;"
SNAP = "struct SnapshotE7 *snap = (struct SnapshotE7 *)(rec + snapOff);"
GUARD = "if (!snap->done) {"
SET = "level = *(unsigned short *)((char *)*(void **)((char *)comb + 0x134) + 0x30);"

CHAIN = {
    "e7base": ["char *e7base = (char *)battle + 0x104;", "char *rec = e7base + 0x7400;"],
    "direct": ["char *rec = (char *)battle + 0x104 + 0x7400;"],
}
OFFPOS = ["first", "before_snap"]
LEVELTY = {"ushort": "unsigned short", "int": "int"}
INNER = {
    "base": lambda ty: ["void *fields = _Z17GetPtrField0x2a04P12BattleStruct(battle);",
                        "%s level = 0;" % ty,
                        "void *comb = _Z24GetCombatantAtField0x3acP12BattleStruct(battle);",
                        "if (comb)", "    " + SET],
    "combinline": lambda ty: ["void *fields = _Z17GetPtrField0x2a04P12BattleStruct(battle);",
                              "%s level = 0;" % ty,
                              "void *comb = "
                              "_Z24GetCombatantAtField0x3acP12BattleStruct(battle);",
                              "if (comb != 0)", "    " + SET],
    "splitload": lambda ty: ["void *fields = _Z17GetPtrField0x2a04P12BattleStruct(battle);",
                             "%s level = 0;" % ty,
                             "void *comb = "
                             "_Z24GetCombatantAtField0x3acP12BattleStruct(battle);",
                             "if (comb) {",
                             "    void *cp = *(void **)((char *)comb + 0x134);",
                             "    level = *(unsigned short *)((char *)cp + 0x30);", "}"],
    "copyb": lambda ty: ["void *b2 = battle;",
                         "void *fields = _Z17GetPtrField0x2a04P12BattleStruct(b2);",
                         "%s level = 0;" % ty,
                         "void *comb = _Z24GetCombatantAtField0x3acP12BattleStruct(b2);",
                         "if (comb)", "    " + SET],
}

n = 0
for (cn, chain), op, (tn, ty), (inn, mk) in itertools.product(
        CHAIN.items(), OFFPOS, LEVELTY.items(), INNER.items()):
    head = []
    if op == "first":
        head.append(OFF)
    head.append(CALL)
    head += chain
    if op == "before_snap":
        head.append(OFF)
    head += [SNAP, GUARD]
    text = "".join("        %s\n" % l for l in head)
    text += "".join("            %s\n" % l for l in mk(ty))
    open(os.path.join(OUT, "to_%s_%s_%s_%s.txt" % (cn, op, tn, inn)), "w",
         encoding="utf-8").write(text)
    n += 1
print("wrote %d variants to %s" % (n, OUT))
