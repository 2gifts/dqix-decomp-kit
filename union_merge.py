#!/usr/bin/env python
"""Semantic union-merge for dsd config files when merging a human-authored branch.

git's line merge is wrong for these files: delinks.txt and symbols.txt are
unordered records keyed by path/address, not sequential prose. A textual conflict
here is almost always "both sides appended different records", which is a union,
not a choice.

Usage: python union_merge.py <conflicted-path> [...]   (run inside the merge)
"""
import os
import re
import subprocess
import sys

ADDR = re.compile(r"addr:0x([0-9a-fA-F]+)")
PLACEHOLDER = re.compile(r"^(func|data|lbl|jumptable)_(ov\d+_)?[0-9a-fA-F]{6,8}$")
TEXT_RANGE = re.compile(r"\.text\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)")


def stage(n, path):
    r = subprocess.run(["git", "show", ":%d:%s" % (n, path)], capture_output=True, text=True)
    return r.stdout if r.returncode == 0 else None


def parse_delinks(text):
    """-> (preamble_lines, [(path, [lines])]); a block starts at an unindented 'path:' line."""
    pre, blocks, cur = [], [], None
    for line in text.splitlines():
        if line and not line[0].isspace() and line.rstrip().endswith(":"):
            cur = (line.rstrip()[:-1], [])
            blocks.append(cur)
        elif cur is None:
            pre.append(line)
        else:
            cur[1].append(line)
    return pre, blocks


def block_range(lines):
    for l in lines:
        m = TEXT_RANGE.search(l)
        if m:
            return int(m.group(1), 16), int(m.group(2), 16)
    return None


def merge_delinks(ours, theirs):
    """Union by source path.

    dsd rejects any two delink blocks whose .text ranges OVERLAP, not merely
    ranges that are equal -- the human branch groups a whole class into one file
    (BackgroundLoader.cpp spans 0x202f700..0x2030634) where our workers emitted
    one file per function inside that span. So overlap, not equality, is the
    collision test. Theirs wins: their single file implements every function in
    the span, and each ours-side file it displaces is reported as DROP-FILE so
    the caller deletes it -- left on disk it is an orphan nothing builds.

    Theirs wins only when their file is actually ON DISK after the merge. Their
    record may name a path we have since renamed or absorbed (src/System/Random.cpp
    -> src/Util/Random.cpp), and git resolves the rename in the tree while this
    file still carries the old name; taking theirs there deletes a source we build
    and leaves delinks.txt pointing at nothing.
    """
    pre, ob = parse_delinks(ours)
    _, tb = parse_delinks(theirs)
    ours_paths = set(p for p, _ in ob)
    by_path, dropped = {}, []
    for blocks in (ob, tb):                       # theirs second, so theirs wins
        for path, lines in blocks:
            rng = block_range(lines)
            if rng:
                displaced = []
                for other, olines in list(by_path.items()):
                    orng = block_range(olines)
                    if other != path and orng and rng[0] < orng[1] and orng[0] < rng[1]:
                        displaced.append((other, orng))
                if displaced and not os.path.exists(path) \
                   and any(os.path.exists(o) for o, _ in displaced):
                    continue
                for other, orng in displaced:
                    dropped.append((path, other, orng))
                    by_path.pop(other)
            by_path[path] = lines
    out = list(pre)
    for path, lines in sorted(by_path.items(), key=lambda kv: (block_range(kv[1]) or (1 << 32, 0))):
        out.append(path + ":")
        out.extend(lines)
        if out[-1].strip():
            out.append("")
    body = "\n".join(out).rstrip() + "\n"
    return body, [d for d in dropped if d[1] in ours_paths]


def merge_symbols(base, ours, theirs):
    """Union by address. On a name collision a REAL name always beats a
    func_02xxxxxx placeholder, whichever side it came from; only when both sides
    carry a real name does theirs win.

    A flat theirs-wins rule is wrong here, and was measured to be: the human
    branch predates ~11k of our matched functions, so it still calls them
    func_02xxxxxx. Taking theirs blindly renamed 4,185 already-named symbols back
    to placeholders, which unnames every source file that calls them.

    A pure union is also wrong: a symbol in the BASE that one side DELETED was
    deleted on purpose, and re-adding it makes `dsd check symbols` fail with
    "not found in linked binary" (func_0202f980, data_02104304) or "expected to
    be global but is local" (AllocatorTree::topLevel, which the human branch made
    static). So consult the base and honour deletions, as a 3-way merge should.
    """
    def addrs(text):
        out = set()
        for line in text.splitlines():
            m = ADDR.search(line)
            if m:
                out.add(int(m.group(1), 16))
        return out

    base_a, ours_a, theirs_a = addrs(base), addrs(ours), addrs(theirs)
    deleted = ((base_a - theirs_a) & ours_a) | ((base_a - ours_a) & theirs_a)

    by_addr, aliases, plain, renamed = {}, {}, [], []
    for text in (ours, theirs):
        first_here = set()
        for line in text.splitlines():
            m = ADDR.search(line)
            if not m:
                if line.strip() and line not in plain:
                    plain.append(line)
                continue
            a = int(m.group(1), 16)
            if a in first_here:
                aliases.setdefault(a, []).append(line)
                continue
            first_here.add(a)
            prev = by_addr.get(a)
            if prev is None:
                by_addr[a] = line
                continue
            pn, nn = prev.split()[0], line.split()[0]
            if pn == nn:
                continue
            if PLACEHOLDER.match(pn) or not PLACEHOLDER.match(nn):
                by_addr[a] = line
                renamed.append((a, pn, nn))
    for a in deleted:
        by_addr.pop(a, None)
        aliases.pop(a, None)
    out = list(plain)
    for a in sorted(by_addr):
        out.append(by_addr[a])
        kept = {by_addr[a].split()[0]}
        for line in aliases.get(a, []):
            if line.split()[0] not in kept:
                out.append(line)
                kept.add(line.split()[0])
    notes = renamed + [(a, "<deleted on one side>", "") for a in sorted(deleted)]
    return "\n".join(out) + "\n", notes


for path in sys.argv[1:]:
    ours, theirs = stage(2, path), stage(3, path)
    if ours is None or theirs is None:
        print("SKIP %s: not a two-sided conflict" % path)
        continue
    if path.endswith("delinks.txt"):
        text, notes = merge_delinks(ours, theirs)
        for kept, gone, r in notes:
            print("  range 0x%08x-0x%08x now covered by %s" % (r[0], r[1], kept))
            print("DROP-FILE %s" % gone)          # machine-readable for the caller
    elif path.endswith("symbols.txt"):
        text, notes = merge_symbols(stage(1, path) or "", ours, theirs)
        for a, o, n in notes[:15]:
            print("  0x%08x: %s -> %s" % (a, o, n))
        if len(notes) > 15:
            print("  ... %d more renames" % (len(notes) - 15))
    else:
        print("SKIP %s: no union rule" % path)
        continue
    # Explicit utf-8: without it Python uses the Windows locale codepage, so any non-ASCII byte in a
    # symbol name would be mangled or raise -- every other writer in the pipeline pins the encoding.
    open(path, "w", encoding="utf-8", newline="\n").write(text)
    print("UNION %s: %d lines" % (path, len(text.splitlines())))
