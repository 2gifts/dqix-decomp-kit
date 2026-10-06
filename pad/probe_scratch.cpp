// Is SCRATCH register allocation (r0-r3, ip, lr) reachable from the source at all?
// 020371b8 (7B), 02093b90 (14B) and 02069fec all end on "pure scratch-register swap", each after
// 8+ reorderings with byte-identical output. If nothing moves it, workers should SKIP on sight.

typedef long long s64;
typedef int s32;

extern "C" s64 s_ab(s32 a, s32 b, s32 t)
{
    s64 m = (s64)a * b;
    return m - t;
}

extern "C" s64 s_ba(s32 a, s32 b, s32 t)
{
    s64 m = (s64)b * a;
    return m - t;
}

extern "C" s64 s_split(s32 a, s32 b, s32 t)
{
    s64 m;
    s64 r;
    m = (s64)a * b;
    r = m - t;
    return r;
}

extern "C" s64 s_inline(s32 a, s32 b, s32 t)
{
    return (s64)a * b - t;
}

extern "C" s64 s_tlast(s32 a, s32 b, s32 t)
{
    s32 u = t;
    s64 m = (s64)a * b;
    return m - u;
}
