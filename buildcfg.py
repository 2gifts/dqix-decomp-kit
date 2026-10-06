#!/usr/bin/env python3
import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.abspath(__file__)))
import kitpaths as _kp
# The compiler, flags and include paths the BUILD uses, read from tools/configure.py itself.
# wgate, classify and the repair sweeps each carried their own copy of the flag list, so the
# rebase onto sp2p2 left every one of them measuring on a compiler the ROM is no longer built
# with. Importing the build's own definitions is the only arrangement they cannot drift from.
import os
import re
import sys

REPO = _kp.REPO
_TOOLS = os.path.join(REPO, "tools")


def _load():
    cwd, argv = os.getcwd(), sys.argv[:]
    os.chdir(REPO)
    sys.argv = ["configure.py", "usa"]
    sys.path.insert(0, _TOOLS)
    try:
        import configure
        return configure
    finally:
        os.chdir(cwd)
        sys.argv = argv
        if _TOOLS in sys.path:
            sys.path.remove(_TOOLS)


_cfg = _load()

MWCC_VERSION = _cfg.MWCC_VERSION
DECOMP_ME_COMPILER = _cfg.DECOMP_ME_COMPILER
CC = f"{REPO}/tools/mwccarm/{MWCC_VERSION}/mwccarm.exe"
AS = f"{REPO}/tools/mwccarm/{MWCC_VERSION}/mwasmarm.exe"
AS_FLAGS = _cfg.AS_FLAGS.split()
FLAGS = (_cfg.CC_FLAGS + " " + _cfg.CC_INCLUDES + " -d usa").split()
CODEGEN_PRAGMA = re.compile(r"(?m)^[ \t]*#[ \t]*pragma[ \t]+(?!(?:define_section|section|once)\b)(\w+)")


def lcf_symbols():
    overlays = [d for d in os.listdir(f"{REPO}/config/usa/arm9/overlays") if re.fullmatch(r"ov\d+", d)]
    return {f"OVERLAY_{int(d[2:])}_ID": int(d[2:]) for d in overlays}


def cc_path(version):
    return f"{REPO}/tools/mwccarm/{version}/mwccarm.exe" if version else CC


if __name__ == "__main__":
    if "--cc" in sys.argv:
        print(CC)
    elif "--flags" in sys.argv:
        print(" ".join(FLAGS))
    else:
        print("MWCC", MWCC_VERSION)
        print("CC  ", CC)
        print("FLAGS", " ".join(FLAGS))
