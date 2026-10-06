#include <globaldefs.h>

extern "C" void sink(int);
struct Obj { int a; int b; int value; };

// y2 gave `subs Rd, Rs, #0`. Does the S bit drop when the value is used further on?
extern "C" ARM int z0(int x, int n) {
    int i = (int)((long long)x - ((long long)n << 32));
    sink(i);
    return i + x;
}

extern "C" ARM int z1(Obj *obj, int idx, int hi) {
    int i = (int)((long long)idx - ((long long)hi << 32));
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return 0;
}

// same idea with the 64-bit value coming from a struct field rather than a shift
struct Big { int lo; int hi; };
extern "C" ARM int z2(Obj *obj, int idx, Big *b) {
    long long k = ((long long)b->hi << 32);
    int i = (int)((long long)idx - k);
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return 0;
}
