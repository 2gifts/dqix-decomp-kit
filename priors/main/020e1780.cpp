#include <globaldefs.h>

extern "C" void* func_0203bd08(void);
extern "C" void* func_0203be40(void* obj);
extern "C" void* func_0203be4c(void* obj);
extern "C" int func_020937f0(int a);
extern "C" int _Z26GetDisplayModeCode0209378ci(int a);

struct Pack020e197c {
    unsigned int w;
    unsigned short h;
};
void _Z23PackControlWord020e197cP12Pack020e197ciiiiiiiiiii(struct Pack020e197c* d, int a2, int a3, int a4,
        int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12);

struct Elem020e1780 {
    unsigned int f0;
    unsigned int f4;
    char f8;
    unsigned char f9;
};

struct Entity020e1780 {
    unsigned char pad0[8];
    Elem020e1780* arr;
    unsigned char pad1[0x24 - 0x0c];
    unsigned int field24;
    unsigned char pad2[0x2e - 0x28];
    unsigned char field2e;
    unsigned char field2f;
    unsigned int field30;
    unsigned int field34;
    unsigned char field38;
    unsigned char field39;
    unsigned char field3a;
    unsigned char field3b;
    unsigned char field3c;
    unsigned char field3d;
    unsigned char field3e;
    unsigned char field3f_flag : 1;
};

struct Ctx020e1780 {
    unsigned char pad0[4];
    Entity020e1780* entity;
};

// USA: func_020e1780
extern "C" ARM void func_020e1780(Ctx020e1780* ctx) {
    if (ctx->entity == 0) return;
    if (ctx->entity->field3f_flag == 0 || ctx->entity->arr == 0) return;
    Elem020e1780* arr = ctx->entity->arr;

    void* base = func_0203bd08();
    func_0203be40(base);

    int mode = ctx->entity->field3e;
    unsigned char xbase = ctx->entity->field38;
    unsigned char ybase = ctx->entity->field39;
    func_020937f0(mode);
    int shift = _Z26GetDisplayModeCode0209378ci(mode);
    struct Pack020e197c* pack = (struct Pack020e197c*)func_0203be40(base);
    if (mode != 0) {
        pack = (struct Pack020e197c*)func_0203be4c(base);
    }

    int skip = ctx->entity->field3d;
    int heightBase = ctx->entity->field30;
    int count = ctx->entity->field3c;
    pack += skip;

    int i;
    for (i = 0; i < count; i++) {
        int val = (heightBase + arr[i].f4) >> shift;
        _Z23PackControlWord020e197cP12Pack020e197ciiiiiiiiiii(pack, xbase + arr[i].f8, ybase + arr[i].f9, 0,
                0, 0, 0, arr[i].f0, 0, val, 0xe, 0);
        pack++;
    }

    int j = i;
    Entity020e1780* e = ctx->entity;
    unsigned char rawA = e->field3a;
    unsigned char rawB = e->field3b;
    int boundA = rawA - 4;
    int boundB = rawB - 4;
    int value2 = (heightBase + e->field34) >> shift;

    for (j = i; j < boundB; j += e->field2f) {
        unsigned char stepB = e->field2f;
        if (boundB < j + stepB) {
            j = boundB - stepB;
        }
        int rowY = ybase + j;

        for (i = 0; i < boundA; i += e->field2e) {
            unsigned char stepA = e->field2e;
            if (boundA < i + stepA) {
                i = boundA - stepA;
            }
            _Z23PackControlWord020e197cP12Pack020e197ciiiiiiiiiii(pack, xbase + i + 2, rowY + 2, 0,
                    1, 0, 0, e->field24, 0, value2, 0xe, 0);
            pack++;
        }
    }
}
