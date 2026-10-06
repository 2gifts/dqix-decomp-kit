#include <globaldefs.h>

void Reset(unsigned char* obj);
int GetVal(void* bs);
void* GetPtr(void* obj);
int Compute(int obj);

extern "C" ARM void probe_b(unsigned char* self, void* bs) {
    void* a = GetPtr(self);
    void* b = GetPtr(a);
    void* c = GetPtr(b);
    int d = Compute((int)(long)c);
    int e = Compute(d);
    int f = Compute(e);
    int g = Compute(f);
    int h = Compute(g);

    if (*(int*)(self + 0x38) != 0) {
        int fieldValue = GetVal(bs);
        unsigned int f38 = *(unsigned int*)(self + 0x38);
        if (f38 < (unsigned int)fieldValue) {
            Reset(self + 0x2c);
        } else {
            *(int*)(self + 0x38) = (int)f38 - fieldValue + d + e + f + g + h;
        }
        return;
    }
    *(int*)(self + 0x40) = d + e + f + g + h;
}
