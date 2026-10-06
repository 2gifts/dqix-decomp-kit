#!/bin/bash
# Try every installed mwccarm build (optionally with extra flags) against one function.
# Library code in the ROM was built by whatever toolchain shipped that library, so a function
# whose SHAPE is right but whose size is stubbornly wrong is usually a different compiler build,
# not a different source. Usage: ccsweep.sh <OV|main> <addr> <file.cpp> ["extra flags"]
SP="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
REPO="${DQIX_REPO:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && { pwd -W 2>/dev/null || pwd; })/dqix-decomp}"
OV="$1"; ADDR="$2"; SRC="$3"; EXTRA="$4"
cd "$REPO" || exit 2

for d in tools/mwccarm/*/*/mwccarm.exe; do
  ver=$(echo "$d" | sed 's|tools/mwccarm/||; s|/mwccarm.exe||')
  out=$(MWCC="$ver" WDIFF_FLAGS="$EXTRA" python "$SP/wdiff.py" "$OV" "$ADDR" "$SRC" </dev/null 2>&1 | head -1)
  printf '%-14s %s\n' "$ver" "$out"
done
echo "CCSWEEP DONE"
