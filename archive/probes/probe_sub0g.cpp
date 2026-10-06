#include <globaldefs.h>

extern "C" void sink(int);

// 64-bit low-half identity ops: which spelling yields a NON flag-setting `sub Rd, Rs, #0`?
extern "C" ARM int y0(int x) { return (int)((long long)x - 0x8000000000000000LL); }
extern "C" ARM int y1(int x) { return (int)((long long)x - 0xffffffff00000000LL); }
extern "C" ARM int y2(int x, int n) { return (int)((long long)x - ((long long)n << 32)); }
extern "C" ARM int y3(int x, long long n) { return (int)((long long)x - n); }
extern "C" ARM int y4(int x) { return (int)(0x100000000LL - (long long)x); }
extern "C" ARM int y5(int x) { return (int)((long long)x & 0xffffffff00000000LL); }
extern "C" ARM int y6(int x) { return (int)((long long)x & ~0xffffffffLL); }
extern "C" ARM int y7(int x) { return (int)((long long)x ^ 0x100000000LL); }
extern "C" ARM int y8(unsigned x) { return (int)((unsigned long long)x - 0x100000000ULL); }
extern "C" ARM int y9(int x) { return (int)((long long)x * 0x100000001LL); }
extern "C" ARM int ya(int x) { unsigned long long v = (unsigned)x; return (int)(v - 0x100000000ULL); }
extern "C" ARM int yb(int x) { return (int)((long long)(unsigned)x - 0x100000000LL); }
extern "C" ARM int yc(int x) { long long v = x; v -= 0x100000000LL; return (int)v; }
extern "C" ARM int yd(int x) { return (int)((long long)x - 1LL * 0x100000000LL); }
