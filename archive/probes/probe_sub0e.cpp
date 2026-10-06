#include <globaldefs.h>

extern "C" void sink(int);
struct Obj { int a; int b; int value; };

// A LITERAL identity folds in the front end. A PROPAGATED zero reaches the backend as a real
// sub node with an immediate 0 -- which is where `sub Rd, Rs, #0` should come from.
extern "C" ARM int c0(int x) { int base = 0; int i = x - base; sink(i); return i; }
extern "C" ARM int c1(int x, int y) { int base = 0; int i = x - base; return i + y; }
extern "C" ARM int c2(int x) { int m = 0; int i = x | m; sink(i); return i; }

extern "C" ARM int h0(Obj *obj, int idx, int dflt) {
    int base = 0;
    int i = idx - base;
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return dflt;
}

extern "C" ARM int h1(Obj *obj, int idx, int dflt) {
    int base = 0;
    int flag = 1;
    if (idx - base != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + (idx - base);
    return dflt;
}
