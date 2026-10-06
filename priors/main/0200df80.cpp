#include <globaldefs.h>

typedef void (*DtorFunc0200df80)(void*, int);
typedef void (*DeleteFunc0200df80)(void*);
typedef void (*DtorPairFunc0200df80)(void*, void*);

struct CatchInfo0200df80 {
    void* location;
    void* typeinfo;
    DtorFunc0200df80 dtor;
};

struct ThrowContext0200df80 {
    void* throwtype;
    void* location;
    DtorFunc0200df80 dtor;
    char padc[0x18 - 0xc];
    char* FP;
    int reg[16];
};

struct ExceptionInfo0200df80 {
    unsigned int f0;
    void* exception_record;
    unsigned char* action_pointer;
};

extern "C" unsigned int func_0200f1a0(ThrowContext0200df80* context, ExceptionInfo0200df80* info);
extern "C" void* func_0200dad4(unsigned int key, ExceptionInfo0200df80* info);
extern "C" void func_0200efb8(void);
extern "C" void func_0200f1fc(ThrowContext0200df80* context, ExceptionInfo0200df80* info);
extern "C" unsigned char* func_0200d958(unsigned char* ptr, int* outVal);
extern "C" unsigned char* func_0200d9e4(unsigned char* ptr, unsigned int* outVal);

#define ReadU32_0200df80(p) ((p)[0] | ((p)[1] << 8) | ((p)[2] << 16) | ((p)[3] << 24))

// USA: func_0200df80
extern "C" ARM void func_0200df80(ThrowContext0200df80* context, ExceptionInfo0200df80* info, unsigned char* catcher) {
    for (;;) {
        unsigned char* p = info->action_pointer;
        if (p == 0) {
            func_0200dad4(func_0200f1a0(context, info), info);
            if (info->exception_record == 0) {
                func_0200efb8();
            }
            func_0200f1fc(context, info);
            p = info->action_pointer;
            if (p == 0) {
                continue;
            }
        }

        unsigned char action = *p;
        switch (action & 0x1f) {
        case 1: {
            int offset;
            func_0200d958(p + 1, &offset);
            info->action_pointer = info->action_pointer + offset;
            break;
        }
        case 2: {
            int local;
            unsigned char* ptr = func_0200d958(p + 1, &local);
            ((DtorFunc0200df80)ReadU32_0200df80(ptr))(context->FP + local, -1);
            info->action_pointer = ptr + 4;
            break;
        }
        case 3: {
            int local;
            int cond;
            int regcond = *p & 0x40;
            unsigned char* ptr = func_0200d958(p + 1, &cond);
            ptr = func_0200d958(ptr, &local);
            DtorFunc0200df80 dtor = (DtorFunc0200df80)ReadU32_0200df80(ptr);
            unsigned char* next;
            next = ptr + 4;
            if (regcond ? (unsigned char)context->reg[cond] : *(unsigned char*)(context->FP + cond)) {
                dtor(context->FP + local, -1);
            }
            info->action_pointer = next;
            break;
        }
        case 4: {
            int pointer;
            int regpointer = *p & 0x20;
            unsigned char* ptr = func_0200d958(p + 1, &pointer);
            DtorFunc0200df80 dtor = (DtorFunc0200df80)ReadU32_0200df80(ptr);
            unsigned char* next;
            next = ptr + 4;
            dtor(regpointer ? (void*)context->reg[pointer] : *(void**)(context->FP + pointer), -1);
            info->action_pointer = next;
            break;
        }
        case 5: {
            unsigned int element_size;
            unsigned int elements;
            int local;
            unsigned char* ptr = func_0200d958(p + 1, &local);
            ptr = func_0200d9e4(ptr, &elements);
            ptr = func_0200d9e4(ptr, &element_size);
            char* obj = context->FP + local + elements * element_size;
            DtorFunc0200df80 dtor = (DtorFunc0200df80)ReadU32_0200df80(ptr);
            unsigned int n = elements;
            unsigned char* next;
            next = ptr + 4;
            if (n != 0) {
                do {
                    obj -= element_size;
                    dtor(obj, -1);
                } while (--n != 0);
            }
            info->action_pointer = next;
            break;
        }
        case 6: {
            int offset;
            int objectptr;
            int regpointer = *p & 0x20;
            unsigned char* ptr = func_0200d958(p + 1, &objectptr);
            ptr = func_0200d958(ptr, &offset);
            DtorFunc0200df80 dtor = (DtorFunc0200df80)ReadU32_0200df80(ptr);
            unsigned char* next;
            next = ptr + 4;
            dtor((regpointer ? (char*)context->reg[objectptr] : *(char**)(context->FP + objectptr)) + offset, 0);
            info->action_pointer = next;
            break;
        }
        case 7: {
            int offset;
            int objectptr;
            int regpointer = *p & 0x20;
            unsigned char* ptr = func_0200d958(p + 1, &objectptr);
            ptr = func_0200d958(ptr, &offset);
            DtorFunc0200df80 dtor = (DtorFunc0200df80)ReadU32_0200df80(ptr);
            unsigned char* next;
            next = ptr + 4;
            dtor((regpointer ? (char*)context->reg[objectptr] : *(char**)(context->FP + objectptr)) + offset, -1);
            info->action_pointer = next;
            break;
        }
        case 8: {
            int offset;
            int objectptr;
            int cond;
            int regpointer = *p & 0x20;
            unsigned char* ptr = func_0200d958(p + 1, &cond);
            ptr = func_0200d958(ptr, &objectptr);
            ptr = func_0200d958(ptr, &offset);
            DtorFunc0200df80 dtor = (DtorFunc0200df80)ReadU32_0200df80(ptr);
            unsigned char* next;
            next = ptr + 4;
            if ((action & 0x40) ? (short)context->reg[cond] : *(short*)(context->FP + cond)) {
                dtor((regpointer ? (char*)context->reg[objectptr] : *(char**)(context->FP + objectptr)) + offset, -1);
            }
            info->action_pointer = next;
            break;
        }
        case 9: {
            unsigned int element_size;
            unsigned int elements;
            int offset;
            int objectptr;
            int regpointer = *p & 0x20;
            unsigned char* ptr = func_0200d958(p + 1, &objectptr);
            ptr = func_0200d958(ptr, &offset);
            ptr = func_0200d9e4(ptr, &elements);
            ptr = func_0200d9e4(ptr, &element_size);
            char* obj;
            DtorFunc0200df80 dtor = (DtorFunc0200df80)ReadU32_0200df80(ptr);
            unsigned char* next;
            next = ptr + 4;
            if (regpointer) {
                obj = (char*)context->reg[objectptr] + offset;
            } else {
                obj = *(char**)(context->FP + objectptr) + offset;
            }
            unsigned int n = elements;
            obj += n * element_size;
            if (n != 0) {
                do {
                    obj -= element_size;
                    dtor(obj, -1);
                } while (--n != 0);
            }
            info->action_pointer = next;
            break;
        }
        case 10: {
            int objectptr;
            int regpointer = *p & 0x20;
            unsigned char* ptr = func_0200d958(p + 1, &objectptr);
            DeleteFunc0200df80 deletefunc = (DeleteFunc0200df80)ReadU32_0200df80(ptr);
            unsigned char* next;
            next = ptr + 4;
            deletefunc(regpointer ? (void*)context->reg[objectptr] : *(void**)(context->FP + objectptr));
            info->action_pointer = next;
            break;
        }
        case 11: {
            int objectptr;
            int cond;
            int regpointer = *p & 0x20;
            unsigned char* ptr = func_0200d958(p + 1, &cond);
            ptr = func_0200d958(ptr, &objectptr);
            DeleteFunc0200df80 deletefunc = (DeleteFunc0200df80)ReadU32_0200df80(ptr);
            unsigned char* next;
            next = ptr + 4;
            if ((action & 0x40) ? (unsigned char)context->reg[cond] : *(unsigned char*)(context->FP + cond)) {
                deletefunc(regpointer ? (void*)context->reg[objectptr] : *(void**)(context->FP + objectptr));
            }
            info->action_pointer = next;
            break;
        }
        case 12: {
            int cinfo_ref;
            unsigned int catch_pcoffset;
            if (catcher == p) {
                return;
            }
            unsigned char* ptr = func_0200d9e4(p + 5, &catch_pcoffset);
            info->action_pointer = func_0200d958(ptr, &cinfo_ref);
            break;
        }
        case 13: {
            int cinfo_ref;
            unsigned char* ptr = func_0200d958(p + 1, &cinfo_ref);
            CatchInfo0200df80* catchinfo = (CatchInfo0200df80*)(context->FP + cinfo_ref);
            if (catchinfo->dtor) {
                if (context->location == catchinfo->location) {
                    context->dtor = catchinfo->dtor;
                } else {
                    catchinfo->dtor(catchinfo->location, -1);
                }
            }
            info->action_pointer = ptr;
            break;
        }
        case 15: {
            int cinfo_ref;
            unsigned int pcoffset;
            unsigned int specs;
            if (catcher == p) {
                return;
            }
            unsigned char* ptr = func_0200d9e4(p + 1, &specs);
            ptr = func_0200d9e4(ptr, &pcoffset);
            ptr = func_0200d958(ptr, &cinfo_ref);
            info->action_pointer = ptr + specs * 4;
            break;
        }
        case 16: {
            struct {
                int offset;
                char* base;
            } arg;
            int offset;
            int objectptr;
            int regpointer = *p & 0x20;
            unsigned char* ptr = func_0200d958(p + 1, &objectptr);
            ptr = func_0200d958(ptr, &offset);
            unsigned char* q = ptr + 4;
            arg.base = (char*)ReadU32_0200df80(ptr);
            ptr = func_0200d958(q, &arg.offset);
            DtorPairFunc0200df80 dtor = (DtorPairFunc0200df80)ReadU32_0200df80(ptr);
            unsigned char* next;
            next = ptr + 4;
            dtor((regpointer ? (char*)context->reg[objectptr] : *(char**)(context->FP + objectptr)) + offset, arg.base + arg.offset);
            info->action_pointer = next;
            break;
        }
        case 17: {
            int offset2;
            int offset;
            int objectptr2;
            int objectptr;
            int regpointer2;
            int regpointer = *p & 0x20;
            unsigned char* ptr = func_0200d958(p + 1, &objectptr);
            ptr = func_0200d958(ptr, &offset);
            regpointer2 = *ptr++ & 0x20;
            ptr = func_0200d958(ptr, &objectptr2);
            ptr = func_0200d958(ptr, &offset2);
            DtorPairFunc0200df80 dtor = (DtorPairFunc0200df80)ReadU32_0200df80(ptr);
            unsigned char* next;
            next = ptr + 4;
            char* a = regpointer ? (char*)context->reg[objectptr] : *(char**)(context->FP + objectptr);
            char* b = regpointer2 ? (char*)context->reg[objectptr2] : *(char**)(context->FP + objectptr2);
            dtor(a + offset, b + offset2);
            info->action_pointer = next;
            break;
        }
        case 18: {
            unsigned int element_size;
            int sizeptr;
            int objectptr;
            int regpointer = *p & 0x20;
            char* obj;
            DtorFunc0200df80 dtor;
            unsigned char* ptr = func_0200d958(p + 1, &objectptr);
            int regpointer2 = *ptr++ & 0x20;
            ptr = func_0200d958(ptr, &sizeptr);
            ptr = func_0200d9e4(ptr, &element_size);
            dtor = (DtorFunc0200df80)ReadU32_0200df80(ptr);
            unsigned char* next;
            next = ptr + 4;
            if (regpointer) {
                obj = (char*)context->reg[objectptr];
            } else {
                obj = *(char**)(context->FP + objectptr);
            }
            unsigned int size = regpointer2 ? (unsigned int)context->reg[sizeptr] : *(unsigned int*)(context->FP + sizeptr);
            obj += size;
            unsigned int n = size / element_size;
            if (n != 0) {
                do {
                    obj -= element_size;
                    dtor(obj, -1);
                } while (--n != 0);
            }
            info->action_pointer = next;
            break;
        }
        case 19: {
            int local;
            info->action_pointer = func_0200d958(p + 1, &local);
            break;
        }
        default:
            func_0200efb8();
            break;
        }

        if (action & 0x80) {
            info->action_pointer = 0;
        }
    }
}
