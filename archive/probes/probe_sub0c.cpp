#include <globaldefs.h>

extern "C" void sink(int);

// Does the range-check `sub` survive with lo == 0 when the switched value is still live after?
extern "C" ARM int t0(int x) {
    int r = 0;
    switch (x) {
    case 0: r = 11; break;
    case 1: r = 22; break;
    case 2: r = 33; break;
    case 3: r = 44; break;
    case 4: r = 55; break;
    case 5: r = 66; break;
    }
    return r + x;
}

extern "C" ARM int t5(int x) {
    int r = 0;
    switch (x) {
    case 5: r = 11; break;
    case 6: r = 22; break;
    case 7: r = 33; break;
    case 8: r = 44; break;
    case 9: r = 55; break;
    case 10: r = 66; break;
    }
    return r + x;
}

// a plain `x - 0` whose result must occupy a register distinct from x
extern "C" ARM int n0(int x) { int i = x - 0; sink(i); return i + x; }
extern "C" ARM int n1(int x) { int i = x - 0; int j = i + 1; sink(j); return i + x; }

// if-chain equality tests on a value that stays live
extern "C" ARM int e0(int x, int d) {
    int r = d;
    if (x == 0) r = 11;
    else if (x == 1) r = 22;
    else if (x == 2) r = 33;
    return r + x;
}

// the target's own shape, but with the compare against 0 instead of -1
extern "C" ARM int g0(int *obj, int idx, int dflt) {
    int flag = 1;
    if (idx != 0) {
        if (obj[2] != 0) flag = 0;
    }
    if (flag == 0) return obj[2] + idx;
    return dflt;
}
