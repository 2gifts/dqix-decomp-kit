// Does an INLINED helper allocate the scratch pair differently?
//
// 512 source variants, 24 builds, 29 flag sets and 40 pragmas all give `add r2` for case 0xd3's
// address. An inlined function body reaches the register allocator through a different path than a
// hand-written expression, and `-inline noauto` means a plain `inline` helper stays out-of-line
// here (probe_d3c.cpp n6 emitted its own section), so it has never actually been tested.

#pragma opt_propagation off

struct Msg { unsigned short cmd, p1; };
extern "C" void *GetP(void);

#pragma always_inline on
static void SetBitAt(void *p, int bit, int one)
{
    char *base = (char *)p + 0x1840;
    unsigned int *w = (unsigned int *)(base + 0xb4c);
    *w = *w | (one << bit);
}

static void SetBitAt2(void *p, int bit)
{
    unsigned int *w = (unsigned int *)((char *)p + 0x1840 + 0xb4c);
    *w |= 1 << bit;
}
#pragma always_inline off

extern "C" int r0f(Msg *m)                      // inlined helper, three parameters
{
    int bit = m->p1;
    int one = 1;
    SetBitAt(GetP(), bit, one);
    return one;
}

extern "C" int r1f(Msg *m)                      // inlined helper, folded offset
{
    int bit = m->p1;
    int one = 1;
    SetBitAt2(GetP(), bit);
    return one;
}

extern "C" int r2f(Msg *m)                      // call result bound first, then inlined
{
    int bit = m->p1;
    int one = 1;
    void *p0 = GetP();
    SetBitAt(p0, bit, one);
    return one;
}
