"""Re-derive every recorded resume verdict from the session JSON that produced it.

    python fix_verdicts.py            report disagreements
    python fix_verdicts.py --apply    rewrite wlog/resume_done.txt

resume_sweep.sh classified a session by matching MATCH|matched in its result text, so a worker that
finished with `PASS <addr>` was filed as a miss. That is the expensive direction of the error:
resume_done.txt is what stops an address being served again, so a real match recorded as a miss is
a paid-for conversion thrown away and never retried. Measured 08-20: 0203af48 matched for $0.38 and
was recorded as a miss.

Run this after any sweep, never during one -- the sweep appends to the same file.
"""
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
import json, os, re, sys

SP = _kp.SP
KIT = _kp.KIT
DONE = f"{SP}/wlog/resume_done.txt"
# What a worker actually writes when it succeeds. PASS and OK are the two the pipeline's own gate
# prints, so a verdict parser that only knows MATCH is narrower than the tools it reads.
WIN = re.compile(r'\bMATCH\b|\bmatched\b|^\s*PASS\b|\bGATE\s*OK\b', re.I | re.M)
LOSE = re.compile(r'\bSKIP\b|\bMISS\b|\bBYTEDIFF\b|\bSIZE\b|\bUNDEF\b', re.I)


def verdict_of(mod, addr):
    """MATCH / miss / None when the session JSON is missing or unreadable."""
    p = f"{SP}/wlog/resume_{mod}_{addr}.json"
    try:
        res = json.load(open(p, encoding="utf-8")).get("result", "")
    except Exception:
        return None
    # A SKIP line usually quotes the word MATCH while explaining what did not match, so a losing
    # marker anywhere outranks a winning one.
    if LOSE.search(res):
        return "miss"
    return "MATCH" if WIN.search(res) else "miss"


def main():
    apply = "--apply" in sys.argv
    try:
        lines = open(DONE, encoding="utf-8").read().splitlines()
    except OSError:
        print("no resume_done.txt")
        return
    out, changed = [], 0
    for ln in lines:
        f = ln.split()
        if len(f) < 3:
            out.append(ln)
            continue
        addr, mod, rec = f[0], f[1], f[2]
        real = verdict_of(mod, addr)
        if real and real != rec:
            changed += 1
            print(f"{addr} {mod}: recorded {rec} -> actually {real}")
            f[2] = real
            ln = " ".join(f)
        out.append(ln)
    print(f"{changed} disagreement(s) over {len(lines)} record(s)")
    if apply and changed:
        open(DONE, "w", encoding="utf-8", newline="\n").write("\n".join(out) + "\n")
        print(f"rewrote {DONE}")


if __name__ == "__main__":
    main()
