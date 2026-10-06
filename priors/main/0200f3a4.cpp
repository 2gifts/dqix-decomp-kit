#include <globaldefs.h>
#include "System/Memory.h"
#include "Util/Random.h"
#include "std_library_functions.h"

extern "C" void func_020c99c8(void* out);
unsigned long long GetCurrentTimestamp(void);

struct S_02010124;
extern "C" void _Z18InitFields02010124P10S_02010124(struct S_02010124* obj);

extern "C" void _Z28InitBigManagerStruct0208660cPc(char* base);
extern "C" void func_02082828(void* field150);

struct Pair0209a338;
extern "C" void _Z26ClearFirstTwoWords0209a338P12Pair0209a338(struct Pair0209a338* p);

void ClearSubstructBytes(void* obj);
void ResetState(void* obj);

extern "C" void* _Z27GetDataPtr02114e04_020d6c00v(void);

struct S02046730;
extern "C" void _Z22ClearField0x0_02046730P9S02046730(struct S02046730* p);

struct State0200fad4;
extern "C" void _Z26ClearFlagsAndValue0200fad4P13State0200fad4(struct State0200fad4* s);

extern float data_020f33b4[];

struct Flags0x569c {
    unsigned int f0_11 : 12;
    unsigned int f12_15 : 4;
    unsigned int f16_20 : 5;
    unsigned int f21_24 : 4;
    unsigned int bit25 : 1;
    unsigned int bit26 : 1;
    unsigned int bit27 : 1;
    unsigned int bit28 : 1;
    unsigned int bit29 : 1;
    unsigned int bit30 : 1;
    unsigned int bit31 : 1;
};

struct Flags0x56a0 {
    unsigned int f0_8 : 9;
    unsigned int f9_18 : 10;
    unsigned int f19_29 : 11;
    unsigned int bit30 : 1;
    unsigned int bit31 : 1;
};

struct LoopWordFlags {
    unsigned int low7 : 7;
    unsigned int upper25 : 25;
};

struct HalfFlags0x12 {
    unsigned short low14 : 14;
    unsigned short bit14 : 1;
    unsigned short bit15 : 1;
};

struct HalfFlags0x78 {
    unsigned short low13 : 13;
    unsigned short high3 : 3;
};

struct ByteFlags0x7ff0 {
    unsigned char bit0 : 1;
    unsigned char bit1 : 1;
    unsigned char bit2 : 1;
    unsigned char bit3 : 1;
    unsigned char bit4 : 1;
    unsigned char rest : 3;
};

// USA: func_0200f3a4
extern "C" ARM void func_0200f3a4(void* obj) {
    char* base = (char*)obj;
    unsigned char buf[0x54];
    unsigned char v;
    int i;

    func_020c99c8(buf);
    v = buf[0];
    *(unsigned char*)(base + 5) = v;
    if (v != 2 && v != 5) *(unsigned char*)(base + 5) = 1;

    memset(base + 8, 0, 0x3a4);

    _Z28InitBigManagerStruct0208660cPc((char*)base + 0x204 + 0x2800);

    *(int*)(base + 0x3b0) = 0;
    *(int*)(base + 0x3ac) = 0;
    for (i = 0; i < 4; i++) {
        func_02082828(base + 0x74 + 0x400 + i * 0x964);
    }

    VectorizedMemset(base + 0x3f8, 0, 0x70);
    {
        int one = 1;
        int negOne;
        *(unsigned char*)(base + 0x3fc) = one;
        *(unsigned char*)(base + 0x400) = one;
        *(unsigned char*)(base + 0x401) = one;
        negOne = one - 2;
        *(unsigned char*)(base + 0x403) = negOne;
        *(int*)(base + 0x418) = negOne;
        *(int*)(base + 0x41c) = negOne;
        *(int*)(base + 0x420) = negOne;
        *(int*)(base + 0x424) = negOne;
        *(short*)(base + 0x400 + 0x16) = -1;
        *(unsigned char*)(base + 0x404) = 0;
        *(short*)(base + 0x400 + 0x64) = -1;
        *(int*)(base + 0x5000 + 0xca4) = 0;
        *(int*)(base + 0x5000 + 0xca8) = 0;

        *(float*)(base + 0x3d0) = data_020f33b4[1];
        *(float*)(base + 0x3d4) = 1.0f / 60.0f;
        *(float*)(base + 0x3cc) = data_020f33b4[3];
        *(int*)(base + 0x3dc) = 2;
        *(int*)(base + 0x3d8) = one;
        *(int*)(base + 0x3e0) = 0;
        memset(base + 0x3e8, 0, 8);
        memset(base + 0x3f0, 0, 8);

        *(int*)(base + 0x5000 + 0xcb0) = one;
        *(int*)(base + 0x5000 + 0xcb4) = one;
        *(int*)(base + 0x5000 + 0xcb8) = one;
        *(int*)(base + 0x5000 + 0xcbc) = one;
    }
    *(unsigned char*)(base + 0x5000 + 0x728) = 0;
    *(unsigned char*)(base + 0x5000 + 0x729) = 0;
    memset(base + 0x1cc0 + 0x4000, 0, 8);

    *(unsigned char*)(base + 0x6000 + 0x3d4) = *(int*)(base + 0x7000 + 0xf5c) = 0;
    *(unsigned char*)(base + 0x5000 + 0xcda) = 8;
    memset(base + 0xdc + 0x5c00, 0, 0x190);

    {
        unsigned long long timestamp = GetCurrentTimestamp();
        srand((unsigned int)(timestamp & 0xFFFFFFFFULL));
        SeedRandom64(GetBTRandom(), timestamp);
    }

    *(int*)(base + 0x7000 + 0xf60) = 0;
    *(int*)(base + 0x7000 + 0xf64) = 0;
    *(int*)(base + 0x7000 + 0xf68) = 0;
    ClearSubstructBytes(base);

    {
        struct Flags0x569c* f = (struct Flags0x569c*)(base + 0x5000 + 0x69c);
        f->f0_11 = 0x7d0;
        f->f12_15 = 1;
        f->f16_20 = 1;
        f->bit28 = 0;
        f->f21_24 = 0;
        f->bit27 = 0;
        f->bit25 = 0;
        f->bit26 = 0;
        f->bit29 = 0;
        f->bit30 = 1;
        f->bit31 = 0;
    }
    {
        struct Flags0x56a0* f = (struct Flags0x56a0*)(base + 0x5000 + 0x6a0);
        f->f0_8 = 0x1ff;
        f->bit30 = 0;
        f->f9_18 = 300;
    }
    {
        volatile struct Flags0x56a0* f = (volatile struct Flags0x56a0*)(base + 0x6a0 + 0x5000);
        f->f19_29 = 706;
    }
    *(unsigned char*)(base + 0x5000 + 0x6a4) = 0;

    _Z26ClearFirstTwoWords0209a338P12Pair0209a338((struct Pair0209a338*)(base + 0x32c + 0x5400));
    memset(base + 0x334 + 0x5400, 0, 0x570);

    *(short*)(base + 0x5e00 + 0x6c) = 0;
    for (i = 0; i < 6; i++) {
        *(short*)(base + i * 0x28 + 0x5e00 + 0x70) = -1;
        {
            struct LoopWordFlags* w = (struct LoopWordFlags*)(base + 0x274 + 0x5c00 + i * 0x28);
            w->low7 = 0;
            w->upper25 = 0;
        }
        memset(base + 0x278 + 0x5c00 + i * 0x28, 0, 0xc);
        memset(base + 0x284 + 0x5c00 + i * 0x28, 0, 0x12);
    }

    memset(base + 0xf60 + 0x5000, 0, 4);

    *(short*)(base + 0x5f00 + 0x64) = -1;
    *(short*)(base + 0x5f00 + 0x66) = 0;
    *(short*)(base + 0x5f00 + 0x68) = 0;
    memset(base + 0x36c + 0x5c00, 0, 4);

    memset(base + 0xf70 + 0x5000, 0, 4);
    memset(base + 0x374 + 0x5c00, 0, 4);

    {
        struct HalfFlags0x78* f = (struct HalfFlags0x78*)(base + 0x5f00 + 0x78);
        f->low13 = 0;
        f->high3 = 0;
    }
    *(int*)(base + 0x5000 + 0xf7c) = 0;

    memset(base + 0x1f80 + 0x4000, 0, 0x200);
    memset(base + 0x2180 + 0x4000, 0, 0x200);
    memset(base + 0x2380 + 0x4000, 0, 0x54);
    memset(base + 0x318 + 0x5400, 0, 4);

    *(unsigned char*)(base + 0x5000 + 0x71c) = 1;

    memset(base + 0x1d + 0x5700, 0, 4);

    *(unsigned char*)(base + 0x5000 + 0x721) = 1;

    {
        unsigned int* p1 = (unsigned int*)(base + 0x5000 + 0xccc);
        unsigned int* p2 = (unsigned int*)(base + 0xcc + 0x5c00);
        *p1 = *p1 & ~1u;
        *p2 = *p2 | 2u;
    }

    _Z18InitFields02010124P10S_02010124((struct S_02010124*)base);

    *(unsigned char*)(base + 0x6000 + 0x3d6) = 0;
    *(short*)(base + 0x6300 + 0xd8) = 0;
    *(short*)(base + 0x6300 + 0xda) = 0;
    *(int*)(base + 0x6000 + 0x3e0) = 0;
    *(unsigned char*)(base + 0x6000 + 0x3e4) = 0;
    *(unsigned char*)(base + 0x6000 + 0x3e5) = 0;
    *(unsigned char*)(base + 0x6000 + 0x3e6) = 1;
    *(unsigned char*)(base + 0x6000 + 0x3e7) = 0;
    *(unsigned char*)(base + 0x6000 + 0x3e8) = 0;
    *(unsigned char*)(base + 0x6000 + 0x3ed) = 0;
    ResetState(base);

    *(unsigned char*)(base + 0x6000 + 0x480) = 0;
    VectorizedMemset(base + 0x9e + 0x6400, 0, 0x40);

    *(unsigned char*)(base + 0x6000 + 0x4de) = 0;
    *(int*)(base + 0x6000 + 0x474) = -1;
    *(unsigned char*)(base + 0x6000 + 0x3dc) = 0;
    *(short*)(base + 0x7100 + 0xdc) = 0;
    *(short*)(base + 0x7f00 + 0x58) = 0;
    *(short*)(base + 0x7100 + 0xde) = -1;
    *(short*)(base + 0x7100 + 0xe0) = -1;
    *(int*)(base + 0x5000 + 0x724) = 0;

    {
        int f6c = *(int*)(base + 0x7000 + 0xf6c);
        if (f6c < 1) {
            *(int*)(base + 0x7000 + 0xf6c) = 0;
            *(unsigned char*)(base + 0x7000 + 0xf72) = 0;
            *(unsigned char*)(base + 0x7000 + 0xf73) = 0;
        }
        *(unsigned char*)(base + 0x7000 + 0xf70) = 0;
        *(unsigned char*)(base + 0x6000 + 0x3d5) = 1;
        if (*(unsigned char*)data_020f33b4 == 0) {
            *(unsigned char*)(base + 0x7000 + 0xf71) = 0;
            *(unsigned char*)data_020f33b4 = 1;
        }
    }

    *(int*)(base + 0x7000 + 0x1f8) = 0;
    *(unsigned char*)(base + 0x7000 + 0x1fc) = 0;

    {
        char* elemBase = base + 0x7200;
        for (i = 0; i < 16; i++) {
            char* sb = elemBase + i * 0x2c;
            VectorizedMemset(sb, 0, 6);
            memset(sb + 6, 0, 0xb);
            {
                struct HalfFlags0x12* hw = (struct HalfFlags0x12*)(sb + 0x12);
                hw->low14 = 0;
                *(unsigned char*)(base + i * 0x2c + 0x7000 + 0x211) = 0;
                hw->bit14 = 0;
            }
            VectorizedMemset(sb + 0x14, 0, 0x18);
        }
    }

    memset(base + 0xde + 0x7400, 0, 0x20);
    memset(base + 0xfe + 0x7400, 0, 6);
    memset(base + 0x1e4 + 0x7000, 0, 0xc);

    *(int*)(base + 0x7000 + 0x1f0) = 0;
    *(int*)(base + 0x7000 + 0x1f4) = 0;

    _Z22ClearField0x0_02046730P9S02046730((struct S02046730*)_Z27GetDataPtr02114e04_020d6c00v());

    memset(base + 0x104 + 0x7400, 0, 0xec);
    memset(base + 0x5f0 + 0x7000, 0, 0x4d0);
    memset(base + 0x3ac0 + 0x4000, 0, 0x3b4);

    *(unsigned char*)(base + 0x7000 + 0xe7e) = 0xff;
    *(short*)(base + 0x7e00 + 0x78) = 0;
    *(short*)(base + 0x7e00 + 0x7a) = 0;
    *(short*)(base + 0x7e00 + 0x7c) = 0;
    *(int*)(base + 0x7000 + 0xe74) = 0x452;
    *(unsigned char*)(base + 0x5000 + 0xcac) = 0;
    *(int*)(base + 0x3c8) = 0;

    {
        char* elemBaseA = base + 0x38c + 0x7c00;
        char* elemBaseB = base + 0x34c0 + 0x4000;
        for (i = 0; i < 3; i++) {
            _Z26ClearFlagsAndValue0200fad4P13State0200fad4((struct State0200fad4*)(elemBaseA + i * 0xc));
            VectorizedMemset(elemBaseB + i * 0xa, 0, 6);
            {
                char* p = base + i * 0xa + 0x7000;
                *(unsigned char*)(p + 0x4c6) = -1;
                *(unsigned char*)(p + 0x4c7) = 0;
                *(unsigned char*)(p + 0x4c8) = 0;
                *(unsigned char*)(p + 0x4c9) = -1;
            }
        }
    }

    memset(base + 0xfb0 + 0x7000, 0, 0x40);

    {
        struct ByteFlags0x7ff0* f = (struct ByteFlags0x7ff0*)(base + 0x7000 + 0xff0);
        f->bit0 = 0;
        f->bit1 = 0;
        f->bit2 = 0;
        f->bit3 = 0;
        f->bit4 = 0;
    }
    *(unsigned char*)(base + 0x7000 + 0xff1) = 0;
    *(unsigned char*)(base + 0x7000 + 0xff2) = 0;
}
