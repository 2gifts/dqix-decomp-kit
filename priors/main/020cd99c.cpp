// BEST: BYTEDIFF 40 bytes in 6 runs, size exact (0x128). Whole diff is confined to the two
// div-setup blocks and is one root cause: the target keeps the compare's `ldrsh rect->width`
// alive in r2 and rematerialises `mov r1,#0` twice, while mwcc here puts the compare value in
// r1, reloads rect->width for denomLo, and CSEs the zero into ip.
// The `switch ((int)rect)` on the null guard is load-bearing: it is what forces the target's
// `bne` + non-predicated null block (plain `if (...) return;`, `if/else`, and `goto` all
// predicate to ldreq/moveq/strheq/popeq and come out 4 bytes short).
// Ruled out (all measured, do not retry): colorsweep (20 compiles, no change) - pragmas
// opt_common_subs/opt_propagation/scheduling/opt_lifetimes/peephole/opt_dead_assignments/
// register_coloring off - local `int width` before the if (hoists `mov #0` to the dominator,
// 196b) - local inside the if-block (0x118) - `if (int width = ...)` scoped decl (0x118/0x110)
// - 64-bit `denom` field (170b) - long long / struct return from DisableIRQInterrupts -
// two-arg SetIRQInterruptState - single div pointer at function top - inline
// StartDivide/DivideResult helpers (0x110) - else-arm zeros chained off data.x.
#include <globaldefs.h>

extern "C" int _Z20DisableIRQInterruptsv(void);
extern "C" void _Z20SetIRQInterruptStatei(int state);

struct DivRegs020cd99c {
    volatile unsigned short cnt;
    unsigned short pad02;
    unsigned int pad04[3];
    volatile int numerLo;
    volatile int numerHi;
    volatile int denomLo;
    volatile int denomHi;
};

struct ViewRect020cd99c {
    short x;
    short y;
    short width;
    short height;
};

struct ViewScales020cd99c {
    char pad0[0x1c];
    int x;
    int width;
    int invWidth;
    int y;
    int height;
    int invHeight;
    short valid;
};

extern ViewScales020cd99c data_021117b0;

// USA: func_020cd99c  (semantic: StoreViewScales020cd99c)
extern "C" ARM void func_020cd99c(ViewRect020cd99c* rect) {
    if (rect == NULL) {
        data_021117b0.valid = 0;
    } else {
        int irq = _Z20DisableIRQInterruptsv();
        int width = rect->width;

        if (width != 0) {
            DivRegs020cd99c* div = (DivRegs020cd99c*)0x04000280;
            div->cnt = 0;
            div->numerLo = 0x10000000;
            div->denomLo = width;
            div->denomHi = 0;
            data_021117b0.x = rect->x;
            data_021117b0.width = rect->width;
            while (div->cnt & 0x8000) {
            }
            data_021117b0.invWidth = *(volatile int*)0x040002a0;
        } else {
            data_021117b0.x = 0;
            data_021117b0.width = 0;
            data_021117b0.invWidth = 0;
        }

        int height = rect->height;

        if (height != 0) {
            DivRegs020cd99c* div = (DivRegs020cd99c*)0x04000280;
            div->cnt = 0;
            div->numerLo = 0x10000000;
            div->denomLo = height;
            div->denomHi = 0;
            data_021117b0.y = rect->y;
            data_021117b0.height = rect->height;
            while (div->cnt & 0x8000) {
            }
            data_021117b0.invHeight = *(volatile int*)0x040002a0;
        } else {
            data_021117b0.y = 0;
            data_021117b0.height = 0;
            data_021117b0.invHeight = 0;
        }

        _Z20SetIRQInterruptStatei(irq);
        data_021117b0.valid = 1;
    }
}
