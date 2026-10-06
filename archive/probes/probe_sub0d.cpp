#include <globaldefs.h>

extern "C" void sink(int);
struct Node { int link; int payload; };
struct Obj { int a; int b; int value; };

// `add rN, sp, #0` is NOT folded in the ROM, so a zero DISPLACEMENT on an address survives.
// Does a zero displacement subtracted from a pointer survive too?
extern "C" ARM char *a0(char *p) { return p - 0; }
extern "C" ARM Node *a1(char *p) { return (Node *)(p - 0); }
extern "C" ARM Node *a2(Node *p) { return (Node *)((char *)p - 0); }
extern "C" ARM int a3(char *p, char *q) { return (int)(p - 0 - q); }
extern "C" ARM int a4(int *p) { return p[0] + (int)(long)(p - 0); }

// the `orr Rd, Rs, #0` leak the ROM also carries -- same question for OR
extern "C" ARM int o0(int x) { return x | 0; }
extern "C" ARM int o1(int x, int y) { int i = x | 0; sink(i); return i + y; }

// zero displacement reached through a pointer-to-member style cast chain
extern "C" ARM Obj *b0(Node *n) { return (Obj *)((char *)n - (int)(long)&((Obj *)0)->a); }
extern "C" ARM Obj *b1(Node *n) { return (Obj *)((char *)n - (int)(long)&((Obj *)0)->value); }

// the target shape with the index arriving as a pointer difference
extern "C" ARM int g1(Obj *obj, char *idxp, int dflt) {
    int i = (int)(idxp - 0);
    int flag = 1;
    if (i != -1) {
        if (obj->value != 0) flag = 0;
    }
    if (flag == 0) return obj->value + i;
    return dflt;
}
