// Isolate 02061c04 case 0xe4 (11 of the remaining 42 bytes).
//
//   ROM:  ldr r1,[pc] / ldrb r3,[r1,#0xc8] / cmp / moveq / movne / lsl r2,r0,#0x1f
//         bic r3,r3,#4 / orr r2,r3,r2,lsr #29 / mov r0,#1 / strb r2,[r1,#0xc8]
//   ours: the same instructions with (r1,r3,r2) -> (r3,r2,r1)
//
//   MWCC=2.0/sp2p2 python pad/probe_cc.py pad/probe_e4.cpp --bytes

#pragma opt_propagation off

struct Msg { unsigned short cmd, p1; };
struct Bits { unsigned char lo2 : 2, eqFlag : 1, hi5 : 5; };
extern "C" char data_blob[0x200];

extern "C" int k0(Msg *m)                       // the form the big function has
{
    ((Bits *)((char *)&data_blob + 0xc8))->eqFlag = (m->p1 == 0);
    return 1;
}

extern "C" int k1(Msg *m)                       // pointer bound to a local
{
    Bits *b = (Bits *)((char *)&data_blob + 0xc8);
    b->eqFlag = (m->p1 == 0);
    return 1;
}

extern "C" int k2(Msg *m)                       // whole struct indexed, offset in the member
{
    struct Wrap { char pad[0xc8]; Bits b; };
    ((Wrap *)&data_blob)->b.eqFlag = (m->p1 == 0);
    return 1;
}

extern "C" int k3(Msg *m)                       // comparison the other way round
{
    ((Bits *)((char *)&data_blob + 0xc8))->eqFlag = (0 == m->p1);
    return 1;
}

extern "C" int k4(Msg *m)                       // absolute address instead of the symbol
{
    ((Bits *)((char *)0x02109bf4 + 0xc8))->eqFlag = (m->p1 == 0);
    return 1;
}

extern "C" int k5(Msg *m)                       // ternary rather than a comparison result
{
    ((Bits *)((char *)&data_blob + 0xc8))->eqFlag = m->p1 ? 0 : 1;
    return 1;
}

extern "C" int k6(Msg *m)                       // read-modify-write written out
{
    Bits *b = (Bits *)((char *)&data_blob + 0xc8);
    unsigned char v = *(unsigned char *)b;
    v = (v & ~4) | ((m->p1 == 0) << 2);
    *(unsigned char *)b = v;
    return 1;
}

extern "C" int k8(Msg *m)                       // boolean bound to an unsigned char first
{
    unsigned char v = (m->p1 == 0);
    ((Bits *)((char *)&data_blob + 0xc8))->eqFlag = v;
    return 1;
}

extern "C" int k9(Msg *m)                       // logical not
{
    ((Bits *)((char *)&data_blob + 0xc8))->eqFlag = !m->p1;
    return 1;
}

extern "C" int k10(Msg *m)                      // the pointer read before the comparison
{
    Bits *b = (Bits *)((char *)&data_blob + 0xc8);
    unsigned char keep = b->hi5;
    b->eqFlag = (m->p1 == 0);
    b->hi5 = keep;
    return 1;
}

extern "C" int k11(Msg *m)                      // comparison against a local, not a literal
{
    int zero = 0;
    ((Bits *)((char *)&data_blob + 0xc8))->eqFlag = (m->p1 == zero);
    return 1;
}

extern "C" int k7(Msg *m)                       // the offset folded into the symbol expression
{
    ((Bits *)&data_blob[0xc8])->eqFlag = (m->p1 == 0);
    return 1;
}
