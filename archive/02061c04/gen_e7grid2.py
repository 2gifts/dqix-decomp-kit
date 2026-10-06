"""Second case-0xe7 grid: the INNER block, not the address chain.

24 rearrangements of the address chain all left the residue at 20 bytes. Case 0xbf was cracked by
changing the consumer code (the loop body), not the declarations, so this grid moves the three
values defined inside the guard -- `fields`, `level`, `comb` -- which is where `battle`'s only
callee-saved use lives.

Usage: python gen_e7grid2.py <out-dir>
"""
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

F = "void *fields = _Z17GetPtrField0x2a04P12BattleStruct(battle);"
L = "unsigned short level = 0;"
C = "void *comb = _Z24GetCombatantAtField0x3acP12BattleStruct(battle);"
SET = "level = *(unsigned short *)((char *)*(void **)((char *)comb + 0x134) + 0x30);"

V = {
    "base": [F, L, C, "if (comb)", "    " + SET],
    "level_first": [L, F, C, "if (comb)", "    " + SET],
    "comb_first": [C, L, F, "if (comb)", "    " + SET],
    "comb_inline": [F, L,
                    "void *comb = _Z24GetCombatantAtField0x3acP12BattleStruct(battle);",
                    "if (comb != 0)", "    " + SET],
    "level_int": [F, "int level = 0;", C, "if (comb)", "    " + SET],
    "copy_battle": ["void *b2 = battle;",
                    "void *fields = _Z17GetPtrField0x2a04P12BattleStruct(b2);", L,
                    "void *comb = _Z24GetCombatantAtField0x3acP12BattleStruct(b2);",
                    "if (comb)", "    " + SET],
    "fields_last": [L, C, "if (comb)", "    " + SET, F],
    "split_load": [F, L, C, "if (comb) {",
                   "    void *cp = *(void **)((char *)comb + 0x134);",
                   "    level = *(unsigned short *)((char *)cp + 0x30);", "}"],
    "decl_split": ["void *fields;", "unsigned short level = 0;", "void *comb;",
                   "fields = _Z17GetPtrField0x2a04P12BattleStruct(battle);",
                   "comb = _Z24GetCombatantAtField0x3acP12BattleStruct(battle);",
                   "if (comb)", "    " + SET],
    "level_after": [F, C, "unsigned short level = 0;", "if (comb)", "    " + SET],
    "guard_else": [F, C, "unsigned short level;",
                   "if (comb)", "    " + SET, "else", "    level = 0;"],
}

for name, lines in V.items():
    open(os.path.join(OUT, "to_%s.txt" % name), "w", encoding="utf-8").write(
        "".join("            %s\n" % l for l in lines))
print("wrote %d variants to %s" % (len(V), OUT))
