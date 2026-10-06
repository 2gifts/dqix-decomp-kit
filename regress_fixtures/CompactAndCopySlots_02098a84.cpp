#include <globaldefs.h>

struct ValueList02098e3c;
extern int Field0x1b68ContainsValue(struct ValueList02098e3c* s, int val);

struct S02098834 {
    unsigned char pad0[0xc];
    unsigned int flagsLow : 4;
    unsigned int category : 4;
    unsigned int flagsHigh : 24;
    unsigned int used : 2;
    unsigned int valueId : 30;
    unsigned char pad1[0xe8 - 0x14];
};

extern "C" extern struct S02098834* _Z18CopyStruct02098834P9S02098834S0_(struct S02098834* dst, struct S02098834* src);
extern "C" void func_02098364(void* entry);

struct Ctx02098a84 {
    unsigned char pad0[0xb38];
    int b38;
    unsigned char pad1[0xb74 - 0xb38 - 4];
    int b74;
};

// USA: func_02098a84
#pragma opt_common_subs off
#pragma opt_propagation off
extern "C" ARM int func_02098a84(struct S02098834* arr) {
    if (((struct Ctx02098a84*)((unsigned char*)arr + 0x1000))->b38 == 0) {
        return 0;
    }

    int recycle = 0;
    int last = recycle;
    int freeIdx = recycle - 1;
    int allValid = 1;
    for (int idx = recycle; idx < 0x1e; idx++) {
        struct S02098834* e = &arr[idx];
        if (e->used == 0) {
            freeIdx = idx;
            break;
        }
        if (e->category == 9 || e->category == 10) {
            continue;
        }
        if (Field0x1b68ContainsValue((struct ValueList02098e3c*)arr, e->valueId) == 0) {
            allValid = 0;
        }
    }

    if (freeIdx == 0) {
        return 0;
    }

    if (freeIdx != -1) {
        if (allValid) {
            if (freeIdx < 6) {
                return freeIdx;
            }
            _Z18CopyStruct02098834P9S02098834S0_(&arr[freeIdx], &arr[((volatile struct Ctx02098a84*)((unsigned char*)arr + 0x1000))->b74]);
            func_02098364(&arr[((volatile struct Ctx02098a84*)((unsigned char*)arr + 0x1000))->b74]);
            return ((volatile struct Ctx02098a84*)((unsigned char*)arr + 0x1000))->b74;
        }
        recycle = freeIdx;
    } else {
        for (int idx = 0x1d; idx >= 0; idx--) {
            struct S02098834* e = &arr[idx];
            if (e->category == 9 || e->category == 10) {
                continue;
            }
            if (Field0x1b68ContainsValue((struct ValueList02098e3c*)arr, e->valueId) == 0) {
                recycle = idx;
                break;
            }
        }
    }

    for (int idx = recycle - 1; idx >= 0; idx--) {
        struct S02098834* e = &arr[idx];
        if (e->category == 9 || e->category == 10) {
            continue;
        }
        if (Field0x1b68ContainsValue((struct ValueList02098e3c*)arr, e->valueId) == 0) {
            last = idx;
            _Z18CopyStruct02098834P9S02098834S0_(&arr[recycle], e);
            recycle = idx;
        }
    }
    func_02098364(&arr[last]);

    struct S02098834* e;
    for (recycle = 0; recycle < 6; recycle++) {
        if (arr[recycle].category == 9 || arr[recycle].category == 10) {
            continue;
        }
        e = &arr[recycle];
        if (Field0x1b68ContainsValue((struct ValueList02098e3c*)arr, e->valueId) == 0) {
            e->category = 2;
        }
    }
    for (recycle = 6; recycle < 12; recycle++) {
        if (arr[recycle].category == 9 || arr[recycle].category == 10) {
            continue;
        }
        e = &arr[recycle];
        if (Field0x1b68ContainsValue((struct ValueList02098e3c*)arr, e->valueId) == 0) {
            e->category = 3;
        }
    }
    for (recycle = 12; recycle < 18; recycle++) {
        if (arr[recycle].category == 9 || arr[recycle].category == 10) {
            continue;
        }
        e = &arr[recycle];
        if (Field0x1b68ContainsValue((struct ValueList02098e3c*)arr, e->valueId) == 0) {
            e->category = 4;
        }
    }
    for (recycle = 18; recycle < 24; recycle++) {
        if (arr[recycle].category == 9 || arr[recycle].category == 10) {
            continue;
        }
        e = &arr[recycle];
        if (Field0x1b68ContainsValue((struct ValueList02098e3c*)arr, e->valueId) == 0) {
            e->category = 5;
        }
    }
    for (recycle = 24; recycle < 0x1e; recycle++) {
        if (arr[recycle].category == 9 || arr[recycle].category == 10) {
            continue;
        }
        e = &arr[recycle];
        if (Field0x1b68ContainsValue((struct ValueList02098e3c*)arr, e->valueId) == 0) {
            e->category = 6;
        }
    }

    if (last < 6) {
        return last;
    }
    _Z18CopyStruct02098834P9S02098834S0_(&arr[last], &arr[((volatile struct Ctx02098a84*)((unsigned char*)arr + 0x1000))->b74]);
    func_02098364(&arr[((volatile struct Ctx02098a84*)((unsigned char*)arr + 0x1000))->b74]);
    return ((volatile struct Ctx02098a84*)((unsigned char*)arr + 0x1000))->b74;
}
#pragma opt_propagation reset
#pragma opt_common_subs reset
