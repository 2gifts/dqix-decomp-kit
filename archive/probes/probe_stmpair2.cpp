typedef unsigned int u32;
struct S { u32 a, b, c, d; };
extern "C" void* memcpy(void*, const void*, unsigned int);

#pragma optimization_level 4
extern "C" void y_o4(struct S* p)
{
    p->a = 0; p->b = 0; p->c = 0; p->d = 0;
}
#pragma optimization_level 2

#pragma opt_unroll_loops on
extern "C" void y_unroll(struct S* p)
{
    int i;
    u32* q = (u32*)p;
    for (i = 0; i < 4; i++) q[i] = 0;
}
#pragma opt_unroll_loops off

extern "C" void y_memcpy(struct S* p, const struct S* s)
{
    memcpy(p, s, sizeof(struct S));
}

// two DIFFERENT live values into consecutive slots — the shape the ROM shows (two live registers)
extern "C" void y_pair(struct S* p, u32 x, u32 y)
{
    p->a = x;
    p->b = y;
    p->c = x;
    p->d = y;
}
