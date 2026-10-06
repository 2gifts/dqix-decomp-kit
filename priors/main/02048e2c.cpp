#include <globaldefs.h>

struct Inner02048e2c {
    char pad00[0x10];
    int vec10[3];
    char pad1c[0x4];
    unsigned int flags20;
    unsigned char arr[0x10];
    unsigned char count34;
    char pad35[0x3];
    int i38;
    unsigned short h3c;
    char pad3e[0x2];
    int vec40[3];
};

struct Obj02048e2c {
    char pad00[0x13c];
    Inner02048e2c* ptr;
};

struct Pos02048e2c { int x, z; };

extern "C" Pos02048e2c func_ov000_0216f74c(int* index);

// USA: func_02048e2c
ARM void func_02048e2c(Obj02048e2c* obj) {
    if (obj->ptr == 0) {
        return;
    }

    {
        Inner02048e2c* p = obj->ptr;
        unsigned char prev = p->arr[0];
        int r;
        int w = 1;
        r = w;
        while (r < obj->ptr->count34) {
            unsigned char cur = p->arr[r];
            if (prev != cur) {
                ((Obj02048e2c volatile*)obj)->ptr->arr[w] = cur;
                p = obj->ptr;
                w++;
                prev = p->arr[r];
            }
            r++;
        }
        p->count34 = w;
    }

    if (obj->ptr->count34 <= 1) {
        obj->ptr->count34 = 0;
        return;
    }

    int idx;
    Pos02048e2c pos;
    for (;;) {
        idx = obj->ptr->arr[1];
        pos = func_ov000_0216f74c(&idx);
        int x = pos.x;
        int z = pos.z;
        volatile int savedX = x;
        volatile int savedZ = z;
        if (obj->ptr->vec40[0] == x && obj->ptr->vec40[2] == z) {
            obj->ptr->count34--;
            if (obj->ptr->count34 <= 1) {
                obj->ptr->count34 = 0;
                return;
            }
            for (int i = 1; i < obj->ptr->count34; i++) {
                obj->ptr->arr[i] = obj->ptr->arr[i + 1];
            }
        } else {
            obj->ptr->flags20 |= 0x10;
            return;
        }
    }
}
