#include <globaldefs.h>

// KEEP-NAME: the ROM symbol is a curated name, not a func_ tag.
// SCRATCH-USA: func_0200c5fc
extern "C" ARM int _ffix(unsigned int bits) {
    int shift = 0x9e;
    unsigned int mag = bits & 0x7fffffffu;
    if ((shift -= mag >> 23) > 0) {
        unsigned int frac = (mag << 8) | 0x80000000u;
        int result = 0;
        result = result + (int)(frac >> shift);
        if ((int)bits < 0) {
            result = -result;
        }
        return result;
    }
    return ~((int)bits >> 31) + (int)0x80000000;
}
