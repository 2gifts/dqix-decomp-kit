#include <globaldefs.h>
#include "Memory/SafeAllocator.h"

struct Variant02030b0c;
int GetIntFromVariant02030b0c(struct Variant02030b0c*);

extern "C" int func_ov017_0218b5b0(void);

struct TaggedValue02030b44;
struct TaggedValue02030b44* ConvertTaggedVec3ToFx32(struct TaggedValue02030b44*, int*);

struct Cont0208e778;
struct Node0208e778;
void AppendNodeToCountedList0208e778(struct Cont0208e778*, struct Node0208e778*);

extern int data_02108fe4;

struct TableEntry0208e0c4 {
    unsigned short a;
    unsigned char b;
    unsigned char c;
};
extern struct TableEntry0208e0c4 data_02108ff4[];

struct Node0208e0c4 {
    struct {
        unsigned int reserved0 : 16;
        unsigned int val1 : 7;
        unsigned int val2 : 2;
        unsigned int pad0 : 7;
    } dw0;
    struct {
        unsigned int mode : 4;
        unsigned int valC : 9;
        unsigned int valD : 4;
        unsigned int fieldB : 4;
        unsigned int fieldC : 8;
        unsigned int pad1 : 3;
    } dw1;
    unsigned int dw2;
    int vec[8][3];
    struct Node0208e778* next;
};

// USA: func_0208e0c4
ARM int func_0208e0c4(char* v) {
    SafeAllocator* alloc = (SafeAllocator*)(func_ov017_0218b5b0() + 0x1a0);
    struct Node0208e0c4* node = (struct Node0208e0c4*)alloc->Allocate(0x70);
    node->next = NULL;

    unsigned short val1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)v);
    node->dw0.val1 = val1;

    node->dw0.reserved0 = (unsigned short)GetIntFromVariant02030b0c((struct Variant02030b0c*)(v + 8));

    node->dw0.val2 = (unsigned char)GetIntFromVariant02030b0c((struct Variant02030b0c*)(v + 0x10));

    node->dw1.mode = (unsigned char)GetIntFromVariant02030b0c((struct Variant02030b0c*)(v + 0x20));

    if (node->dw1.mode == 8) {
        unsigned short a1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(v + 0x28));
        node->dw1.valC = a1;
        node->dw1.valD = (unsigned char)GetIntFromVariant02030b0c((struct Variant02030b0c*)(v + 0x30));
        char* p38 = v + 0x38;
        v += 0x40;
        unsigned char a3 = GetIntFromVariant02030b0c((struct Variant02030b0c*)p38);
        node->dw1.fieldB = a3;
    } else {
        node->dw1.valC = data_02108ff4[node->dw1.mode].a;
        v += 0x40;
        struct TableEntry0208e0c4* e1 = &data_02108ff4[node->dw1.mode];
        node->dw1.valD = e1->b;
        struct TableEntry0208e0c4* e2 = &data_02108ff4[node->dw1.mode];
        node->dw1.fieldB = e2->c;
    }

    for (int i = 0; i < 8; i++) {
        v = (char*)ConvertTaggedVec3ToFx32((struct TaggedValue02030b44*)v, node->vec[i]);
    }

    node->dw2 = 0;
    node->dw1.fieldC = 0;
    AppendNodeToCountedList0208e778((struct Cont0208e778*)&data_02108fe4, (struct Node0208e778*)node);
    return 1;
}
