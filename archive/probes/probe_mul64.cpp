// 0200cf24: body byte-matches but the target's epilogue is `push {r4,r5,lr} ... bx lr` (the umull
// temps live in CALLEE-SAVED registers) while every C form gives `push {r3,lr}` / `pop {r3,pc}`
// with the temps in ip/lr. 4 bytes short. What makes a LEAF function use r4/r5 for its temps?

typedef unsigned long long u64;
typedef unsigned int u32;

extern "C" u64 k_plain(u64 a, u64 b)
{
    return a * b;
}

extern "C" u64 k_local(u64 a, u64 b)
{
    u64 r = a * b;
    return r;
}

// extra live values, to see whether pressure alone pushes the temps into r4/r5
extern "C" u64 k_pressure(u64 a, u64 b)
{
    u64 r = a * b;
    u32 x = (u32)a + 1;
    u32 y = (u32)b + 2;
    u32 z = (u32)(a >> 32) + 3;
    return r + x + y + z;
}

// hand-split 32x32 halves, the shape the ROM's three instructions actually spell
extern "C" u64 k_split(u64 a, u64 b)
{
    u32 al = (u32)a, ah = (u32)(a >> 32);
    u32 bl = (u32)b, bh = (u32)(b >> 32);
    u64 lo = (u64)al * bl;
    u32 hi = (u32)(lo >> 32) + al * bh + bl * ah;
    return ((u64)hi << 32) | (u32)lo;
}
