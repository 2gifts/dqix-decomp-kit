// Bisect between a KNOWN-GOOD shape and 02061c04's case 0xe4.
//
// StoreByteAtCountTail020a1fd4 is committed and byte-exact, and it puts the pool pointer in r1 --
// the direction case 0xe4 needs and never gets. Rather than guess again, morph the working function
// one step at a time toward the failing one and read off the step where the register flips.
//
//   MWCC=2.0/sp2p2 python pad/probe_cc.py pad/probe_morph.cpp --bytes

#pragma opt_propagation off

struct Msg { unsigned short cmd, p1; };
struct Bits { unsigned char lo2 : 2, eqFlag : 1, hi5 : 5; };
extern unsigned char data_a;
extern unsigned char data_b;

// m0: the committed function verbatim -- pool in r1
extern "C" void m0(unsigned char value)
{
    if (*(volatile unsigned char *)(&data_a + 1) != 0)
        (&data_b)[*(volatile unsigned char *)(&data_a + 1) - 1] = value;
}

// m1: same, but the guard is gone -- one read, one write, like 0xe4
extern "C" void m1(unsigned char value)
{
    (&data_b)[*(volatile unsigned char *)(&data_a + 1) - 1] = value;
}

// m2: the store target becomes a BITFIELD at a fixed offset
extern "C" void m2(unsigned char value)
{
    ((volatile Bits *)(&data_a + 0xc8))->eqFlag = value;
}

// m3: same, non-volatile
extern "C" void m3(unsigned char value)
{
    ((Bits *)(&data_a + 0xc8))->eqFlag = value;
}

// m4: the value becomes a comparison of a field reached through a pointer parameter
extern "C" void m4(Msg *msg)
{
    ((Bits *)(&data_a + 0xc8))->eqFlag = (msg->p1 == 0);
}

// m5: and a return value, which is 0xe4 exactly
extern "C" int m5(Msg *msg)
{
    ((Bits *)(&data_a + 0xc8))->eqFlag = (msg->p1 == 0);
    return 1;
}

// m7..m12: m5 with the RETURN VALUE expressed differently -- the bisection says `return 1;` is
// exactly what pushes the pool pointer from r2 up to r3.
extern "C" int m7(Msg *msg)
{
    int rv = 1;
    ((Bits *)(&data_a + 0xc8))->eqFlag = (msg->p1 == 0);
    return rv;
}

extern "C" int m8(Msg *msg)
{
    unsigned char v = (msg->p1 == 0);
    ((Bits *)(&data_a + 0xc8))->eqFlag = v;
    return 1;
}

extern "C" int m9(Msg *msg)
{
    Bits *f = (Bits *)(&data_a + 0xc8);
    int rv = 1;
    f->eqFlag = (msg->p1 == 0);
    return rv;
}

extern "C" int m10(Msg *msg)
{
    int rv;
    ((Bits *)(&data_a + 0xc8))->eqFlag = (msg->p1 == 0);
    rv = 1;
    return rv;
}

extern "C" unsigned int m11(Msg *msg)
{
    ((Bits *)(&data_a + 0xc8))->eqFlag = (msg->p1 == 0);
    return 1;
}

extern "C" int m12(Msg *msg)
{
    ((volatile Bits *)(&data_a + 0xc8))->eqFlag = (msg->p1 == 0);
    return 1;
}

// m13..m16: the bitfield insert written out BY HAND. m1->m2 showed the bitfield store is what first
// pushes the pool off r1, so spell the ROM's `lsl #31` / `lsr #29` pair explicitly and keep the
// store an ordinary byte store, the form the two committed r1/r2 examples use.
extern "C" int m13(Msg *msg)
{
    unsigned char *p = (unsigned char *)&data_a + 0xc8;
    unsigned int b = *p;
    unsigned int v = (unsigned int)(msg->p1 == 0);
    *p = (unsigned char)((b & ~4u) | ((v << 31) >> 29));
    return 1;
}

extern "C" int m14(Msg *msg)
{
    volatile unsigned char *p = (volatile unsigned char *)&data_a + 0xc8;
    unsigned int b = *p;
    unsigned int v = (unsigned int)(msg->p1 == 0);
    *p = (unsigned char)((b & ~4u) | ((v << 31) >> 29));
    return 1;
}

extern "C" int m15(Msg *msg)
{
    unsigned int v = (unsigned int)(msg->p1 == 0);
    unsigned char *p = (unsigned char *)&data_a + 0xc8;
    *p = (unsigned char)((*p & ~4u) | ((v << 31) >> 29));
    return 1;
}

extern "C" int m16(Msg *msg)
{
    unsigned char *p = (unsigned char *)&data_a + 0xc8;
    *p = (unsigned char)((*p & ~4u) | ((((unsigned int)(msg->p1 == 0)) << 31) >> 29));
    return 1;
}

// m17..m22: explicit shifts, sweeping WHERE each of the three values is bound. m15 already emits the
// ROM's exact instruction sequence and differs only in the names; m13 reaches pool=r2 but reorders.
// The target is the ROM's order AND pool=r1.
extern "C" int m17(Msg *msg)
{
    unsigned char *p = (unsigned char *)&data_a + 0xc8;
    unsigned int b = *p;
    *p = (unsigned char)((b & ~4u) | ((((unsigned int)(msg->p1 == 0)) << 31) >> 29));
    return 1;
}

extern "C" int m18(Msg *msg)
{
    unsigned char *p = (unsigned char *)&data_a + 0xc8;
    unsigned char b = *p;
    unsigned int v = (unsigned int)(msg->p1 == 0);
    *p = (unsigned char)((b & ~4u) | ((v << 31) >> 29));
    return 1;
}

extern "C" int m19(Msg *msg)
{
    unsigned int v;
    unsigned char *p = (unsigned char *)&data_a + 0xc8;
    unsigned int b = *p;
    v = (unsigned int)(msg->p1 == 0);
    *p = (unsigned char)((b & ~4u) | ((v << 31) >> 29));
    return 1;
}

extern "C" int m20(Msg *msg)
{
    unsigned int b;
    unsigned char *p = (unsigned char *)&data_a + 0xc8;
    unsigned int v = (unsigned int)(msg->p1 == 0);
    b = *p;
    *p = (unsigned char)((b & ~4u) | ((v << 31) >> 29));
    return 1;
}

extern "C" int m21(Msg *msg)
{
    unsigned char *p;
    unsigned int b;
    unsigned int v;
    p = (unsigned char *)&data_a + 0xc8;
    b = *p;
    v = (unsigned int)(msg->p1 == 0);
    *p = (unsigned char)((b & ~4u) | ((v << 31) >> 29));
    return 1;
}

extern "C" int m22(Msg *msg)
{
    unsigned char *p = (unsigned char *)&data_a + 0xc8;
    unsigned int b = *p;
    unsigned int v = (unsigned int)(msg->p1 == 0);
    unsigned int r = (b & ~4u) | ((v << 31) >> 29);
    *p = (unsigned char)r;
    return 1;
}

// m6: m5 but the message field is read into r0 by an earlier statement, as the switch does
extern "C" int m6(Msg *msg)
{
    int p = msg->p1;
    ((Bits *)(&data_a + 0xc8))->eqFlag = (p == 0);
    return 1;
}
