typedef unsigned int u32;
struct S { u32 a, b, c, d; };

// baseline: two zero locals get CSE'd into one register, so no stm is possible
extern "C" void w_two_zero(struct S* p)
{
    u32 z0 = 0, z1 = 0;
    p->a = z0; p->b = z1; p->c = z0; p->d = z1;
}

#pragma opt_common_subs off
extern "C" void w_two_zero_nocse(struct S* p)
{
    u32 z0 = 0, z1 = 0;
    p->a = z0; p->b = z1; p->c = z0; p->d = z1;
}

extern "C" void w_walk_nocse(struct S* p)
{
    u32 z0 = 0, z1 = 0;
    u32* q = (u32*)p;
    *q++ = z0; *q++ = z1;
    *q++ = z0; *q++ = z1;
}
#pragma opt_common_subs reset

extern "C" void w_walk_pair(struct S* p, u32 x, u32 y)
{
    u32* q = (u32*)p;
    *q++ = x; *q++ = y;
    *q++ = x; *q++ = y;
}
