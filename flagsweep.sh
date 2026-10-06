#!/bin/bash
# Which compiler settings produce THIS function? Library objects linked into the ROM were not
# necessarily built with the project's own flags: a routine that branches to a shared `bx lr`
# where our -O2 predicates the return is a different optimisation level, not a different source.
# Usage: flagsweep.sh <OV|main> <addr> <file.cpp>
SP="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
REPO="${DQIX_REPO:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && { pwd -W 2>/dev/null || pwd; })/dqix-decomp}"
OV="$1"; ADDR="$2"; SRC="$3"
cd "$REPO" || exit 2

sets=(
  ""
  "-O0"
  "-O1"
  "-O2"
  "-O3"
  "-O4"
  "-Os"
  "-O2 -ipa off"
  "-O4,p"
  "-O4,s"
  "-O2 -sdatathreshold 0"
  "-O2 -opt noschedule"
  "-O2 -opt nolifetimes"
  "-O2 -opt nopeephole"
)

for f in "${sets[@]}"; do
  out=$(WDIFF_FLAGS="$f" python "$SP/wdiff.py" "$OV" "$ADDR" "$SRC" </dev/null 2>&1 | head -1)
  printf '%-24s %s\n' "[${f:-default}]" "$out"
done
echo "FLAGSWEEP DONE"
