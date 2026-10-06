typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

union U { u64 q; struct { u32 lo, hi; } w; };

extern "C" u64 gA(volatile u16* p) {
    u64 v = (u64)p[0] << 32;
    *(u32*)&v = ((u32)p[-1] << 16) | (u32)p[-2];
    return v;
}

extern "C" u64 gB(volatile u16* p) {
    register union U u;
    u.w.hi = p[0];
    u.w.lo = ((u32)p[-1] << 16) | (u32)p[-2];
    return u.q;
}

extern "C" u64 gC(volatile u16* p) {
    u64 hi = p[0];
    u32 lo = ((u32)p[-1] << 16) | (u32)p[-2];
    return (hi << 32) | lo;
}

extern "C" u64 gD(volatile u16* p) {
    u16 h = p[0];
    u16 a = p[-1];
    u16 b = p[-2];
    return ((u64)h << 32) | (u32)(((u32)a << 16) | (u32)b);
}
