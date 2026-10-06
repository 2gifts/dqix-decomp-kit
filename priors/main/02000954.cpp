#include <globaldefs.h>

// USA: func_02000954
extern "C" ARM unsigned int Trans_02000954(unsigned int r0, unsigned int r1, unsigned int r2, unsigned int r3) {
    int cc = 0;
    unsigned int r12 = 0;
    r12 = r1 + r2;
    cc = (int)(r1) - (int)(r12);
    goto L14;
L14:;
    return r0;
    return r0;
}
