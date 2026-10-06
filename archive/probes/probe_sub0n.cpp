#include <globaldefs.h>

extern "C" void sink(int);
struct Obj { int a; int b; int value; };

// pointer MINUS POINTER lowers to a real `sub`; with a null-pointer constant on the right the
// immediate is 0, and that subtract is not the front end's `x - 0` fold.
extern "C" ARM int n1(char *p) { return (int)(p - (char *)0); }
extern "C" ARM int n2(char *p, int d) { int i = (int)(p - (char *)0); sink(i); return i + d; }
extern "C" ARM int n3(int *p) { return (int)(p - (int *)0); }
extern "C" ARM int n4(char *p) { return (int)(p - (char *)((void *)0)); }

extern "C" ARM int m1(Obj *obj, char *idx, int dflt) {
    int i = (int)(idx - (char *)0);
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return dflt;
}

// and the same thing spelled as an integer cast of the pointer
extern "C" ARM int m2(Obj *obj, char *idx, int dflt) {
    int i = (int)(long)idx;
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return dflt;
}
