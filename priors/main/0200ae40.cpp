#include <globaldefs.h>

// ROM SYMBOL: _d2f
// KEEP-NAME
// USA: func_0200ae40
extern "C" ARM int _d2f(unsigned int lo, unsigned int hi)
{
    unsigned int sign = hi & 0x80000000u;
    unsigned int exp = hi >> 20;
    exp &= ~0x800u;

    if (exp == 0) {
        if ((lo | (hi << 12)) != 0) {
            return sign;
        }
        return sign;
    } else {
        unsigned int expsh = exp << 21;
        unsigned int sum = expsh + 0x200000u;
        if (sum < expsh) {
            if ((lo | (hi << 12)) != 0) {
                return 0x7fffffff;
            }
            return sign | 0x7f800000u;
        } else {
            int e = (int)exp - 0x380;
            if (e <= 0) {
                if (e == -0x17) {
                    unsigned int t = lo | (hi << 12);
                    unsigned int r = sign;
                    if (t != 0) {
                        r += 1;
                    }
                    return r;
                } else if (e < -0x17) {
                    return sign;
                } else {
                    unsigned int m = (hi << 11) | 0x80000000u;
                    unsigned int r3 = (m >> 8) | (lo >> 29);
                    int shift = 1 - e;
                    unsigned int extra = lo << 3;
                    unsigned int result = sign | (r3 >> shift);
                    r3 = r3 << (32 - shift);
                    if (extra != 0) {
                        r3 |= 1;
                    }
                    if (r3 == 0) {
                        return result;
                    }
                    if (!(r3 & 0x80000000u)) {
                        return result;
                    }
                    r3 <<= 1;
                    if (r3 == 0) {
                        if (result & 1) {
                            result += 1;
                        }
                        return result;
                    }
                    result += 1;
                    return result;
                }
            } else if (e >= 0xff) {
                return sign | 0x7f800000u;
            } else {
                unsigned int h = hi << 12;
                unsigned int mant = sign | (h >> 9);
                mant |= lo >> 29;
                unsigned int extra = lo << 3;
                unsigned int result = mant | ((unsigned int)e << 23);
                if (extra == 0) {
                    return result;
                }
                if (!(extra & 0x80000000u)) {
                    return result;
                }
                extra <<= 1;
                if (extra == 0) {
                    if (result & 1) {
                        result += 1;
                    }
                    return result;
                }
                result += 1;
                return result;
            }
        }
    }
}
