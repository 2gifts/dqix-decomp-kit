// BEST: 144 bytes in 3 runs, .text 0x120 vs slot 0x124 (1 instruction short).
// Two independent residues:
//  (1) if-conversion: target branches the `else if (flags & 0x20)` arm
//      (tst; beq; ldrsb; ldrsb; rsb; mla; mla) — mwcc predicates the same 5 instructions
//      here (ldrsbne/rsbne/ldrsbne/mlane/mlane), so the beq is the missing instruction and
//      every offset from +0x74 on is shifted by 4. RULED OUT: 5-statement split of the arm
//      (explicit stepX/stepY locals), shared `int shift` declared before both ifs,
//      pragmas optimize_for_size on|off, peephole off, scheduling off, opt_conditional_move off,
//      opt_dead_code off, opt_lifetimes off, opt_common_subs off, opt_propagation off,
//      optimization_level 3. All byte-identical at 144.
//  (2) stack slot order: target sret buffer @sp+0x18, struct copies stored to 0x10 then 0x20,
//      later reads from 0x20/0x24. This build puts the sret temp @0x10, copies to 0x20 then
//      0x18. RULED OUT: declaring the live local first (mwcc coalesces, frame 0x20),
//      3-local chain with plain assignments (coalesces to one slot, frame 0x18),
//      3-local chain with an init-form middle (166 bytes), passing Result020b1020 by value as
//      a 9th arg to func_020b245c (copy emitted at the call site, 203 bytes / 0x128).
//  colorsweep --apply found nothing (its one "improvement" was a semantics-breaking operand
//  swap scored at 1142 bytes); reverted.

#include <globaldefs.h>

struct Result020b1020 { int a; int b; };

struct BoxStep020b25c4 { signed char x; signed char y; };

struct BoxContext020b25c4 {
    int unused0;
    void* iface;
    int param2;
    int adjustment;
};

Result020b1020 Compute020b1020(void* iface, int param2, int adjustment, int count);

extern "C" void func_020b245c(BoxContext020b25c4* ctx, int x, int y, int width, int param5,
                              unsigned int flags, int count, BoxStep020b25c4 step);

// USA: func_020b25c4
extern "C" ARM void AlignBoxAndDraw_020b25c4(BoxContext020b25c4* ctx, int x, int y, int param5,
                                             unsigned int flags, int count,
                                             BoxStep020b25c4 step) {
    Result020b1020 size = Compute020b1020(ctx->iface, ctx->param2, ctx->adjustment, count);
    Result020b1020 box = size;
    int shift;

    if (flags & 0x10) {
        shift = 0;
        shift = shift + -(box.a + 1) / 2;
        y = step.y * shift + y;
        x = shift * step.x + x;
    } else if (flags & 0x20) {
        shift = -box.a;
        x = shift * step.x + x;
        y = shift * step.y + y;
    }

    if (flags & 2) {
        shift = -(box.b + 1) / 2;
        x = shift * -step.y + x;
        y = shift * step.x + y;
    } else if (flags & 4) {
        shift = -box.b;
        x = shift * -step.y + x;
        y = shift * step.x + y;
    }

    func_020b245c(ctx, x, y, box.a, param5, flags, count, step);
}
