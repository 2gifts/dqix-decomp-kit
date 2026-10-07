"""Where the kit's code, its state and the decomp checkout live.

    KIT    this checkout: scripts, docs, skills. Nothing the pipeline produces is written here.
    SP     the state directory: attempts, logs, claims, staging, worker docs, knobs. $DQIX_STATE, else
           the path in KIT/state.path, else ../dqix-kit-state beside the checkout. Kept outside the
           checkout so no git command run in it can touch a single attempt.
    REPO   the decomp checkout: $DQIX_REPO, else ../dqix-decomp beside the checkout.

    python kitpaths.py kit|state|repo     print one of them, for shell scripts
"""
import os
import sys


def _norm(p):
    return os.path.abspath(p).replace("\\", "/")


KIT = _norm(os.path.dirname(os.path.abspath(__file__)))


def _state():
    env = os.environ.get("DQIX_STATE")
    if env:
        return _norm(env)
    try:
        with open(os.path.join(KIT, "state.path"), encoding="utf-8") as fh:
            line = fh.readline().strip()
        if line:
            return _norm(line if os.path.isabs(line) else os.path.join(KIT, line))
    except OSError:
        pass
    return _norm(os.path.join(os.path.dirname(KIT), "dqix-kit-state"))


SP = _state()
REPO = _norm(os.environ.get("DQIX_REPO", os.path.join(os.path.dirname(KIT), "dqix-decomp")))
CLAUDE_PROJECTS = _norm(os.environ.get("CLAUDE_PROJECTS", os.path.expanduser("~/.claude/projects")))

if __name__ == "__main__":
    which = sys.argv[1] if len(sys.argv) > 1 else ""
    if which not in ("kit", "state", "repo"):
        sys.exit(__doc__)
    print({"kit": KIT, "state": SP, "repo": REPO}[which])
