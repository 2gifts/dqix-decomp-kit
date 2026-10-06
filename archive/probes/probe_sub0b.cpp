#include <globaldefs.h>

extern "C" void sink(int);
extern "C" int src(void);

// switch controls: if a switch starting at N lowers to `sub Rd, Rs, #N`, then N == 0 is the answer.
extern "C" ARM int s0(int x) {
    switch (x) {
    case 0: return 11;
    case 1: return 22;
    case 2: return 33;
    case 3: return 44;
    case 4: return 55;
    case 5: return 66;
    }
    return 0;
}

extern "C" ARM int s5(int x) {
    switch (x) {
    case 5: return 11;
    case 6: return 22;
    case 7: return 33;
    case 8: return 44;
    case 9: return 55;
    case 10: return 66;
    }
    return 0;
}

extern "C" ARM int u0(int x) { return (unsigned)x < 4u ? 1 : 0; }
extern "C" ARM int u1(int x) { return (x >= 0 && x < 4) ? 1 : 0; }
extern "C" ARM int u2(int x, int n) { return (unsigned)(x - 0) < (unsigned)n ? 1 : 0; }

extern "C" ARM int l0(int x) { return (int)((long long)x - 0); }
extern "C" ARM int l1(int x) { long long v = (long long)x; return (int)(v - 0LL); }
extern "C" ARM int l2(int x) { return ((long long)x != -1LL) ? 1 : 0; }

extern "C" ARM int m0(int x) { int i = x; i -= 0; return i; }
extern "C" ARM int m1(int x) { int i = x; sink(i); return i + x; }
extern "C" ARM int m2(int x) { return -(0 - x); }
extern "C" ARM int m3(int x) { return 0 - (0 - x); }

extern "C" ARM int d0(int x) { return x / 1; }
extern "C" ARM int d1(int x) { return x % 1; }
extern "C" ARM int d2(int x) { return x * 1; }

extern "C" ARM int c0(int x) { return x == -1 ? 0 : x; }
extern "C" ARM int c1(int x) { return x != -1 ? x : 0; }
