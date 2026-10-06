#include <globaldefs.h>

extern "C" void sink(int);
union U64 { long long v; struct { unsigned lo; unsigned hi; } w; };
struct Pair { unsigned lo; unsigned hi; };

// Hunting a NON flag-setting `sub Rd, Rs, #0`: the committed corpus shows `adds`/`subs` with #0
// whenever the carry chain is live (NormalizeAndScaleVec3, SplitTimestampIntoDateAndTime), so the
// ROM's plain `sub` means the high half is gone AND the S bit was cleaned.
extern "C" ARM int q0(int x, int hi) { U64 k; k.w.lo = 0; k.w.hi = hi; return (int)((long long)x - k.v); }
extern "C" ARM int q1(int x, int hi) { long long k = 0; k = (long long)hi << 32; return (int)((long long)x - k); }
extern "C" ARM int q2(int x, int hi) { return (int)((long long)x - (long long)hi * 0x100000000LL); }
extern "C" ARM int q3(unsigned x, unsigned hi) { return (int)((unsigned long long)x - ((unsigned long long)hi << 32)); }
extern "C" ARM int q4(int x, int hi) { return (int)(((long long)x - ((long long)hi << 32)) >> 0); }
extern "C" ARM int q5(int x, int hi) { long long v = (long long)x - ((long long)hi << 32); return (int)(unsigned)v; }
extern "C" ARM int q6(int x, Pair *p) { U64 k; k.w.lo = 0; k.w.hi = p->hi; return (int)((long long)x - k.v); }
extern "C" ARM int q7(int x, int hi) { return (int)(((long long)x << 32) - ((long long)hi << 32)) ; }
extern "C" ARM int q8(int x, int hi) { return (int)((((long long)x << 32) - ((long long)hi << 32)) >> 32); }
extern "C" ARM int q9(int x, int hi) { return (int)((long long)x + (long long)hi * -0x100000000LL); }
extern "C" ARM int qa(int x, int hi) { long long v = (long long)x; v -= (long long)hi << 32; return (int)v; }
extern "C" ARM int qb(int x, int hi) { return (int)((long long)x - ((long long)(hi + 1) << 32)); }
extern "C" ARM int qc(int x, unsigned hi) { return (int)((long long)x - (long long)((unsigned long long)hi << 32)); }
