// Does mwcc honour GCC-style explicit register variables? The project compiles with -gccext,on.
//
// If `register T x asm("r1")` binds, the four remaining 02061c04 residues -- all of which are pure
// register NAMING with the instruction sequence already identical -- become a one-line fix each,
// in C, with no assembly.

#pragma opt_propagation off

struct Msg { unsigned short cmd, p1; };
extern "C" void *GetP(void);

extern "C" int s0(Msg *m)                       // baseline: address lands in r2
{
    int bit = m->p1;
    int one = 1;
    void *p0 = GetP();
    char *base = (char *)p0 + 0x1840;
    unsigned int *w = (unsigned int *)(base + 0xb4c);
    *w = *w | (one << bit);
    return one;
}

extern "C" int s1(Msg *m)                       // explicit register variable, declared FIRST
{
    register char *base asm("r1");
    int bit = m->p1;
    int one = 1;
    void *p0 = GetP();
    base = (char *)p0 + 0x1840;
    unsigned int *w = (unsigned int *)(base + 0xb4c);
    *w = *w | (one << bit);
    return one;
}
