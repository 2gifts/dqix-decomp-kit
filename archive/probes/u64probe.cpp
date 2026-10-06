typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

struct Pair { u32 lo; u32 hi; };

extern "C" u64 fA(u16* p) {
    return ((u64)p[0] << 32) | (u32)(p[-2] | (p[-1] << 16));
}

extern "C" u64 fB(u16* p) {
    u64 v = p[0];
    v <<= 32;
    v |= (u32)(p[-2] | (p[-1] << 16));
    return v;
}

extern "C" Pair fC(u16* p) {
    Pair r;
    r.hi = p[0];
    r.lo = p[-2] | (p[-1] << 16);
    return r;
}

extern "C" u64 fD(u16* p) {
    u32 hi = p[0];
    u32 lo = p[-2] | (p[-1] << 16);
    return ((u64)hi << 32) + lo;
}

extern "C" u64 fE(u16* p) {
    union { u64 q; struct { u32 lo, hi; } w; } u;
    u.w.hi = p[0];
    u.w.lo = p[-2] | (p[-1] << 16);
    return u.q;
}

extern "C" u64 fF(u16* p) {
    return ((u64)p[0] << 32) ^ (u32)(p[-2] | (p[-1] << 16));
}
