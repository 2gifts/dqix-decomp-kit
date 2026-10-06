"""Make case 0xe7's `battle` a FUNCTION-SCOPE variable instead of a case-local.

Several cases call GetBattleStruct, so the original may well have had one `void *battle;` at the top
of the function. That changes where its register home is allocated relative to `rec`, which is the
one thing every in-case rewrite failed to move.

Usage: python e7fnbattle.py <src> <out>
"""
import sys

src, out = sys.argv[1:3]
s = open(src, encoding="utf-8").read()

anchor = "    } lr;\n"
if s.count(anchor) != 1:
    sys.exit("function-scope block anchor not found")
s = s.replace(anchor, anchor + "    void *fnBattle;\n")

i = s.index("    case 0xe7: {")
j = s.index("    case 0xe8:", i)
body = s[i:j]
body = body.replace("void *battle = _Z15GetBattleStructv();", "fnBattle = _Z15GetBattleStructv();")
body = body.replace("(char *)battle", "(char *)fnBattle")
body = body.replace("P12BattleStruct(battle)", "P12BattleStruct(fnBattle)")
if "battle" in body.replace("fnBattle", "").replace("BattleStruct", "").replace("BattleField", ""):
    sys.exit("a bare `battle` survived in the case body")
open(out, "w", encoding="utf-8").write(s[:i] + body + s[j:])
print("wrote", out)
