// What decides mwcc's stack layout? Each local here has a DISTINCT size and its address escapes, so
// none can be eliminated or coalesced, and the sp offset of each `add rN, sp, #imm` names it.
// Read the offsets against the declaration order and the case order to see which one the compiler
// follows.
#include <globaldefs.h>

extern "C" void Take(void *p);

struct S4  { int a; };
struct S8  { int a, b; };
struct S12 { int a, b, c; };
struct S16 { int a, b, c, d; };
struct S20 { int a, b, c, d, e; };
struct S24 { int a, b, c, d, e, f; };

extern "C" ARM int ProbeLayout(int k) {
    char fnBig[0xb8];       // function scope, declared 1st -- a big array like 0xcd's
    struct S4 fnA;          // function scope, declared 2nd
    struct S8 fnB;          // function scope, declared 2nd
    switch (k) {
    case 0: {
        struct S12 caseA;   // earliest clause
        Take(&caseA);
        break;
    }
    case 1: {
        struct S16 caseB;
        Take(&caseB);
        break;
    }
    case 2: {
        struct S20 caseC;
        Take(&caseC);
        break;
    }
    case 3: {
        struct S24 caseD;   // latest clause
        Take(&caseD);
        break;
    }
    }
    Take(&fnBig);
    Take(&fnA);
    Take(&fnB);
    return 0;
}
