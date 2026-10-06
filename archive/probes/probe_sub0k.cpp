#include <globaldefs.h>

struct Obj { int a; int b; int value; };

// The inliner substitutes an argument AFTER the front end has already folded the expression, so a
// literal 0 passed to an inline helper reaches the backend as a real `sub` node with immediate 0.
static inline int helper(Obj *obj, int idx, int base, int dflt) {
    int i = idx - base;
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return dflt;
}

extern "C" ARM int r0f(Obj *obj, int idx, int dflt) { return helper(obj, idx, 0, dflt); }
extern "C" ARM int r1f(Obj *obj, int idx, int dflt) { return helper(obj, idx, 4, dflt); }

static inline int copy(int x, int base) { return x - base; }
extern "C" ARM int r2f(int x, int d) { int i = copy(x, 0); return i + d; }

template <class T>
static inline int thelper(T *obj, int idx, int base, int dflt) {
    int i = idx - base;
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return dflt;
}

extern "C" ARM int r3f(Obj *obj, int idx, int dflt) { return thelper(obj, idx, 0, dflt); }
