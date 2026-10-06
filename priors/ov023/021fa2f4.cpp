#include <globaldefs.h>

struct Pair021fa2f4 { volatile unsigned int a; volatile unsigned int b; };
extern struct Pair021fa2f4 data_020e6d5c;

extern unsigned int data_ov023_021fff28;

struct DispatchEntry021fa2f4 { unsigned int fn; unsigned int locator; };
extern struct DispatchEntry021fa2f4 data_ov023_021fea0c[];

struct Obj021fa2f4 { char pad[0x1c]; int idx; };

// SCRATCH-USA: func_ov023_021fa2f4
extern "C" ARM int func_ov023_021fa2f4(struct Obj021fa2f4* obj) {
    unsigned int flags = data_ov023_021fff28;
    if (!(flags & 1)) {
        unsigned int va = data_020e6d5c.a;
        unsigned int vb = data_020e6d5c.b;
        data_ov023_021fea0c[3].fn = va;
        data_ov023_021fea0c[3].locator = vb;
        data_ov023_021fff28 = flags | 1;
    }
    int idx = obj->idx;
    struct DispatchEntry021fa2f4* d = &((__typeof__(&data_ov023_021fea0c[0]))0x021FEA0C)[idx];
    void* base = (char*)obj + ((int)d->locator >> 1);
    void* callback;
    if (d->locator & 1) {
        callback = *(void**)((char*)*(void**)base + d->fn);
    } else {
        callback = (void*)d->fn;
    }
    int result = ((int(*)(void*))callback)(base);
    obj->idx = result;
    return result;
}
