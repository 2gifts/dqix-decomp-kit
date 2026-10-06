// 020c197c and 02025f28 both end at the same wall: the ROM packs consecutive word stores into
// `stm rD!, {rX, rY}` with two live registers, and no plain-field C shape reproduces it.
// Which source form makes mwcc emit the stm pair?

typedef unsigned int u32;
typedef unsigned long long u64;

struct S { u32 a, b, c, d; };

extern "C" void z_fields(struct S* p)
{
    p->a = 0;
    p->b = 0;
    p->c = 0;
    p->d = 0;
}

extern "C" void z_locals(struct S* p)
{
    u32 z0 = 0, z1 = 0;
    p->a = z0;
    p->b = z1;
    p->c = z0;
    p->d = z1;
}

extern "C" void z_walk(struct S* p)
{
    u32* q = &p->a;
    *q++ = 0;
    *q++ = 0;
    *q++ = 0;
    *q++ = 0;
}

extern "C" void z_u64(struct S* p)
{
    *(u64*)&p->a = 0;
    *(u64*)&p->c = 0;
}

extern "C" void z_u64_walk(struct S* p)
{
    u64* q = (u64*)&p->a;
    *q++ = 0;
    *q++ = 0;
}

extern "C" void z_aggregate(struct S* p)
{
    static const struct S zero = {0, 0, 0, 0};
    *p = zero;
}
