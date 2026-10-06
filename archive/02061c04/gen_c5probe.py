"""Emit the case-0xc5 ladder probe: four candidate C shapes for one dead compare chain.

The ROM tests 20 ranges in order and every branch lands on the same continuation, so whatever
the original wrote, the compiler kept the control flow and dropped the values.
"""
import sys

GROUPS = [(0x0d, 0x0d), (0x0e, 0x0f), (0x10, 0x11), (0x12, 0x18), (0x19, 0x1b), (0x1c, 0x21),
          (0x22, 0x26), (0x28, 0x5c), (0x5d, 0x5d), (0x5e, 0x64), (0x65, 0x68), (0x69, 0x6b),
          (0x6c, 0x71), (0x7b, 0x7c), (0x7d, 0x80), (0x81, 0x85), (0x86, 0x89), (0x8a, 0x8c),
          (0x8d, 0x92), (0x94, 0x94)]


def labels(lo, hi, indent):
    out, line = [], indent
    for v in range(lo, hi + 1):
        if len(line) > 92:
            out.append(line)
            line = indent
        line += "case 0x%x: " % v
    out.append(line)
    return "\n".join(out)


def switch_body(var, body_of, indent="        "):
    parts = []
    for i, (lo, hi) in enumerate(GROUPS):
        parts.append(labels(lo, hi, indent))
        parts.append(indent + "    " + body_of(i))
    parts.append(indent + "default:")
    parts.append(indent + "    " + body_of(-1))
    return "    switch (%s) {\n%s\n    }" % (var, "\n".join(parts))


def main():
    out = ["// probe: which shape emits the ROM's dead range ladder for case 0xc5",
           "extern \"C\" void sink(int);", ""]

    # A: an inline classifier whose result is discarded.
    out.append("static inline int ClassifyA(int v) {")
    out.append(switch_body("v", lambda i: "return %d;" % i, "    "))
    out.append("    return -1;")
    out.append("}")
    out.append("extern \"C\" int probeA(int cur, int nv) {")
    out.append("    ClassifyA(cur);")
    out.append("    ClassifyA(nv);")
    out.append("    return cur != nv;")
    out.append("}")
    out.append("")

    # B: the same classification written in place, into a local nothing reads.
    out.append("extern \"C\" int probeB(int cur, int nv) {")
    out.append("    int k;")
    out.append(switch_body("cur", lambda i: "k = %d; break;" % i, "    "))
    out.append(switch_body("nv", lambda i: "k = %d; break;" % i, "    "))
    out.append("    return cur != nv;")
    out.append("}")
    out.append("")

    # C: empty bodies -- what the file has now, expected to fold away.
    out.append("extern \"C\" int probeC(int cur, int nv) {")
    out.append(switch_body("cur", lambda i: "break;", "    "))
    out.append(switch_body("nv", lambda i: "break;", "    "))
    out.append("    return cur != nv;")
    out.append("}")
    out.append("")

    # D: distinct empty labelled bodies, each falling out of its own case.
    out.append("extern \"C\" int probeD(int cur, int nv) {")
    out.append("    int k = 0;")
    out.append(switch_body("cur", lambda i: "k += %d; break;" % i, "    "))
    out.append("    sink(k);")
    out.append("    return cur != nv;")
    out.append("}")

    # E: the classifier forced inline, its result discarded -- values die, branches may not.
    out.append("#pragma inline_max_size(20000)")
    out.append("#pragma inline_max_auto_size(20000)")
    out.append("static inline int ClassifyE(int v) {")
    out.append(switch_body("v", lambda i: "return %d;" % i, "    "))
    out.append("    return -1;")
    out.append("}")
    out.append("extern \"C\" int probeE(int cur, int nv) {")
    out.append("    ClassifyE(cur);")
    out.append("    ClassifyE(nv);")
    out.append("    return cur != nv;")
    out.append("}")
    out.append("")

    # F: an inline predicate that only ever returns the same value.
    out.append("static inline int ClassifyF(int v) {")
    out.append(switch_body("v", lambda i: "return 0;" if i >= 0 else "return 0;", "    "))
    out.append("}")
    out.append("extern \"C\" int probeF(int cur, int nv) {")
    out.append("    ClassifyF(cur);")
    out.append("    ClassifyF(nv);")
    out.append("    return cur != nv;")
    out.append("}")

    # G/H: does a dead-code pragma keep the empty chain alive?
    for tag, prag in (("G", "opt_dead_code off"), ("H", "opt_dead_assignments off")):
        out.append("#pragma %s" % prag)
        out.append("extern \"C\" int probe%s(int cur, int nv) {" % tag)
        out.append("    int k;")
        out.append(switch_body("cur", lambda i: "k = %d; break;" % i, "    "))
        out.append(switch_body("nv", lambda i: "k = %d; break;" % i, "    "))
        out.append("    return cur != nv;")
        out.append("}")
        out.append("#pragma %s" % prag.replace(" off", " on"))
        out.append("")

    # I: every body jumps to the statement after the switch.
    out.append("extern \"C\" int probeI(int cur, int nv) {")
    out.append(switch_body("cur", lambda i: "goto after1;", "    "))
    out.append("after1:")
    out.append(switch_body("nv", lambda i: "goto after2;", "    "))
    out.append("after2:")
    out.append("    return cur != nv;")
    out.append("}")
    out.append("")

    # J: null-statement bodies -- non-empty in the AST, no code of their own.
    out.append("extern \"C\" int probeJ(int cur, int nv) {")
    out.append(switch_body("cur", lambda i: ";", "    "))
    out.append(switch_body("nv", lambda i: ";", "    "))
    out.append("    return cur != nv;")
    out.append("}")
    out.append("")

    # K: each body is its own labelled fallthrough to a shared tail.
    out.append("extern \"C\" int probeK(int cur, int nv) {")
    out.append("    int k = cur;")
    out.append(switch_body("cur", lambda i: "k = k; break;", "    "))
    out.append(switch_body("nv", lambda i: "k = k; break;", "    "))
    out.append("    return cur != nv;")
    out.append("}")

    # L/M: an || chain of range tests whose value nothing reads. Short-circuit branches are built
    # during expression codegen, so they may outlive the value that dies in DCE.
    chain = " ||\n        ".join("(%s >= 0x%x && %s <= 0x%x)" % ("%s", lo, "%s", hi)
                                 for lo, hi in GROUPS)
    out.append("extern \"C\" int probeL(int cur, int nv) {")
    out.append("    (void)(" + chain.replace("%s", "cur") + ");")
    out.append("    (void)(" + chain.replace("%s", "nv") + ");")
    out.append("    return cur != nv;")
    out.append("}")
    out.append("")
    out.append("extern \"C\" int probeM(int cur, int nv) {")
    out.append("    int a = " + chain.replace("%s", "cur") + ";")
    out.append("    int b = " + chain.replace("%s", "nv") + ";")
    out.append("    return cur != nv;")
    out.append("}")

    # N: nested switch, every body identical. Lowering emits both chains; the identical bodies are
    # tail-merged afterwards, which is the only way an in-set target can equal the fall-through.
    inner = switch_body("nv", lambda i: "return cur != nv;", "            ")
    out.append("extern \"C\" int probeN(int cur, int nv) {")
    out.append(switch_body("cur", lambda i: "\n" + inner + "\n            break;", "    "))
    out.append("    return 0;")
    out.append("}")

    # P/Q: the gcc case-range extension. One source range per emitted cmp/blt/cmp/ble pair would
    # explain the ROM's grouping exactly ([0xd], [0xe,0xf], [0x10,0x11], [0x12,0x18], ...).
    def ranges(var, body, indent="    "):
        parts = []
        for lo, hi in GROUPS:
            parts.append(indent + ("case 0x%x:" % lo if lo == hi
                                   else "case 0x%x ... 0x%x:" % (lo, hi)))
            parts.append(indent + "    " + body)
        parts.append(indent + "default:")
        parts.append(indent + "    " + body)
        return "    switch (%s) {\n%s\n    }" % (var, "\n".join(parts))

    # R/S: an if / else-if chain of range tests. Branch senses match the ROM exactly -- below the
    # low bound falls to the next test, inside the range jumps forward to a common block.
    def ifchain(var, body, tail=""):
        parts = []
        for n, (lo, hi) in enumerate(GROUPS):
            kw = "    if" if n == 0 else "    else if"
            parts.append("%s (%s >= 0x%x && %s <= 0x%x) { %s }" % (kw, var, lo, var, hi, body))
        if tail:
            parts.append("    else { %s }" % tail)
        return "\n".join(parts)

    out.append("extern \"C\" int probeR(int cur, int nv) {")
    out.append(ifchain("cur", ""))
    out.append(ifchain("nv", ""))
    out.append("    return cur != nv;")
    out.append("}")
    out.append("")
    # T: each body calls an empty inline function -- non-empty in the AST, no code after inlining.
    # This is what a retail build does to a disabled assert/log macro.
    out.append("static inline void Nop(int) {}")
    # W: if-chain whose bodies jump to the label right after the chain.
    out.append("extern \"C\" int probeW(int cur, int nv) {")
    out.append(ifchain("cur", "goto w1;"))
    out.append("w1:")
    out.append(ifchain("nv", "goto w2;"))
    out.append("w2:")
    out.append("    return cur != nv;")
    out.append("}")
    out.append("")
    # X: if-chain guarding a shared call -- bodies identical and non-trivial.
    out.append("extern \"C\" int probeX(int cur, int nv) {")
    out.append("    int k = 0;")
    out.append(ifchain("cur", "k = 1;"))
    out.append(ifchain("nv", "k = 1;"))
    out.append("    return k + (cur != nv);")
    out.append("}")
    out.append("")
    # Y: the same chain, but every body assigns the value the variable already holds. A redundant
    # store can be dropped without the branch that guards it -- which is exactly the ROM's shape.
    out.append("extern \"C\" int probeY(int cur, int nv) {")
    out.append("    int k = 1;")
    out.append(ifchain("cur", "k = 1;"))
    out.append(ifchain("nv", "k = 1;"))
    out.append("    return k + (cur != nv);")
    out.append("}")
    out.append("")
    # AA: guarded stores that a later unconditional store kills. Dead-store elimination runs after
    # lowering, so the branches survive while every assignment disappears.
    out.append("extern \"C\" int probeAA(int cur, int nv) {")
    out.append("    int k;")
    out.append(ifchain("cur", "k = 1;"))
    out.append(ifchain("nv", "k = 2;"))
    out.append("    k = cur != nv;")
    out.append("    return k;")
    out.append("}")
    out.append("")
    # AB: each body re-assigns the value the variable already holds, and the variable IS read after.
    # The store is redundant rather than dead, so the branch survives and no mov is emitted.
    out.append("extern \"C\" int probeAB(int cur, int nv) {")
    out.append("    int a = cur;")
    out.append(ifchain("cur", "a = cur;"))
    out.append("    int b = nv;")
    out.append(ifchain("nv", "b = nv;"))
    out.append("    return a != b;")
    out.append("}")
    out.append("")
    out.append("extern \"C\" int probeT(int cur, int nv) {")
    out.append(switch_body("cur", lambda i: "Nop(%d); break;" % i, "    "))
    out.append(switch_body("nv", lambda i: "Nop(%d); break;" % i, "    "))
    out.append("    return cur != nv;")
    out.append("}")
    out.append("")
    out.append("extern \"C\" int probeS(int cur, int nv) {")
    out.append("    int k;")
    out.append(ifchain("cur", "k = 1;"))
    out.append(ifchain("nv", "k = 1;"))
    out.append("    return cur != nv;")
    out.append("}")

    open(sys.argv[1], "w", newline="\n").write("\n".join(out) + "\n")


if __name__ == "__main__":
    main()
