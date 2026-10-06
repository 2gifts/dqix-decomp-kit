// Does mwcc emit the ROM's unfused `and`/`cmp`/`b` guard instead of `tst` + predicated arm?
// Family: 020055e4, 02205b48, 0221474c, 0200af44, 020b25c4, 020b33f0 all report
// "mwcc if-converts / predicates the arm, the ROM branches over it".

extern "C" void pA(int x, int *d)
{
    if (x & 0x20) {
        d[0] = 1;
        d[1] = 2;
    }
}

extern "C" void pB(int x, int *d)
{
    int g = x & 0x20;
    if (g != 0) {
        d[0] = 1;
        d[1] = 2;
    }
}

extern "C" void pC(int x, int *d)
{
    int g = x & 0x20;
    if (g != 0) {
        d[0] = g;
        d[1] = 2;
    }
}

extern "C" void pD(int x, int *d)
{
    int g = x & 0x20;
    if (g != 0) {
        d[0] = 1;
        d[1] = 2;
    }
    d[2] = g;
}

extern "C" void pE(int x, int *d)
{
    if (!(x & 0x20)) {
        goto skip;
    }
    d[0] = 1;
    d[1] = 2;
skip:
    ;
}

extern "C" void pF(int x, int *d)
{
    int g = x & 0x20;
    int t = 0;
    if (g != 0) {
        t = 1;
        d[0] = 1;
        d[1] = 2;
    }
    d[2] = t;
}
