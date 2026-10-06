#!/usr/bin/env python3
"""Print ONE recipe section from a worker recipe file.

    python recipe.py worker_main_recipes.md "SCRATCH REGS"
    python recipe.py worker_main_recipes.md --list

Exists so a worker can pay for the recipe it needs instead of the 21k tokens of recipes it does
not. Matching is by heading prefix, case-insensitive, so the index entry can be typed loosely.
"""
import io
import os
import re
import sys

SP = os.path.dirname(os.path.abspath(__file__))


def sections(path):
    text = io.open(path, encoding="utf-8").read()
    parts = re.split(r"(?m)^(## .*)$", text)
    return [(parts[i][3:].strip(), parts[i] + (parts[i + 1] if i + 1 < len(parts) else ""))
            for i in range(1, len(parts), 2)]


def main():
    if len(sys.argv) < 3:
        sys.exit('usage: recipe.py <recipes.md> "<title>" | --list')
    path = sys.argv[1] if os.path.isabs(sys.argv[1]) else f"{SP}/{sys.argv[1]}"
    if not os.path.exists(path):
        sys.exit(f"no such recipe file: {path}")
    secs = sections(path)
    if sys.argv[2] == "--list":
        for t, _b in secs:
            print(t)
        return 0
    want = sys.argv[2].strip().lower()
    hits = [b for t, b in secs if t.lower().startswith(want)] or \
           [b for t, b in secs if want in t.lower()]
    if not hits:
        print(f"no recipe matching {sys.argv[2]!r}. Titles:")
        for t, _b in secs:
            print(" -", t)
        return 1
    print(hits[0].rstrip())
    return 0


if __name__ == "__main__":
    sys.exit(main())
