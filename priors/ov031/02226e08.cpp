#include <globaldefs.h>
#include "System/Memory.h"

extern "C" void* func_ov031_0223cf4c(unsigned int len, int align);
extern "C" ARM void func_020ca390(unsigned short value, unsigned short* dst, unsigned int size);
extern "C" void CopyFrom027ffcf4(void* dst);
extern "C" int func_ov031_0221e7d0(void* a, void* b);
extern "C" ARM void func_020c9be0(void);

extern "C" int _Z23TailCallWith32_02226fc8i(int a);
extern "C" int func_ov031_02226fd8(void* a);

struct AllocState_02250c04 {
    unsigned char inUse;
    unsigned char pad[3];
    void* buf;
};
extern AllocState_02250c04 data_ov031_02250c04;
extern const unsigned char data_ov031_02248d80[0xc];

struct LocalPacket_02226e08 {
    unsigned char cmd;
    unsigned char pad;
    unsigned short len;
    unsigned char body[0x100];
};

struct DestPacket_02226e08 {
    unsigned short tag;
    unsigned short body[0x82];
    unsigned short f106;
    unsigned short f108;
    unsigned short f10a;
    unsigned short f10c;
    unsigned short f10e;
    unsigned char f110[6];
};

// USA: func_ov031_02226e08
extern "C" ARM void func_ov031_02226e08(void) {
    LocalPacket_02226e08 local;

    void* p = func_ov031_0223cf4c(0x26c, 4);
    data_ov031_02250c04.buf = p;
    data_ov031_02250c04.inUse = 0;

    func_020ca390(0, (unsigned short*)&local, sizeof(local));

    local.cmd = 0x50;
    local.len = 0xc;
    VectorizedInvertedMemcpy(data_ov031_02248d80, local.body, 0xc);

#define DST ((DestPacket_02226e08*)data_ov031_02250c04.buf)
    DST->tag = 3;

    unsigned short* s = (unsigned short*)&local;
    unsigned short* d = DST->body;
    int n = 0x41;
    do {
        unsigned short a = s[0];
        unsigned short b = s[1];
        s += 2;
        d[0] = a;
        d[1] = b;
        d += 2;
    } while (--n);

    DST->f106 = 1;
    DST->f108 = (unsigned short)-1;
    DST->f10a = 1;
    DST->f10c = (unsigned short)-1;
    DST->f10e = (unsigned short)-1;

    CopyFrom027ffcf4(DST->f110);
#undef DST

    if (func_ov031_0221e7d0((void*)_Z23TailCallWith32_02226fc8i, (void*)func_ov031_02226fd8) != 0) {
        func_020c9be0();
    }
}
