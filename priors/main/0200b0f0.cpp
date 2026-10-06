// HOLD: best attempt, semantically-correct IEEE754 soft-double multiply (extract exp/mantissa,
// 64x64->128 partial-product multiply, normalize, round-to-nearest-even, denormal path, full
// zero/inf/nan matrix). Logic verified against every branch in the target listing by hand.
// BLOCKER: SIZE/OVERGEN, not regalloc. Compiles to 0x3fc-0x40c bytes vs target 0x364 (~150-170
// bytes over) in EVERY formulation tried, and mwcc reaches for r8/sb + spills both double args to
// the stack in the first few instructions -- even in a goto-free, special-case-free skeleton of
// just extract+multiply+pack (see scratch_probe attempts). Target uses ONLY r0-r7,ip,lr, zero
// spills, fully register-resident, and is tuned to the byte (e.g. mantissa shifted by 11 not 12 so
// the implicit-bit OR lands for free, a stray dead `mov r1,r3` at the shared NaN-return site).
// Ruled out: goto-heavy CFG vs plain if/else (no difference), union-copy vs pointer-cast parameter
// extraction (no difference), unsigned long long partial products vs manual 32-bit carry tracking
// (both spill). This 10-register-exact allocation with zero slack across a 217-instruction routine
// reads as genuine hand-written assembly (a soft-float library primitive), not compiler output --
// matches the documented "genuine hand-asm" skip class. Logic below is a faithful reference even
// if never byte-exact; keep for any future structural insight.
#include <globaldefs.h>

union DoubleBits0200b0f0 {
    struct { unsigned int lo, hi; } w;
    double d;
};

// USA: func_0200b0f0
extern "C" ARM double func_0200b0f0(double a, double b) {
    union DoubleBits0200b0f0 ua, ub, result;
    ua.d = a;
    ub.d = b;
    unsigned int r0 = ua.w.lo;
    unsigned int r1 = ua.w.hi;
    unsigned int r2 = ub.w.lo;
    unsigned int r3 = ub.w.hi;
    unsigned int r4, r5, r6, r7, ip, lr;
    (void)r7;

    lr = (r1 ^ r3) & 0x80000000u;
    ip = r1 >> 20;
    r1 = (r1 << 11) | (r0 >> 21);
    r0 = r0 << 11;
    r6 = ip << 21;
    if (r6 == 0 || (int)(r6 + 0x200000u) == 0) goto SpecialA;
    r1 |= 0x80000000u;
    ip &= ~0x800u;

    r4 = r3 >> 20;
    r3 = (r3 << 11) | (r2 >> 21);
    r2 = r2 << 11;
    r5 = r4 << 21;
    if (r5 == 0 || (int)(r5 + 0x200000u) == 0) goto SpecialB;
    r3 |= 0x80000000u;
    r4 &= ~0x800u;

Multiply:
    ip = r4 + ip;
    {
        unsigned long long m = (unsigned long long)r0 * r2;
        r5 = (unsigned int)m;
        r4 = (unsigned int)(m >> 32);

        m = (unsigned long long)r0 * r3;
        r7 = (unsigned int)m;
        r6 = (unsigned int)(m >> 32);

        m = (unsigned long long)r7 + r4;
        r4 = (unsigned int)m;
        r6 = r6 + (unsigned int)(m >> 32);

        m = (unsigned long long)r1 * r2;
        r7 = (unsigned int)m;
        r0 = (unsigned int)(m >> 32);

        m = (unsigned long long)r7 + r4;
        r4 = (unsigned int)m;
        m = (unsigned long long)r0 + r6 + (unsigned int)(m >> 32);
        r0 = (unsigned int)m;
        r6 = (unsigned int)(m >> 32);

        m = (unsigned long long)r1 * r3;
        r7 = (unsigned int)m;
        r2 = (unsigned int)(m >> 32);

        r1 = r2 + r6;

        m = (unsigned long long)r0 + r7;
        r0 = (unsigned int)m;
        r1 = r1 + (unsigned int)(m >> 32);
    }
    if ((r4 | r5) != 0) r0 |= 1;

    if ((int)r1 < 0) goto NoNormShift;
    ip = ip - 1;
    {
        unsigned long long v = (((unsigned long long)r1 << 32) | r0) << 1;
        r0 = (unsigned int)v;
        r1 = (unsigned int)(v >> 32);
    }
NoNormShift:
    ip = ip + 2;
    ip = ip - 0x400;
    if ((int)ip <= 0) goto Underflow;

    r6 = ip << 20;
    if ((int)(r6 + 0x100000u) < 0) goto Overflow;

    r2 = r0 << 21;
    r0 = (r0 >> 11) | (r1 << 21);
    r1 = r1 + r1;
    r1 = lr | (r1 >> 12);
    r1 = r1 | (ip << 20);

    if (r2 == 0) goto Pack;
    if ((r2 & 0x80000000u) == 0) goto Pack;
    r2 = r2 << 1;
    if (r2 == 0) {
        r2 = r0 & 1;
        if (r2 == 0) goto Pack;
    }
    {
        unsigned long long packed = (((unsigned long long)r1 << 32) | r0) + 1;
        r0 = (unsigned int)packed;
        r1 = (unsigned int)(packed >> 32);
    }

Pack:
    result.w.lo = r0;
    result.w.hi = r1;
    return result.d;

SpecialA:
    ip &= ~0x800u;
    if (ip == 0) goto ZeroOrSubA;
    if ((r0 | (r1 << 1)) != 0) goto ReturnNaN1;
    r4 = r3 >> 20;
    r3 = (r3 << 11) | (r2 >> 21);
    r2 = r2 << 11;
    r5 = r4 << 21;
    if (r5 == 0) goto BZeroFromInfA;
    if ((int)(r5 + 0x200000u) != 0) goto ReturnInf1;
    if ((r2 | (r3 << 1)) == 0) goto ReturnInf1;
    goto ReturnNaN1;

BZeroFromInfA:
    if ((r3 | r2) == 0) goto ReturnNaN2;
    goto ReturnInf1;

SpecialB:
    r4 &= ~0x800u;
    if (r4 == 0) goto ZeroOrSubB;
    if ((r2 | (r3 << 1)) != 0) goto ReturnNaN1;
    goto ReturnInf1;

ZeroOrSubA:
    if ((r0 | (r1 << 1)) == 0) goto ZeroFromA;
    ip = 1;
    if (r1 != 0) goto ClzA;
    ip = ip - 0x20;
    r1 = r0;
    r0 = 0;
    if ((int)r1 < 0) goto AfterClzA;
ClzA:
    {
        unsigned int n;
        asm { clz n, r1 }
        {
            unsigned long long v = (((unsigned long long)r1 << 32) | r0) << n;
            r1 = (unsigned int)(v >> 32);
            r0 = (unsigned int)v;
        }
        ip = ip - n;
    }
AfterClzA:
    r4 = r3 >> 20;
    r3 = (r3 << 11) | (r2 >> 21);
    r2 = r2 << 11;
    r5 = r4 << 21;
    if (r5 == 0 || (int)(r5 + 0x200000u) == 0) goto SpecialB;
    r3 |= 0x80000000u;
    r4 &= ~0x800u;
    goto Multiply;

ZeroFromA:
    r4 = r3 >> 20;
    r3 = (r3 << 11) | (r2 >> 21);
    r2 = r2 << 11;
    r5 = r4 << 21;
    if (r5 == 0) goto ReturnZero1;
    if ((int)(r5 + 0x200000u) != 0) goto ReturnZero1;
    if ((r2 | (r3 << 1)) == 0) goto ReturnNaN2;
    goto ReturnNaN1;

ZeroOrSubB:
    if ((r2 | (r3 << 1)) == 0) goto ReturnZero1;
    r4 = 1;
    if (r3 != 0) goto ClzB;
    r4 = r4 - 0x20;
    r3 = r2;
    r2 = 0;
    if ((int)r3 < 0) goto Multiply;
ClzB:
    {
        unsigned int n;
        asm { clz n, r3 }
        {
            unsigned long long v = (((unsigned long long)r3 << 32) | r2) << n;
            r3 = (unsigned int)(v >> 32);
            r2 = (unsigned int)v;
        }
        r4 = r4 - n;
    }
    goto Multiply;

Underflow:
    {
        int t = (int)ip + 0x34;
        if (t == 0) goto UnderBoundary;
        if (t < 0) goto ReturnZero2;
    }
    {
        unsigned int sr2 = r1, sr3 = r0;
        unsigned int shamt = ip + 0x34;
        if (shamt >= 0x20) { sr2 = sr3; sr3 = 0; shamt -= 0x20; }
        {
            unsigned long long v = (((unsigned long long)sr2 << 32) | sr3) << shamt;
            r2 = (unsigned int)(v >> 32);
            if ((unsigned int)v != 0) r2 |= 1;
        }
    }
    {
        unsigned int nr0 = r0, nr1 = r1;
        unsigned int shamt2 = 0xc - ip;
        if (shamt2 >= 0x20) { nr0 = nr1; nr1 = 0; shamt2 -= 0x20; }
        {
            unsigned long long v2 = (((unsigned long long)nr1 << 32) | nr0) >> shamt2;
            r0 = (unsigned int)v2;
            r1 = lr | (unsigned int)(v2 >> 32);
        }
    }

    if (r2 == 0) goto PackDenorm;
    if ((r2 & 0x80000000u) == 0) goto PackDenorm;
    r2 = r2 << 1;
    if (r2 == 0) {
        r2 = r0 & 1;
        if (r2 == 0) goto PackDenorm;
    }
    {
        unsigned long long packed = (((unsigned long long)r1 << 32) | r0) + 1;
        r0 = (unsigned int)packed;
        r1 = (unsigned int)(packed >> 32);
    }

PackDenorm:
    result.w.lo = r0;
    result.w.hi = r1;
    return result.d;

UnderBoundary:
    r0 = r0 | (r1 << 1);
    r2 = r0;
    result.w.hi = lr;
    result.w.lo = (r2 != 0) ? 1u : 0u;
    return result.d;

ReturnInf1:
    result.w.hi = lr | 0x7ff00000u;
    result.w.lo = 0;
    return result.d;

Overflow:
    result.w.hi = lr | 0x7ff00000u;
    result.w.lo = 0;
    return result.d;

ReturnNaN1:
    result.w.lo = 0xFFFFFFFFu;
    result.w.hi = 0x7FFFFFFFu;
    return result.d;

ReturnNaN2:
    result.w.lo = 0xFFFFFFFFu;
    result.w.hi = 0x7FFFFFFFu;
    return result.d;

ReturnZero1:
    result.w.hi = lr;
    result.w.lo = 0;
    return result.d;

ReturnZero2:
    result.w.hi = lr;
    result.w.lo = 0;
    return result.d;
}
