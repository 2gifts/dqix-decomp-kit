#include <globaldefs.h>

struct S { int a; int b; int value; };
enum E { EZERO = 0, EONE = 1 };
static const int kZero = 0;
struct Node { int link; int payload; };

template <int N>
static int tsub(int x) { return x - N; }

extern "C" ARM int p1(int x) { return x - 0; }
extern "C" ARM int p2(int x) { return x - EZERO; }
extern "C" ARM int p3(int x) { return x - kZero; }
extern "C" ARM int p4(int x) { return tsub<0>(x); }
extern "C" ARM int p5(int x) { return x - (int)((long)&((Node *)0)->link); }
extern "C" ARM int p6(int x) { return x - (int)(EONE - EONE); }
extern "C" ARM int p7(int x, int y) { return x - (y - y); }
extern "C" ARM int p8(unsigned x) { return (int)(x - 0u); }
extern "C" ARM int p9(int x) { return (int)((char *)0 + x - (char *)0); }

extern "C" ARM int q1(S *obj, int idx, int dflt) {
    int i = idx - EZERO;
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return dflt;
}

extern "C" ARM int q2(S *obj, int idx, int dflt) {
    int i = tsub<0>(idx);
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return dflt;
}
