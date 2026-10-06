// 020d6c0c: one instruction from a match. The ROM loads the small constant 0x22 from its own
// literal pool (`ldr r0, [pc, #0x14]`); every C form compiles it to `mov r0, #0x22`, 4 bytes short.
// 0x22 IS an encodable ARM immediate, so what makes mwcc spill a small constant to the pool?

extern "C" int use(int);

extern "C" int p_plain(void) { return use(0x22); }

extern "C" int p_static_const(void)
{
    static const int k = 0x22;
    return use(k);
}

extern "C" int p_local_const(void)
{
    const int k = 0x22;
    return use(k);
}

#pragma opt_propagation off
extern "C" int p_local_nopropagate(void)
{
    const int k = 0x22;
    return use(k);
}

extern "C" int p_local_var_nopropagate(void)
{
    int k = 0x22;
    return use(k);
}
#pragma opt_propagation reset

#pragma optimization_level 1
extern "C" int p_o1(void) { return use(0x22); }
#pragma optimization_level 2

#pragma optimization_level 0
extern "C" int p_o0(void) { return use(0x22); }
#pragma optimization_level 2

extern "C" const int g_k;
extern "C" int p_extern_const(void) { return use(g_k); }
