"""Reorder the top-level case clauses of one switch into numeric order, byte for byte.

mwcc emits case bodies in source order, so a clause written out of sequence lands its body in the
wrong place and every jump-table entry after it diverges even when every body is the right size.
This moves whole clauses and never rewrites their text.

A clause begins at the first top-level `case` label after a body line; consecutive labels share it.

With an order file (one case value per line, as the ROM's jump table lists the bodies) the clauses
are placed in the ORIGINAL order, which is numeric only by habit: DQIX's writer put 0x99 next to
0x6d and 0xc0 next to 0xa2.

Usage: python reorder_cases.py <in.cpp> <out.cpp> [order.txt]
"""
import re
import sys

LABEL = re.compile(r"^    (?:case (0x[0-9a-fA-F]+)|default)\s*:")

src, dst = sys.argv[1], sys.argv[2]
lines = open(src, encoding="utf-8", newline="\n").read().split("\n")

start = next(i for i, l in enumerate(lines) if l.strip() == "switch (msg->cmd) {")
end = next(i for i in range(len(lines) - 1, start, -1) if lines[i].rstrip() == "    }")
head, body, tail = lines[:start + 1], lines[start + 1:end], lines[end:]

clauses, prev_was_label = [], False
for line in body:
    m = LABEL.match(line)
    if m and not prev_was_label:
        clauses.append({"keys": [], "lines": []})
    if m:
        clauses[-1]["keys"].append(int(m.group(1), 16) if m.group(1) else 1 << 30)
    if not clauses:
        head.append(line)
        continue
    clauses[-1]["lines"].append(line)
    prev_was_label = bool(m)

rank = {}
if len(sys.argv) > 3:
    for n, line in enumerate(open(sys.argv[3], encoding="utf-8")):
        if line.strip():
            rank[int(line.strip(), 16)] = n


def key(c):
    ranked = [rank[k] for k in c["keys"] if k in rank]
    return (min(ranked) if ranked else 1 << 30, min(c["keys"]))


ordered = sorted(clauses, key=key)
open(dst, "w", encoding="utf-8", newline="\n").write(
    "\n".join(head + [l for c in ordered for l in c["lines"]] + tail))
print("%d clauses; first %s last %s"
      % (len(ordered), hex(min(ordered[0]["keys"])), hex(min(ordered[-1]["keys"]))))
