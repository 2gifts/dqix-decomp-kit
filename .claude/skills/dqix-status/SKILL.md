---
name: dqix-status
description: Answer "where are we?" on the DQIX decomp — one command produces fleet state, coverage, resume tally, spend, and what changed since the last report. Use when the user asks for a status check, a progress report, "what's happening", "any progress", or invokes /dqix-status.
---

# DQIX status report

The user is asking because they have not heard anything in a while. Give them movement and
problems, not an inventory.

    KIT   the kit checkout (scripts, docs, skills): $DQIX_KIT when set, else this session's
          working directory
    SP    the state directory (attempts, logs, claims, staging): `python $KIT/kitpaths.py state`
    REPO  $DQIX_REPO, or ../dqix-decomp

## Get the data — one call

    python $KIT/progress.py --print

It rewrites `STATE.md` from disk — fleet counts, stop flags, coverage in bytes and functions,
remaining work per size band, HEAD, staged-not-committed, selfcheck/regress, the last verdicts — and
prints it. Give the delta against what the user was last told; never repeat unchanged numbers. Run
`python $KIT/bandcost.py` only if the user asks about money.

Everything comes from disk. Do not re-derive any of it with separate calls, and do not describe
state from the conversation — a driver can have died since the last thing you said.

## Say this, in this order

1. **Running or not.** Drivers and workers, from the process count — never from what you believe you
   launched. If `drivers=0` and `workers=0` with no `FLEET_STOPPED` flag, the run **died**; say so
   first, then check the tail of `$SP/wlog/pull_all.log` for the reason.
2. **What moved since the last report** — coverage delta, matched/attempted delta, commits landed.
   If nothing moved, say that plainly and give the reason; "no progress" is a valid report and much
   more useful than a restated total.
3. **Cost per match so far**, when a worker has run. The current reference is one sonnet slot on the
   large band, 2026-08-25: 4 functions, 2 matched, **$14.14** — and the last two ran $2.62 then
   **$0.57**, because they were siblings (RijndaelEncrypt/Decrypt). Cite a sibling run as the reason
   for a cheap number rather than as the new normal. Resume/backlog work is CLOSED, so ignore any
   older "$1.08 on resume" figure.
4. **Anything needing a decision** — a phase gate reached, a stall, `pending integration` sitting
   uncommitted (only a commit proves a match; staged files are stranded work), or a usage limit.
5. Nothing else. No re-listing of totals the user already has.

## Judgment calls worth flagging without being asked

* `pending integration` non-empty while nothing is running — run `finish_wave.sh <mod>` for one
  module at a time, in the background, and say you did.
* A `TRUNCATED` doc on the last worker session (`832/1876`) — that run measured nothing; do not
  quote its verdict as evidence for or against anything.
* Any `WARN recipe_select fell back` in the worker log.
* **Cost per match sustained above ~$8 on the large band** — the stop rule. One expensive miss is
  normal (a 996B function missed at $6.99 the same evening two others matched for $3.19 combined);
  a run of them is the signal.
* **A staged file that is already committed.** `finish_wave` can commit a match and leave the copy in
  `staging/<mod>/`, which then reads as pending work forever. Check the address against `src/` before
  reporting anything as awaiting integration.
* **`selfcheck.py` not all green.** That is a real red — there are no standing exceptions.

## Length

Under ten lines unless something is broken. A status check is not a place to re-explain the plan.
