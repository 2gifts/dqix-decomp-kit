#include <globaldefs.h>
#include "std_library_functions.h"

struct Actor020db844;
extern "C" void _Z26RunFlaggedCallback020db844P13Actor020db844iii(struct Actor020db844* a, int b, int c, int d);

extern "C" void* _Z27GetDataPtr02114e04_020d6c00v(void);

extern "C" void _Z16OrBitsIntoField0Pjj(unsigned int* word, unsigned int mask);

extern "C" int func_020937f0(int a);

struct Entity020db3a8;
extern "C" extern int _Z24CheckActiveState020db3a8P14Entity020db3a8(struct Entity020db3a8* s);

void CleanInvalidateCacheRange(const void* addr, unsigned int size);
void CleanCacheRange(const void* addr, unsigned int size);

extern "C" void _Z34ConfigureAndDispatchHandle020db304ii(int param0, int param1);

struct Fields020dde64;
extern "C" void _Z19ClearFields020dde64P14Fields020dde64(struct Fields020dde64* obj);

struct Struct_020de0ec;
extern "C" void _Z17RollFlags020de0ecP15Struct_020de0ec(struct Struct_020de0ec* obj);

typedef void* (*FnPtrVoid)(void);
extern FnPtrVoid data_020f28e4[];

typedef void (*Fn3)(void*, int, unsigned int);
extern Fn3 data_020f28ec[];
extern Fn3 data_020f28dc[];

typedef int (*FnIntVoid)(void);
extern FnIntVoid data_020f28fc[];

struct Obj020db3f4 {
    unsigned char pad00[0x3c];
    unsigned char field3c;
    unsigned char pad3d[0x40 - 0x3d];
    void* field40;
    unsigned int field44;
    void* field48;
    unsigned int field4c;
    unsigned char pad50[0x51 - 0x50];
    unsigned char field51;
    unsigned char pad52[0x53 - 0x52];
    unsigned char field53;
    unsigned char pad54[0x58 - 0x54];
    unsigned int field58;
    unsigned char pad5c[0x64 - 0x5c];
    unsigned char field64;
    unsigned char pad65[0x6a - 0x65];
    short field6a;
    unsigned char pad6c[0x6e - 0x6c];
    unsigned short field6e;
    unsigned char slots[3][0x28];
};

// USA: func_020db3f4
extern "C" ARM void func_020db3f4(struct Obj020db3f4* obj) {
    if (obj->field51 != 0) {
        return;
    }

    _Z26RunFlaggedCallback020db844P13Actor020db844iii((struct Actor020db844*)obj, obj->field3c, obj->field6a, 0);
    obj->field51 = 1;

    _Z16OrBitsIntoField0Pjj((unsigned int*)_Z27GetDataPtr02114e04_020d6c00v(), 0x8000);

    int x = func_020937f0(obj->field3c);
    int off_b80 = x - 0xb80;
    int mode = 0x1e0;
    int off_800 = x - 0x800;

    if (_Z24CheckActiveState020db3a8P14Entity020db3a8((struct Entity020db3a8*)obj) != 0) {
        mode = 0x1c0;
    }

    void* base = data_020f28e4[obj->field3c]();

    CleanInvalidateCacheRange(obj->field40, obj->field44);
    data_020f28ec[obj->field3c](obj->field40, off_b80, obj->field44);
    CleanCacheRange(obj->field40, obj->field44);

    char* addr = (char*)base + off_800;
    if (obj->field53 == 0 || obj->field6a == 0) {
        memset(addr, 0, 0x800);
    } else if (obj->field6a == -16) {
        memset(addr, obj->field6a + 15, 0x800);
    } else if (obj->field6a == 0x10) {
        memset(addr, 0xeeeeeeee, 0x800);
    }

    CleanInvalidateCacheRange(addr, 0x800);
    data_020f28ec[obj->field3c](addr, off_800, 0x800);
    CleanCacheRange(addr, 0x800);

    CleanInvalidateCacheRange(obj->field48, obj->field4c);
    data_020f28dc[obj->field3c](obj->field48, mode, obj->field4c);
    CleanCacheRange(obj->field48, obj->field4c);

    _Z34ConfigureAndDispatchHandle020db304ii((int)obj, obj->field3c);

    obj->field6e = 0;
    for (int i = 0; i < 3; i++) {
        unsigned char* arr = obj->slots[0];
        unsigned char* p = arr + i * 0x28;
        _Z19ClearFields020dde64P14Fields020dde64((struct Fields020dde64*)p);
        _Z17RollFlags020de0ecP15Struct_020de0ec((struct Struct_020de0ec*)p);
    }

    if (obj->field53 != 0) {
        obj->field58 = 0x10;
        return;
    }

    int result = data_020f28fc[obj->field3c]();
    obj->field64 = result & 0x10;
    obj->field58 = result | 0x10;
}
