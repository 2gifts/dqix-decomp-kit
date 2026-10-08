import os as _kpos, sys as _kpsys
_kpsys.path.insert(0, _kpos.path.dirname(_kpos.path.dirname(_kpos.path.abspath(__file__))))
import kitpaths as _kp
import argparse
import glob
import os
import re
import subprocess
import sys
from collections import Counter

REPO = _kp.REPO
SP = _kp.SP
KIT = _kp.KIT
POOLS = ["attempts", "clsbest", "quarantine", "wip"] + sorted(
    os.path.basename(p) for p in glob.glob(SP + "/hold*"))
SYM = re.compile(r"(\S+) kind:(\w+)\S* addr:0x([0-9a-fA-F]{8})")
TAG = re.compile(r"\s*//\s*(?:SCRATCH-)?USA:")
KEYWORD = {"return", "if", "while", "for", "switch", "else", "do", "case", "sizeof", "goto"}
BUILTIN = {"unsigned", "signed", "char", "short", "int", "long", "void", "float", "double", "bool"}
DECL = re.compile(r"^\s*(?:extern\s+(?:\"C\"\s+)?)?(?:static\s+|inline\s+)*"
                  r"([A-Za-z_][\w\s\*&:<>]*?[\s\*&])([A-Za-z_]\w*)\s*\(([^;{}]*)\)\s*([;{])")


def git(*args):
    return subprocess.run(["git", "-C", REPO] + list(args), capture_output=True, text=True,
                          encoding="utf-8", errors="ignore").stdout


def functions(rev):
    out = {}
    for path in git("ls-tree", "-r", "--name-only", rev, "config/usa/arm9").split():
        if path.endswith("symbols.txt"):
            m = re.search(r"/ov(\d+)/", path)
            for line in git("show", "%s:%s" % (rev, path)).splitlines():
                s = SYM.match(line)
                if s and s.group(2) == "function":
                    out[(m.group(1) if m else "main", int(s.group(3), 16))] = s.group(1)
    return out


def plain(sym):
    m = re.match(r"_Z(\d+)", sym)
    if m:
        return sym[m.end():m.end() + int(m.group(1))]
    return None if sym.startswith("_Z") else sym


def headers(rev):
    return {p: git("show", "%s:%s" % (rev, p))
            for p in git("ls-tree", "-r", "--name-only", rev, "include").split()
            if p.endswith((".h", ".hpp"))}


def type_names(texts):
    names = set()
    for t in texts:
        names.update(re.findall(r"\b(?:struct|class|union|enum)\s+([A-Za-z_]\w*)\s*(?::[^{;]*)?\{", t))
        names.update(re.findall(r"\}\s*([A-Za-z_]\w*)\s*;", t))
        names.update(re.findall(r"\btypedef\b[^;{]*?\b([A-Za-z_]\w*)\s*;", t))
    return names


def signatures(text, top_level_only=False):
    """Use the shared lexical filter for declarations and ordinary flat definitions."""
    from scaffold import _code_without_comments
    in_block = False
    depth = 0
    for raw in text.splitlines():
        code, in_block = _code_without_comments(raw, in_block)
        match = DECL.match(code)
        if match and (not top_level_only or depth == 0) and match.group(1).strip().split()[-1].strip("*&") not in KEYWORD:
            yield (match.group(1).strip(), match.group(2), match.group(3).strip(),
                   match.group(4), not re.search(r'\bextern\s+"C"', raw))
        depth += code.count("{") - code.count("}")


def matches_cpp_parameters(parameters, symbol):
    """Fail closed on unsupported encodings/types; use the shared mangled decoder."""
    from symfix import demangle_params, split_args
    encoded = re.fullmatch(r"_Z(\d+)(.+)", symbol)
    if not encoded:
        return False
    length = int(encoded.group(1))
    try:
        expected = demangle_params(encoded.group(2)[length:])
    except (ValueError, IndexError):
        return False
    supplied = split_args(parameters)
    if supplied == ["void"]:
        supplied = []
    if len(supplied) != len(expected):
        return False
    for actual, wanted in zip(supplied, expected):
        if re.search(r"[=()<>\[\].]", actual):
            return False
        tokens = lambda text: re.findall(r"[A-Za-z_]\w*|[^\s]", re.sub(r"\b(?:struct|class)\s+", "", text))
        a, w = tokens(actual), tokens(wanted)
        if a != w and not (a[:-1] == w and re.fullmatch(r"[A-Za-z_]\w*", a[-1])):
            return False
    return True


def canonical_declaration(text, name, symbol):
    declarations = [row for row in signatures(text, top_level_only=True) if row[1] == name and row[3] == ";"]
    return bool(declarations) and all(row[4] and matches_cpp_parameters(row[2], symbol) for row in declarations)


def source_cpp_bindings(rev, current, candidates):
    """Prove only configured flat signatures in complete current source owners, in two batches."""
    import io
    import tarfile
    from union_merge import parse_delinks, block_range

    def archive(paths):
        result = subprocess.run(["git", "-C", REPO, "archive", rev, "--", *sorted(paths)], capture_output=True)
        if result.returncode:
            raise RuntimeError(result.stderr.decode("utf-8", errors="replace"))
        with tarfile.open(fileobj=io.BytesIO(result.stdout)) as packed:
            return {member.name: packed.extractfile(member).read().decode("utf-8", errors="replace")
                    for member in packed if member.isfile()}

    if not candidates:
        return {}
    needed = [(module, address, symbol) for (module, address), symbol in current.items()
              if candidates.get(plain(symbol)) == symbol]
    owned = {}
    for path, text in archive(["config/usa/arm9"]).items():
        if not path.endswith("delinks.txt"):
            continue
        overlay = re.search(r"/ov(\d+)/", path)
        module = overlay.group(1) if overlay else "main"
        for owner, lines in parse_delinks(text)[1]:
            bounds = block_range(lines)
            if not owner.endswith(".cpp") or "complete" not in [line.strip() for line in lines] or not bounds:
                continue
            for slot, address, symbol in needed:
                if slot == module and bounds[0] <= address < bounds[1]:
                    owned.setdefault(owner, set()).add(symbol)
    if not owned:
        return {}
    # Fail closed before an unusually large Windows argv; this is a narrow source probe.
    if sum(len(path) + 1 for path in owned) > 16000:
        return {}
    proven = {}
    for path, text in archive(owned).items():
        # The shared line lexer does not evaluate preprocessors or multiline quoted tokens.
        # Such files cannot certify a definition for this conservative spelling guard.
        if re.search(r'\bR"|\\\r?\n|^\s*#\s*(?:if|ifdef|ifndef|elif|else|endif)\b', text, re.M):
            continue
        for return_type, name, parameters, ending, cpp in signatures(text, top_level_only=True):
            if ending != "{" or not cpp or "static" in return_type.split():
                continue
            symbol = candidates.get(name)
            if symbol in owned[path] and matches_cpp_parameters(parameters, symbol):
                proven[name] = symbol
    return proven
def prototypes(texts):
    out = {}
    for text in texts:
        for return_type, name, parameters, ending, cpp in signatures(text):
            if ending == ";":
                out.setdefault(name, (return_type, parameters))
    return out


def definition(text, t):
    m = re.search(r"^[ \t]*struct\s+%s\s*\{" % re.escape(t), text, re.M)
    if not m:
        return None
    depth, k = 0, m.end() - 1
    while k < len(text):
        depth += (text[k] == "{") - (text[k] == "}")
        k += 1
        if depth == 0:
            break
    body = re.sub(r"/\*.*?\*/|//[^\n]*", "", text[m.start():text.find(";", k) + 1], flags=re.S)
    for line in body.split("\n")[1:-1]:
        f = re.match(r"\s*(?:struct\s+|const\s+|volatile\s+)*([A-Za-z_]\w*)\s*(\*?)", line)
        if f and f.group(1) not in BUILTIN and not f.group(2):
            return None
    return "\n".join(ln.rstrip() for ln in body.strip().split("\n"))


def build(rev):
    old, new = functions(rev), functions("HEAD")
    oh, nh = headers(rev), headers("HEAD")
    current_cpp = prototypes(nh.values())
    ren = {old[k]: new[k] for k in old if k in new and old[k] != new[k]}
    spell = dict(ren)
    byplain = {}
    for o, n in ren.items():
        p = plain(o)
        if p and p != o:
            byplain.setdefault(p, set()).add(n)
    source_candidates = {p: next(iter(ns)) for p, ns in byplain.items()
                         if len(ns) == 1 and plain(next(iter(ns))) == p}
    source_cpp = source_cpp_bindings("HEAD", new, source_candidates)
    current = set(new.values())
    for p, ns in byplain.items():
        if len(ns) == 1 and p not in current:
            n = next(iter(ns))
            if p not in current_cpp or plain(n) != p:
                spell.setdefault(p, n)
    changed = [p for p in oh if oh[p] != nh.get(p)]
    protos = prototypes(oh[p] for p in changed)
    gone = type_names(oh[p] for p in changed) - type_names(nh.values())
    defs = {}
    for p in changed:
        for t in gone:
            d = definition(oh[p], t)
            if d:
                defs.setdefault(t, d)
    moved = {}
    for line in git("diff", "-M", "--name-status", rev, "HEAD", "--", "include").splitlines():
        f = line.split("\t")
        if f[0].startswith("R") and len(f) == 3:
            moved[f[1][len("include/"):]] = f[2][len("include/"):]
    return spell, protos, gone, moved, defs, source_cpp


def _declares(line, sym):
    m = re.match(r"^(.*?)\b%s\s*\(" % re.escape(sym), line)
    if not m or not line.strip().endswith(";"):
        return False
    lead = m.group(1)
    return (bool(lead.strip()) and not (set(lead) & set("=(:;,{}?!<>+-/%|^~"))
            and not any(tok.strip("*&") in KEYWORD for tok in lead.split()))


def force_extern_c(text, sym, cxx):
    if not cxx:
        return text
    out = []
    for line in text.split("\n"):
        if _declares(line, sym) and 'extern "C"' not in line:
            line = re.sub(r"^(\s*)", r'\1extern "C" ', line)
        out.append(line)
    return "\n".join(out)


def insert_after_includes(text, block):
    lines = text.split("\n")
    last = max((i for i, ln in enumerate(lines) if re.match(r"\s*#\s*include\b", ln)), default=-1)
    lines[last + 1:last + 1] = block
    return "\n".join(lines)


def rewrite(text, cxx, spell, protos, gone, moved, defs, source_cpp=None):
    orig = text
    for o, n in moved.items():
        text = re.sub(r'(#\s*include\s*[<"])%s([>"])' % re.escape(o), r"\g<1>%s\2" % n, text)
    protected = {name for name, symbol in (source_cpp or {}).items()
                 if cxx and canonical_declaration(text, name, symbol)}
    spell = {old: new for old, new in spell.items() if old not in protected}
    if not spell:
        return text, ("includes" if text != orig else None)
    words = re.compile(r"\b(%s)\b" % "|".join(sorted(map(re.escape, spell), key=len, reverse=True)))
    defined = {w for w in set(words.findall(text))
               if re.search(r"^[ \t]*[A-Za-z_][\w \t\*&:<>]*?[ \t\*&]%s\s*\([^;{}()]*\)\s*(?:const\s*)?\{"
                            % re.escape(w), text, re.M)}
    used = {}
    lines = text.split("\n")
    for k, ln in enumerate(lines):
        if TAG.match(ln):
            continue

        def sub(m):
            w = m.group(1)
            if w in defined:
                return w
            used.setdefault(spell[w], w)
            return spell[w]
        lines[k] = words.sub(sub, ln)
    text = "\n".join(lines)
    add = []
    for newsym, oldw in sorted(used.items()):
        if any(_declares(ln, newsym) for ln in text.split("\n")):
            text = force_extern_c(text, newsym, cxx)
            continue
        proto = protos.get(plain(oldw) or oldw)
        if proto is None:
            return None, "no prototype for %s" % oldw
        add.append('%s%s %s(%s);' % ('extern "C" ' if cxx else "", proto[0], newsym, proto[1]))
    probe = text + "\n" + "\n".join(add)
    used_t = [t for t in sorted(gone) if re.search(r"\b%s\b" % re.escape(t), probe)]
    bare = [t for t in used_t if not re.search(r"\b(?:struct|class|union)\s+%s\s*[;{:]" % re.escape(t), text)]
    full = [defs[t] for t in bare if t in defs]
    block = ["struct %s;" % t for t in bare if t not in defs] + full + add
    if block:
        text = insert_after_includes(text, block)
    what = ", ".join("%s->%s" % (o, n) for n, o in sorted(used.items()))
    what += "".join(" +def %s" % t for t in used_t if t in defs and defs[t] in full)
    return text, (what.strip() or ("includes" if text != orig else None))


def pool_files(exclude):
    for pool in POOLS:
        for p in sorted(glob.glob("%s/%s/**/*.c*" % (SP, pool), recursive=True)):
            if not p.endswith((".c", ".cpp")) or any(a in p for a in exclude):
                continue
            text = open(p, encoding="utf-8", errors="ignore").read()
            if any(TAG.match(ln) for ln in text.split("\n")) and not any(a in text for a in exclude):
                yield p, text


def locate(path, text):
    tag = next((ln for ln in text.split("\n") if TAG.match(ln)), "") + " " + os.path.basename(path)
    a = re.search(r"(02[0-9a-fA-F]{6})", tag)
    m = re.search(r"ov(\d{3})", tag) or re.search(r"hold_(?:ov)?(\d{3})", path)
    return (m.group(1) if m else "main"), (a.group(1).lower() if a else None)


def gate(mod, addr, path):
    r = subprocess.run([sys.executable, KIT + "/wgate.py", mod, addr, path], capture_output=True, text=True,
                       cwd=REPO, env={**os.environ, "WGATE_ALLOW_COMMITTED": "1"})
    out = (r.stdout + r.stderr).strip().splitlines()
    return next((ln for ln in out if "COMPILE-FAIL" in ln or "NO-COMPILE" in ln), out[-1] if out else "no output")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("paths", nargs="*")
    ap.add_argument("--rev", default="a70058a0")
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--check", action="store_true")
    ap.add_argument("--exclude", default="")
    a = ap.parse_args()
    spell, protos, gone, moved, defs, source_cpp = build(a.rev)
    exclude = [x for x in a.exclude.split(",") if x]
    files = ([(p, open(p, encoding="utf-8", errors="ignore").read()) for p in a.paths]
             if a.paths else list(pool_files(exclude)))
    stats = Counter()
    for path, text in files:
        new, what = rewrite(text, path.endswith(".cpp"), spell, protos, gone, moved, defs, source_cpp)
        if new is None:
            stats["unfixable"] += 1
            print("SKIP  %s  %s" % (os.path.relpath(path, SP), what))
            continue
        if new == text:
            continue
        target = path if a.apply else path + ".repool"
        if a.apply:
            mod, addr = locate(path, text)
            if addr:
                before = gate(mod, addr, path)
                open(target, "w", encoding="utf-8", newline="\n").write(new)
                after = gate(mod, addr, target)
                if "COMPILE" in after and "COMPILE" not in before:
                    open(target, "w", encoding="utf-8", newline="\n").write(text)
                    stats["reverted"] += 1
                    print("REVERT %s  rewrite broke the compile: %s" % (os.path.relpath(path, SP), after[:120]))
                    continue
        stats["rewritten"] += 1
        open(target, "w", encoding="utf-8", newline="\n").write(new)
        verdict = ""
        if a.check:
            mod, addr = locate(path, new)
            verdict = gate(mod, addr, target) if addr else "no address"
            stats["NO-COMPILE" if "COMPILE" in verdict else "compiles"] += 1
        if not a.apply:
            os.remove(target)
        print("%s  %s  %s%s" % ("FIX " if a.apply else "WOULD", os.path.relpath(path, SP), what,
                                ("  | " + verdict[:160]) if verdict else ""))
    print("repool: %s" % dict(stats))


if __name__ == "__main__":
    main()
