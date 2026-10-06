#include <globaldefs.h>

struct Obj0205eaa0;
void DispatchWithShortB4_0205eaa0(struct Obj0205eaa0* obj, int a, int b);

extern "C" int fix32_Divide(unsigned int, unsigned int);
extern "C" int func_02030c68(int);

extern int data_02108760;

struct Obj02015ae4 {
    char pad_00[0xe];
    unsigned short fieldE;
    unsigned short counter;
};

// USA: func_02015ae4
extern "C" ARM int func_02015ae4(struct Obj02015ae4* obj) {
    unsigned int n = obj->counter;
    int raw1 = 0xbff4;
    if ((float)n <= (float)raw1 / 4096.0f) {
        if (n == 0) {
            DispatchWithShortB4_0205eaa0((struct Obj0205eaa0*)&data_02108760, 0xd, 0);
        } else if (n == 0x10) {
            DispatchWithShortB4_0205eaa0((struct Obj0205eaa0*)&data_02108760, 0xe, 0);
        }
        unsigned int n2 = obj->counter;
        int sq12 = (obj->counter * n2) << 12;
        int m = fix32_Divide(0xffffe386, 0x0008fee0);
        obj->fieldE = (unsigned short)((int)(((long long)m * sq12 + 0x800) >> 12) + 0x647a);
        obj->counter = obj->counter + 1;
        return 0;
    } else {
        int raw2 = 0xbff4;
        int delta = (int)(4096.0f * ((float)n - (float)raw2 / 4096.0f));
        int raw3 = 0x17fe8;
        if ((float)n <= (float)raw3 / 4096.0f) {
            int m1 = fix32_Divide((unsigned int)delta, 0x17fe8);
            int rem = 0x1000 - m1;
            int m2 = fix32_Divide(0xc90f, 0x17fe8);
            int prod = (int)(((long long)m2 * delta + 0x800) >> 12);
            if (prod > 0x647a) {
                prod = 0x647a;
            }
            int s = 0;
            s = s + func_02030c68(prod);
            int corr = (int)(((long long)s * -0x101 + 0x800) >> 12);
            obj->fieldE = (unsigned short)((int)(((long long)rem * corr + 0x800) >> 12) + 0x4800);
            obj->counter = obj->counter + 1;
            return 0;
        } else {
            obj->fieldE = 0x4800;
            return 1;
        }
    }
}
