#include <globaldefs.h>

extern "C" void sink(int);

struct One { int v; };
struct Two { int a; int b; };
struct BaseA { int a; };
struct BaseB { int b; };
struct Both : BaseA, BaseB { int c; };
struct Bits { unsigned f : 8; unsigned g : 24; };

// `add rN, sp, #0` is NOT folded in the ROM, so mwcc's ADDRESS emitter keeps a zero displacement.
// Which construct reaches that emitter with a SUB and a zero displacement?
extern "C" ARM int v0(One s, int d) { int i = s.v; sink(i); return i + d; }
extern "C" ARM int v1(Two s, int d) { int i = s.a; sink(i); return i + d; }
extern "C" ARM int v2(Two s, int d) { int i = s.b; sink(i); return i + d; }
extern "C" ARM int v3(int &r, int d) { int i = r; sink(i); return i + d; }
extern "C" ARM int v4(int *p, int d) { int i = (int)(long)&p[0]; sink(i); return i + d; }
extern "C" ARM int v5(int *p, int d) { int i = (int)(long)&p[-1]; sink(i); return i + d; }
extern "C" ARM int v6(Both *p) { BaseA *a = p; return a->a; }
extern "C" ARM int v7(Both *p) { BaseB *b = p; return b->b; }
extern "C" ARM int v8(BaseA *a) { Both *p = (Both *)a; return p->c; }
extern "C" ARM int v9(Bits *b, int d) { int i = (int)b->f; sink(i); return i + d; }
extern "C" ARM int va(int x, int d) { int i; *(volatile int *)&i = x; return i + d; }
extern "C" ARM int vb(int *p, int d) { int i = (int)(long)(p - 0); sink(i); return i + d; }
extern "C" ARM int vc(int d) { int a[4]; a[0] = d; sink((int)(long)&a[0]); return a[0]; }
extern "C" ARM int vd(int x, int d) { int i = x; sink(i); return i + d; }
