"""Bring the kit up to date before any work: fetch the published kit and fast-forward to it.

    python kit_update.py

Exit 0: up to date, or updated (then kit_init.py has run). 1: the fetch or kit_init.py failed; the
kit is unchanged or needs the FAIL lines fixed. 2: a fleet or an integration is running, so nothing
was pulled. 3: local changes or local commits block a fast-forward, so nothing was pulled.

Every changed agent instruction file is printed as `RE-READ <path>`: a running session loaded the old
one. $DQIX_KIT_URL and $DQIX_KIT_BRANCH override the source (default ZevyaDev/dqix-decomp-kit, main).
"""
import os
import subprocess
import sys

import kitpaths

SP = kitpaths.SP
URL = os.environ.get("DQIX_KIT_URL", "https://github.com/ZevyaDev/dqix-decomp-kit.git")
BRANCH = os.environ.get("DQIX_KIT_BRANCH", "main")
INSTRUCTIONS = ("AGENTS.md", "CLAUDE.md", ".claude/skills/", ".claude/workflows/", "worker_src/")
SLOW_TRIGGERS = ("colorsweep.py", "wdiff.py", "wgate.py", "regress.py", "regress_fixtures/")
BUSY = ("pull_all.pid", "wave.lock", "claims/INTEGRATING")


def git(*args):
    return subprocess.run(["git", "-C", SP, *args], capture_output=True, text=True)


def main():
    if git("fetch", "-q", URL, BRANCH).returncode != 0:
        print(f"FETCH FAILED {URL} {BRANCH}: continuing on the current version")
        sys.exit(1)
    behind = int(git("rev-list", "--count", "HEAD..FETCH_HEAD").stdout.strip() or 0)
    if behind == 0:
        print("kit up to date")
        return
    busy = [b for b in BUSY if os.path.exists(os.path.join(SP, b))]
    if busy:
        print(f"UPDATE WAITING: {behind} commit(s); {', '.join(busy)} present. A running bash script "
              "must never be rewritten under it: stop the fleet (touch STOP_PULL FLEET_STOPPED, "
              "bash fullstop.sh), wait for the integration to finish, then rerun")
        sys.exit(2)
    dirty = [l[3:] for l in git("status", "--porcelain", "--untracked-files=no").stdout.splitlines() if l.strip()]
    if dirty:
        print(f"UPDATE BLOCKED: {behind} commit(s) waiting; local changes to tracked files: {', '.join(dirty)}. "
              "Commit them, or ask the user what to do with them")
        sys.exit(3)
    old = git("rev-parse", "HEAD").stdout.strip()
    if git("merge", "--ff-only", "-q", "FETCH_HEAD").returncode != 0:
        print(f"UPDATE BLOCKED: local commits diverge from the published kit; rebase them onto it "
              f"(git pull --rebase {URL} {BRANCH})")
        sys.exit(3)
    new = git("rev-parse", "HEAD").stdout.strip()
    changed = git("diff", "--name-only", old, new).stdout.split()
    print(f"updated {old[:8]}..{new[:8]}: {behind} commit(s), {len(changed)} file(s)", flush=True)
    for f in changed:
        if f.startswith(INSTRUCTIONS):
            print(f"RE-READ {f}", flush=True)
    argv = [sys.executable, "kit_init.py"]
    if any(f.startswith(SLOW_TRIGGERS) for f in changed):
        argv.append("--slow")
    sys.exit(0 if subprocess.run(argv, cwd=SP).returncode == 0 else 1)


main()
