typedef unsigned int u32;
struct S { u32 a, b, c, d; };

#pragma opt_propagation off
extern "C" void v_prop_off(struct S* p)
{
    u32 z0 = 0, z1 = 0;
    p->a = z0; p->b = z1; p->c = z0; p->d = z1;
}

extern "C" void v_prop_off_walk(struct S* p)
{
    u32 z0 = 0, z1 = 0;
    u32* q = (u32*)p;
    *q++ = z0; *q++ = z1;
    *q++ = z0; *q++ = z1;
}
#pragma opt_propagation reset

// zeros that propagation cannot prove equal: one comes from a parameter the caller always passes 0
extern "C" void v_param_zero(struct S* p, u32 z1)
{
    u32 z0 = 0;
    p->a = z0; p->b = z1; p->c = z0; p->d = z1;
}
