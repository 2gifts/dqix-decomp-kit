#include <globaldefs.h>

void Reset(unsigned char* obj);
int GetVal(void* bs);

extern "C" ARM void probe_a(unsigned char* self, void* bs) {
    if (*(int*)(self + 0x38) != 0) {
        int fieldValue = GetVal(bs);
        unsigned int f38 = *(unsigned int*)(self + 0x38);
        if (f38 < (unsigned int)fieldValue) {
            Reset(self + 0x2c);
        } else {
            *(int*)(self + 0x38) = (int)f38 - fieldValue;
        }
        return;
    }
    *(int*)(self + 0x40) = 1;
}
