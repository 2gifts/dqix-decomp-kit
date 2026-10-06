#include <globaldefs.h>

// USA: func_0200af90
extern "C" ARM unsigned long func_0200af90(unsigned int lx, unsigned int hx) {
    if ((int)hx < 0) {
        if (hx > 0xfff00000u || (hx == 0xfff00000u && lx != 0))
            return 0xffffffffu;
        return 0;
    }

    int shift = 0x41e - ((int)hx >> 20);
    if (shift < 0) return 0xffffffffu;
    if (shift >= 0x20) return 0;

    unsigned int m = (hx << 11) | 0x80000000u | (lx >> 21);
    return m >> shift;
}
