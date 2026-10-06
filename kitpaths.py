import os

SP = os.path.dirname(os.path.abspath(__file__)).replace("\\", "/")
REPO = os.environ.get("DQIX_REPO", os.path.join(os.path.dirname(SP), "dqix-decomp")).replace("\\", "/")
CLAUDE_PROJECTS = os.environ.get("CLAUDE_PROJECTS", os.path.expanduser("~/.claude/projects")).replace("\\", "/")
