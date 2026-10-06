#include <globaldefs.h>

int ForwardField4To0205765c(int* obj);
struct Obj020577cc;
ARM void SumNestedHalfwordPairs020577cc(struct Obj020577cc* obj, unsigned short* out1, unsigned short* out2);
extern "C" void func_02035a2c(void* p);

// USA: func_ov015_0218ee38
extern "C" ARM void func_ov015_0218ee38(void* obj) {
    *(unsigned short*)((char*)obj + 0x36) = 0;
    *(unsigned short*)((char*)obj + 0x34) = 0;
    if (*(int*)((char*)*(void**)((char*)obj + 0x28) + 0xc) != 0) {
        *(volatile unsigned int*)0x04000444 = 0;
        ForwardField4To0205765c((int*)*(void**)((char*)obj + 0x28));
        *(volatile unsigned int*)0x04000448 = 1;
        void* inner = *(void**)((char*)*(void**)((char*)obj + 0x28) + 4);
        if (inner == 0) {
            *(unsigned short*)((char*)obj + 0x34) = 0;
            *(unsigned short*)((char*)obj + 0x36) = 0;
            return;
        }
        SumNestedHalfwordPairs020577cc((struct Obj020577cc*)inner, (unsigned short*)((char*)obj + 0x34), (unsigned short*)((char*)obj + 0x36));
        return;
    }
    *(volatile unsigned int*)0x04000444 = 0;
    func_02035a2c(*(void**)((char*)obj + 0x24));
    *(volatile unsigned int*)0x04000448 = 1;
    void* p1 = *(void**)((char*)obj + 0x24);
    p1 = *(void**)((char*)p1 + 8);
    p1 = *(void**)((char*)p1 + 0x54);
    *(unsigned short*)((char*)obj + 0x34) = *(unsigned short*)((char*)p1 + 0x28);
    void* p2 = *(void**)((char*)obj + 0x24);
    p2 = *(void**)((char*)p2 + 8);
    p2 = *(void**)((char*)p2 + 0x54);
    *(unsigned short*)((char*)obj + 0x36) = *(unsigned short*)((char*)p2 + 0x2a);
}
