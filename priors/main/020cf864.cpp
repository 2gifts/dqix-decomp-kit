#include <globaldefs.h>

// USA: func_020cf864
extern "C" ARM unsigned int Trans_020cf864(unsigned int r0, unsigned int r1, unsigned int r2, unsigned int r3) {
    int cc = 0;
    unsigned int r12 = 0;
    r12 = 0x2111824;
L4:;
    r0 = *(unsigned int*)((char*)r12 + 0x0);
    cc = (int)(r0) - (int)(0x1);
    if (cc == 0) { goto L4; }
    return r0;
    return r0;
}
