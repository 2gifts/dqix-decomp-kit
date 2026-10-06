"""Render the worker prompt with a PRIOR-attempt block.

Shared by pull_worker.sh's resume path and resume_one.sh so the two cannot drift; the wording that
tells a session to start from a near-miss lives in exactly one place.
"""
import sys


def ruled_out(sp, addr):
    """Every verdict already recorded against this address, as a do-not-re-test list.

    Handing over the saved SOURCE without the saved VERDICT makes a resume worker re-explore the
    lever space blind. Measured 08-20 on 021e6448: the prior attempt was recorded at 142 bytes with
    the regalloc levers noted as inert, and the resume session spent $2.52 arriving at 140 bytes and
    reporting those same levers inert. It was paying to rediscover a written-down result.
    """
    try:
        sys.path.insert(0, sp)
        import resumable
        best, reason = resumable.skips().get(addr.lower(), (None, ""))
    except Exception:
        return ""
    if not reason:
        return ""
    lines = "".join(f"    - {r.strip()}\n" for r in reason.split(" | ") if r.strip())
    head = ("\nALREADY TRIED ON THIS FUNCTION BY EARLIER SESSIONS. This is a HEAD START, not a warning\n"
            "off. Every lever below is one you do not have to spend your budget re-testing, which is\n"
            "why you can afford to go further than they did:\n")
    tail = ("\nThis function is matchable. It is compiler output from human-written C, so a C form\n"
            "exists; nobody has found it yet. Earlier sessions stopping here is evidence about THEM,\n"
            "not about the function.\n"
            "\nYour job is to find a lever that is NOT on that list. When the listed levers are\n"
            "exhausted, that is the point where the real work starts -- go up a tier: change the data\n"
            "model (bitfield vs mask vs union), change WHERE a value is first used rather than where\n"
            "it is declared, change the control-flow shape (switch vs if-chain, predication vs\n"
            "branch), split or merge the expression so folding has a single consumer, mine the corpus\n"
            "for a MATCHED function that already emits your exact pattern and copy its shape.\n"
            "\nDo NOT skip merely because your gate reproduces the recorded distance -- that only means\n"
            "you have caught up to the prior attempt and are now at the frontier where new work\n"
            "begins. Only SKIP after you have tried something genuinely NEW, and then say exactly what\n"
            "you tried and what it produced, so the next session inherits more than you did.\n")
    if best is not None:
        tail = (f"\nThe best distance recorded so far is {best} bytes -- that is the frontier you start\n"
                f"from, not a limit. Getting to {best} means you have caught up; go past it.\n") + tail
    return head + lines + tail


# Kept importable: regress.py exercises ruled_out() directly, and unpacking argv at module level
# made that impossible. Subprocess callers are unaffected.
def main():
    tpl, out, mod, srcdir, sp, doc, addr, fsize, marg, tag, stage = sys.argv[1:12]
    prior = sys.argv[12] if len(sys.argv) > 12 else ""

    if prior:
        block = (
            "A PREVIOUS SESSION GOT CLOSE AND ITS BEST ATTEMPT IS SAVED AT:\n"
            f"    {prior}\n"
            "START FROM THAT FILE, NOT FROM THE SCAFFOLD. Copy it, then gate it FIRST:\n"
            f"    python {sp}/wgate.py {marg} {addr} <yourfile>\n"
            "That prints exactly what remains. Close THAT residual -- do not rewrite the function from\n"
            "scratch, because the expensive part is already done and a small residual is usually one\n"
            "transformation away. Walk the MANDATORY PRE-SKIP CHECKLIST in the doc by symptom, and\n"
            "remember every pragma has two directions: if the target is the PREDICATED one, the fix is\n"
            "to REMOVE optimize_for_size off, not to add it.\n"
            + ruled_out(sp, addr)
        )
    else:
        block = (
            f"A scaffold with every callee and data name already resolved is at "
            f"{sp}/scaffold/{addr}.cpp -- START FROM IT.\n"
        )

    text = open(tpl, encoding="utf-8").read()
    for k, v in (("{MOD}", mod), ("{SRCDIR}", srcdir), ("{SP}", sp), ("{DOC}", doc),
                 ("{ADDR}", addr), ("{FSIZE}", fsize), ("{MARG}", marg), ("{TAG}", tag),
                 ("{STAGE}", stage), ("{PRIOR}", block)):
        text = text.replace(k, v)
    open(out, "w", encoding="utf-8").write(text)


if __name__ == "__main__":
    main()
