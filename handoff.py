"""Carry a failed session's findings into the next session on the same address.

A worker doc is the ONLY file a worker is told to read, so it is the only channel a session that ran
out of context has for what it learned. Nothing wrote to that channel: 0204cd60 spent $3.44 deriving
a struct layout, six callee signatures and 20 of 33 tag handlers, said so in its verdict, and the
next claim regenerated the doc over it. Functions large enough to need more than one context window
therefore restarted from the scaffold every time and could never converge.

`recipe_select.py` preserves the block this writes. Newest note first; the block is capped, so the
oldest note is what falls off.

Usage: python handoff.py <doc.md> <addr> <session.json> [size]
"""
import json
import os
import sys
import time

MARK = "## HANDOFF FROM THE PREVIOUS SESSION ON THIS ADDRESS"
# An explicit end sentinel, so a session's recon can carry its own `##` headings inside the block.
END = "<!-- END HANDOFF -->"
CAP = 16000
NOTE_CAP = 9000

doc, addr, sess = sys.argv[1], sys.argv[2], sys.argv[3]
size = sys.argv[4] if len(sys.argv) > 4 else "?"

# NEVER append to the SHARED static doc. `pull_worker` falls back to it when recipe_select fails,
# and a per-address handoff written there would reach every worker on every function.
if addr.lower() not in os.path.basename(doc).lower():
    sys.exit(0)

try:
    result = json.load(open(sess, encoding="utf-8", errors="ignore")).get("result") or ""
except (IOError, ValueError):
    sys.exit(0)
result = result.strip()
if len(result) < 200:                 # a bare "SKIP <addr>" carries nothing worth a slot
    sys.exit(0)
if len(result) > NOTE_CAP:
    result = result[:NOTE_CAP] + "\n[note truncated]"

try:
    text = open(doc, encoding="utf-8", errors="ignore").read()
except IOError:
    sys.exit(0)

i = text.find(MARK)
old = ""
if i >= 0:
    j = text.find(END, i)
    if j < 0:
        j2 = text.find("\n## ", i + len(MARK))
        old = text[i:] if j2 < 0 else text[i:j2 + 1]
        text = text[:i] + ("" if j2 < 0 else text[j2 + 1:])
    else:
        old = text[i:j]
        text = text[:i] + text[j + len(END):]
    old = old[len(MARK):].lstrip("\n")
    k = old.find("\n### ")
    old = old[k + 1:] if k >= 0 else ""

note = ("%s\n\nRead this before the listing: it is what the last session on this address learned\n"
        "before it ran out of context. Do NOT re-derive it. Continue from it.\n\n"
        "### %s  %s bytes  (%s)\n\n%s\n\n" % (MARK, addr, size, time.strftime("%Y-%m-%d %H:%M"),
                                              result))
body = note + old
if len(body) > CAP:
    body = body[:CAP].rstrip() + "\n[older notes dropped]\n"

open(doc, "w", encoding="utf-8").write(text.rstrip() + "\n\n" + body.rstrip() + "\n" + END + "\n")
print("handoff %d chars -> %s" % (len(body), doc))
