#include <globaldefs.h>
#include "std_library_functions.h"

// USA: func_02076738
extern "C" ARM int func_02076738(void* obj, unsigned char* stream) {
    unsigned int notMask;
    unsigned int mask;
    int shift;
    unsigned int total;
    unsigned short outer;
    unsigned int inner;
    int masked;
    if (*((unsigned char*)obj + 0x5e) == 0) {
        masked = (int)(*(volatile unsigned int*)0x4000000 & 0x300010);
    } else {
        masked = (int)(*(volatile unsigned int*)0x4001000 & 0x300010);
    }

    switch (masked) {
    case 0:
        return 0;
    case 0x10:
        shift = 5;
        break;
    case 0x100010:
        shift = 6;
        break;
    case 0x200010:
        shift = 7;
        break;
    case 0x300010:
        shift = 8;
        break;
    default:
        return 0;
    }

    unsigned short outerCount = 0;
    unsigned int format = 0;
    memcpy(&outerCount, stream, 2);
    memcpy(&format, stream + 2, 2);
    stream += 4;

    total = 0;
    outer = 0;
    while (outer < outerCount) {
        unsigned short unused1;
        unsigned short unused2;
        unsigned int innerCount;
        memcpy(&unused1, stream, 2);
        memcpy(&unused2, stream + 2, 2);
        memcpy(&innerCount, stream + 4, 4);
        stream += 8;

        mask = (1u << shift) - 1;
        notMask = ~mask;
        inner = 0;

        while (inner < innerCount) {
            unsigned short f6, f4, hVal, iVal;
            memcpy(&f6, stream, 2);
            memcpy(&f4, stream + 2, 2);
            memcpy(&hVal, stream + 4, 2);
            memcpy(&iVal, stream + 6, 2);

            unsigned int size = (8 << hVal) * (8 << iVal);
            if (format == 3) size = size >> 1;
            stream += 8;
            stream += size;
            total += (size + mask) & notMask;
            inner++;
        }
        outer++;
    }
    return total;
}
