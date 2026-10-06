#include <globaldefs.h>

extern "C" void sink(int);
struct Obj { int a; int b; int value; };

// The ROM's `orr Rd, Rs, #0` sites are all one half of a 64-bit operation whose constant has a zero
// word (0200a498 pairs `orr r0, r0, #0` with `orr r1, r1, #0x80000`). If `sub Rd, Rs, #0` is the
// same thing -- the low half of a 64-bit subtract whose high half is dead -- these reproduce it.
extern "C" ARM int k0(int x) { return (int)((long long)x - 0x100000000LL); }
extern "C" ARM int k1(int x) { return (int)((long long)x - 0x200000000LL); }
extern "C" ARM unsigned k2(unsigned x) { return (unsigned)((unsigned long long)x - 0x100000000ULL); }
extern "C" ARM int k3(long long x) { return (int)(x - 0x100000000LL); }
extern "C" ARM int k4(int x) { long long v = (long long)x - 0x100000000LL; sink((int)v); return (int)v; }
extern "C" ARM int k5(int x) { return (int)((long long)x | 0x100000000LL); }
extern "C" ARM int k6(int x) { return (int)((long long)x + 0x100000000LL); }

extern "C" ARM int j0(Obj *obj, int idx, int dflt) {
    int i = (int)((long long)idx - 0x100000000LL);
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return dflt;
}
