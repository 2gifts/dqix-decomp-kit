"""Case-0xe4's last two registers: does a block boundary flip the scratch DIRECTION?

With `s` pinned to r2 by the shift-first form, the pool and the byte are allocated over {r1, r3} and
we take them descending (pool r3, byte r1) where the ROM takes them ascending (pool r1, byte r3).
pad/probe_d3d.cpp's q1 showed a guarded block flipping exactly that direction, and the 0xbf crack
proved a probe's answer has to be re-measured against the real function anyway.

Usage: python gen_e4nest.py <out-dir>
"""
import os
import sys

OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

BODY = ["unsigned char *p = (unsigned char *)&data_02109bf4 + 0xc8;",
        "unsigned int v = (unsigned int)(msg->p1 == 0);",
        "unsigned int s = (v << 31) >> 29;",
        "unsigned int b = *p;",
        "*p = (unsigned char)((b & ~4u) | s);"]

WRAP = {
    "block": ("{", "}"),
    "dowhile": ("do {", "} while (0);"),
    "forloop": ("for (;;) {", "    break;\n        }"),
    "whileone": ("while (1) {", "    break;\n        }"),
}

for name, (open_, close) in WRAP.items():
    lines = ["        %s" % open_]
    lines += ["            %s" % l for l in BODY]
    lines.append("        %s" % close)
    lines.append("        return 1;")
    open(os.path.join(OUT, "to_%s.txt" % name), "w", encoding="utf-8").write(
        "\n".join(lines) + "\n")

# and the reverse: hoist the three locals OUT of the case, to function scope order, by declaring
# them before the switch is not possible -- but a nested inner block for only the tail is.
lines = ["        %s" % BODY[0], "        %s" % BODY[1],
         "        {", "            %s" % BODY[2], "            %s" % BODY[3],
         "            %s" % BODY[4], "        }", "        return 1;"]
open(os.path.join(OUT, "to_tailblock.txt"), "w", encoding="utf-8").write("\n".join(lines) + "\n")

lines = ["        %s" % BODY[0], "        {", "            %s" % BODY[1],
         "            %s" % BODY[2], "            %s" % BODY[3], "            %s" % BODY[4],
         "        }", "        return 1;"]
open(os.path.join(OUT, "to_afterptr.txt"), "w", encoding="utf-8").write("\n".join(lines) + "\n")
print("wrote %d variants to %s" % (len(WRAP) + 2, OUT))
