#include <globaldefs.h>
#include "std_library_functions.h"
#include "Memory/SafeAllocator.h"

struct Array02048090 {
    int count;
    void* buffer;
};
void AllocateArray02048090(struct Array02048090* obj, int count, SafeAllocator* alloc);

struct Struct02076928;
void UpdateSelectedCell02076928(struct Struct02076928* s, int val);

int TransferMainObjPalette(int arg0, int arg1, unsigned int arg2);
int TransferSubObjPalette(int arg0, int arg1, unsigned int arg2);

extern "C" void func_02076994(void* ctx, unsigned char* data, unsigned int val, unsigned int w, unsigned int h);

struct Struct02076080 {
    int** f0;                // 0x00
    int** f4;                // 0x04
    int** f8;                // 0x08
    int** fc;                // 0x0c
    unsigned short* f10;     // 0x10
    char pad14[4];           // 0x14
    int* f18;                // 0x18
    void** f1c;              // 0x1c
    void** f20;              // 0x20
    void** f24;              // 0x24
    char pad28[0xc];         // 0x28
    int f34;                 // 0x34
    int f38;                 // 0x38
    int f3c;                 // 0x3c
    char pad40[0x10];        // 0x40
    int f50;                 // 0x50
    struct Array02048090 f54; // 0x54 (count @0x54, buffer @0x58)
    unsigned short f5c;      // 0x5c
    unsigned char f5e;       // 0x5e
    unsigned char f5f;       // 0x5f
};

// SCRATCH-USA: func_02076080
extern "C" ARM int func_02076080(struct Struct02076080* sl, SafeAllocator* sb, unsigned char* data) {
    int masked;
    if (sl->f5e == 0) {
        masked = *(volatile unsigned int*)0x4000000 & 0x300010;
    } else {
        masked = *(volatile unsigned int*)0x4001000 & 0x300010;
    }

    int bpp;
    switch (masked) {
    case 0:        return 1;
    case 0x10:     bpp = 5; break;
    case 0x100010: bpp = 6; break;
    case 0x200010: bpp = 7; break;
    case 0x300010: bpp = 8; break;
    default: return 1;
    }

    int formatMode = 0;
    memcpy(&sl->f5c, data, 2);
    memcpy(&formatMode, data + 2, 2);
    data += 4;

    switch (formatMode) {
    case 3: sl->f50 = 0; break;
    case 4: sl->f50 = 1; break;
    default: return 1;
    }

    sl->f10 = (unsigned short*)sb->Allocate(sl->f5c * 2);
    sl->f4  = (int**)sb->Allocate(sl->f5c * 4);
    sl->f0  = (int**)sb->Allocate(sl->f5c * 4);
    sl->f8  = (int**)sb->Allocate(sl->f5c * 4);
    sl->fc  = (int**)sb->Allocate(sl->f5c * 4);

    unsigned int fp = sl->f38;
    for (unsigned short i = 0; i < sl->f5c; i++) {
        unsigned short skipA, skipB;
        unsigned int cellCount;
        memcpy(&skipA, data, 2);
        memcpy(&skipB, data + 2, 2);
        memcpy(&cellCount, data + 4, 4);
        sl->f10[i] = (unsigned short)cellCount;
        data += 8;

        sl->f4[i] = (int*)sb->Allocate(cellCount * 4);
        sl->f0[i] = (int*)sb->Allocate(cellCount * 4);
        sl->f8[i] = (int*)sb->Allocate(cellCount * 4);
        sl->fc[i] = (int*)sb->Allocate(cellCount * 4);

        int j = 0;
        unsigned int mask = (1u << bpp) - 1;
        unsigned int c10 = 0x80008000;
        unsigned int c0c = 0x40008000;
        unsigned int c08 = 0xc0004000;
        unsigned int c04 = 0x40004000;

        for (; j < cellCount; j++) {
            unsigned short xVal, yVal, wShift, hShift;
            memcpy(&xVal, data, 2);
            memcpy(&yVal, data + 2, 2);
            memcpy(&wShift, data + 4, 2);
            memcpy(&hShift, data + 6, 2);
            unsigned int w = 8u << wShift;
            unsigned int h = 8u << hShift;

            sl->f8[i][j] = xVal;
            data += 8;
            sl->fc[i][j] = yVal;

            int* row4 = sl->f4[i];
            if (w == 8 && h == 8) {
                row4[j] = 0;
            } else if (w == 0x10 && h == 0x10) {
                row4[j] = 0x40000000;
            } else if (w == 0x20 && h == 0x20) {
                row4[j] = 0x80000000;
            } else if (w == 0x40 && h == 0x40) {
                row4[j] = 0xc0000000;
            } else if (w == 0x10 && h == 8) {
                row4[j] = 0x4000;
            } else if (w == 0x20 && h == 8) {
                row4[j] = c04;
            } else if (w == 0x20 && h == 0x10) {
                row4[j] = 0x80004000;
            } else if (w == 0x40 && h == 0x20) {
                row4[j] = c08;
            } else if (w == 8 && h == 0x10) {
                row4[j] = 0x8000;
            } else if (w == 8 && h == 0x20) {
                row4[j] = c0c;
            } else if (w == 0x10 && h == 0x20) {
                row4[j] = c10;
            } else if (w == 0x20 && h == 0x40) {
                row4[j] = 0xc0008000;
            }

            unsigned int size = w * h;
            if (formatMode == 3) {
                size >>= 1;
            }
            sl->f0[i][j] = fp >> bpp;
            func_02076994(sl, data, sl->f0[i][j] << bpp, w, h);
            data += size;
            unsigned int notMask = ~mask;
            fp += (size + mask) & notMask;
        }
    }

    sl->f34 = fp - sl->f38;

    unsigned int paletteSize;
    memcpy(&paletteSize, data, 4);
    data += 4;

    if (paletteSize <= 0x10) {
        int shifted = sl->f3c << 5;
        if (sl->f5e == 0) {
            TransferMainObjPalette((int)data, shifted, 0x20);
        } else {
            TransferSubObjPalette((int)data, shifted, 0x20);
        }
    } else if (paletteSize <= 0x100) {
        int shifted = sl->f3c << 9;
        if (sl->f5e == 0) {
            TransferMainObjPalette((int)data, shifted, 0x200);
        } else {
            TransferSubObjPalette((int)data, shifted, 0x200);
        }
    } else {
        if (sl->f5e == 0) {
            TransferMainObjPalette((int)data, 0, paletteSize);
        } else {
            TransferSubObjPalette((int)data, 0, paletteSize);
        }
    }

    data += paletteSize * 2;
    memcpy(&sl->f54.count, data + 0x1e, 4);
    data += 0x22;
    AllocateArray02048090(&sl->f54, sl->f54.count, sb);

    int count = sl->f54.count;
    if (sl->f54.count > 0) {
        for (int k = 0; k < count; k++) {
            memcpy((char*)sl->f54.buffer + k * 0x1e, data, 0x1e);
            data += 0x1e;
        }

        int byteCount = count * 4;
        sl->f18 = (int*)sb->Allocate(byteCount);
        sl->f1c = (void**)sb->Allocate(byteCount);
        sl->f20 = (void**)sb->Allocate(byteCount);
        sl->f24 = (void**)sb->Allocate(byteCount);

        for (int m = 0; m < count; m++) {
            memcpy(&sl->f18[m], data, 4);
            data += 4;
            int len4 = sl->f18[m] * 4;
            sl->f1c[m] = sb->Allocate(len4);
            sl->f20[m] = sb->Allocate(len4);
            sl->f24[m] = sb->Allocate(len4);
            memcpy(sl->f1c[m], data, len4);
            data += len4;
            memcpy(sl->f20[m], data, len4);
            data += len4;
            memcpy(sl->f24[m], data, len4);
            data += len4;
        }
    }

    sl->f5f = 1;
    UpdateSelectedCell02076928((struct Struct02076928*)sl, 1);
    return 0;
}
