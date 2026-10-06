#include <globaldefs.h>

extern "C" void sink(int);
struct Obj { int a; int b; int value; };
struct Obj2 { int a; int b; int c; int value; };

// The ROM helper is byte-for-byte the shape of an out-of-line `helper(obj, idx, base, dflt)` whose
// `base` operand is an immediate 0 instead of a register. Which spelling of a zero base survives
// the front end's fold?
template <int BASE>
static int tb(Obj *obj, int idx, int dflt) {
    int i = idx - BASE;
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return dflt;
}
extern "C" ARM int tb0(Obj *obj, int idx, int dflt) { return tb<0>(obj, idx, dflt); }

template <class T>
struct Traits { enum { BASE = 0 }; };

template <class T>
static int tc(T *obj, int idx, int dflt) {
    int i = idx - Traits<T>::BASE;
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return dflt;
}
extern "C" ARM int tc0(Obj *obj, int idx, int dflt) { return tc(obj, idx, dflt); }
extern "C" ARM int tc1(Obj2 *obj, int idx, int dflt) { return tc(obj, idx, dflt); }

struct Holder { static const int BASE; };
const int Holder::BASE = 0;
extern "C" ARM int td0(Obj *obj, int idx, int dflt) {
    int i = idx - Holder::BASE;
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return dflt;
}
