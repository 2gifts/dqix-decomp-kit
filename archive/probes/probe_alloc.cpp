// What does mwcc key its CALLEE-SAVED register order on?
//
// 02061c04's residue is four cases where our register NAMES differ from the ROM's while every
// instruction matches. Case 0xe7 is the expensive one: the ROM defines `battle` first and still
// gives the later-defined `rec` the lower register, which "callee-saved follow definition order"
// cannot explain. Each gN() holds four values live across calls and varies ONE property.
//
//   MWCC=2.0/sp2p2 python pad/probe_cc.py pad/probe_alloc.cpp --bytes

#pragma opt_propagation off

extern "C" int F(int);
extern "C" int G(int, int, int, int);

extern "C" int g0(void)                         // equal use counts, plain definition order
{
    int a = F(0);
    int b = F(1);
    int c = F(2);
    int d = F(3);
    return G(a, b, c, d);
}

extern "C" int g1(void)                         // `a` used once, the rest four times each
{
    int a = F(0);
    int b = F(1);
    int c = F(2);
    int d = F(3);
    G(b, c, d, 0);
    G(b, c, d, 1);
    G(b, c, d, 2);
    return G(a, b, c, d);
}

extern "C" int g2(void)                         // `d` used once, the rest four times each
{
    int a = F(0);
    int b = F(1);
    int c = F(2);
    int d = F(3);
    G(a, b, c, 0);
    G(a, b, c, 1);
    G(a, b, c, 2);
    return G(a, b, c, d);
}

extern "C" int g3(void)                         // `a` dies early, the rest live to the end
{
    int a = F(0);
    int b = F(1);
    int c = F(2);
    int d = F(3);
    F(a);
    G(b, c, d, 0);
    G(b, c, d, 1);
    return G(b, c, d, 2);
}

extern "C" int g5(int cond)                     // `d` defined inside a nested block
{
    int a = F(0);
    int b = F(1);
    int c = F(2);
    if (cond) {
        int d = F(3);
        G(a, b, c, d);
        G(a, b, c, d);
        G(a, b, c, d);
    }
    return G(a, b, c, 0);
}

extern "C" int g6(int cond)                     // f0's shape: a chain of derived values, then a
{                                               // nested block that adds one more
    int a = F(0);
    int b = a + 0x104;
    int c = b + 0x7400;
    if (c & 1) {
        int d = F(a);
        G(b, c, d, 0);
        G(b, c, d, 1);
        G(a, c, d, 2);
    }
    return 1;
}

extern "C" int g4(void)                         // `d` derived from `a`, as rec is from battle
{
    int a = F(0);
    int d = a + 0x7504;
    int b = F(1);
    int c = F(2);
    G(d, b, c, 0);
    G(d, b, c, 1);
    G(d, b, c, 2);
    return G(a, b, c, d);
}
