#!/bin/bash
# Identify every unmatched overlay function against the reference decompilations, one overlay at a
# time. Library code is not confined to the arm9 bands -- NNS graphics and sound, DWC wifi and the
# filesystem live in overlays -- so the bands order the search, they do not bound it.
SP="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
cd "$SP" || exit 1
for o in $(cat "$SP/wlog/ovlist.txt"); do
    python -u "$SP/sdkident.py" sweep "$o" > "$SP/wlog/ident_ov$o.txt" 2>&1
    echo "$(date +%H:%M) ov$o $(grep -c '^EXACT\|^CLOSE' "$SP/wlog/ident_ov$o.txt")" \
        >> "$SP/wlog/ident_ov_progress.txt"
done
echo done >> "$SP/wlog/ident_ov_progress.txt"
