#include <globaldefs.h>

struct Variant02030b0c { int tag; union { int i; float f; } u; };
extern int GetIntFromVariant02030b0c(struct Variant02030b0c* p);

struct TaggedValue02030b44 { int type; union { int i; float f; } value; };
extern struct TaggedValue02030b44* ConvertTaggedVec3ToFx32(struct TaggedValue02030b44* obj, int* outVec);
extern float GetTaggedValueAsFloat(struct TaggedValue02030b44* v);

extern void CopyVec3(int* dst, int* src);
extern void ResetStruct0201d424(char* obj);
extern "C" int func_02030f30(int value);

struct Vec3 { int x; int y; int z; };
extern void SubtractVec3(struct Vec3* a, struct Vec3* b, struct Vec3* out);
ARM int Vec3LengthRounded(int* v);
extern "C" void func_020c2f18(struct Vec3* a, struct Vec3* b);

struct FixedVec3 { int x; int y; int z; };
ARM int DotFixedVec3(struct FixedVec3* a, struct FixedVec3* b);

extern "C" void func_0200f374(void* buf, int n);

struct Struct0201d474 { int v[3]; int tail; };
extern struct Struct0201d474* CopyStruct16(struct Struct0201d474* dst, struct Struct0201d474* src);

struct RecordArray0201e710 { char pad[0x24]; void* items; int count; int capacity; };
extern "C" void* func_0201e710(struct RecordArray0201e710* arr, void* src);

struct GlobalCtx0201d530 { void* lastEntry; int pad4; struct RecordArray0201e710* arr; };
extern struct GlobalCtx0201d530 data_020fdc20;

struct RecordX {
    short f0;
    int mode;
    int vecA[3];
    int vecB[3];
    short f20;
    short f22;
    int f24;
    int f28;
    unsigned char code;
    char valid;
    unsigned short f2e_lo : 4;
    unsigned short f2e_hi : 12;
    int f30[3];
    int f3c;
    int f40;
    int f44[3];
    int f50[3];
    int f5c[3];
    char pad68[8];
    int f70;
};

// USA: func_0201d044
extern "C" ARM int func_0201d044(char* a, int count) {
    struct RecordX obj;
    ResetStruct0201d424((char*)&obj);

    obj.mode = GetIntFromVariant02030b0c((struct Variant02030b0c*)a);
    int highMode = 0;
    if (obj.mode >= 10) {
        obj.mode -= 10;
        highMode = 1;
    }

    int tmp[3];
    a = (char*)ConvertTaggedVec3ToFx32((struct TaggedValue02030b44*)(a + 8), tmp);
    CopyVec3(obj.vecA, tmp);

    a = (char*)ConvertTaggedVec3ToFx32((struct TaggedValue02030b44*)a, tmp);
    CopyVec3(obj.vecB, tmp);

    if (highMode) {
        float v = GetTaggedValueAsFloat((struct TaggedValue02030b44*)a);
        a = a + 8;
        obj.f22 = (short)func_02030f30((int)(4096.0f * v));
    }
    obj.f20 = (short)(int)(4096.0f * GetTaggedValueAsFloat((struct TaggedValue02030b44*)a));

    int halfX = obj.vecB[0] / 2;
    int halfZ = obj.vecB[2] / 2;
    int qz = (int)(((long long)halfZ * halfZ + 0x800) >> 12);
    obj.f24 = qz + (int)(((long long)halfX * halfX + 0x800) >> 12);

    if (obj.mode == 2) {
        obj.code = (unsigned char)GetIntFromVariant02030b0c((struct Variant02030b0c*)(a + 8));
        obj.valid = (unsigned char)GetIntFromVariant02030b0c((struct Variant02030b0c*)(a + 0x10));
        unsigned short v = (unsigned short)GetIntFromVariant02030b0c((struct Variant02030b0c*)(a + 0x18));
        obj.f2e_hi = v;
    } else if (obj.mode == 3) {
        obj.code = (unsigned char)GetIntFromVariant02030b0c((struct Variant02030b0c*)(a + 8));
    } else if (obj.mode == 4) {
        void* p = a + 8;
        a = a + 0x10;
        unsigned char raw = (unsigned char)GetIntFromVariant02030b0c((struct Variant02030b0c*)p);
        unsigned char merged = (unsigned char)((obj.code & ~0x7f) | (raw & 0x7f));
        merged &= ~0x80;
        obj.valid = (unsigned char)-1;
        obj.code = merged;

        switch ((unsigned int)(merged << 25) >> 25) {
        case 0: {
            float v0 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)a);
            obj.f30[1] = (int)(4096.0f * v0);
            void* p2 = a + 8;
            a = a + 0x10;
            float v1 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)p2);
            obj.f30[2] = (int)(4096.0f * v1);
            break;
        }
        case 1:
            a = (char*)ConvertTaggedVec3ToFx32((struct TaggedValue02030b44*)a, tmp);
            CopyVec3(obj.f30, tmp);
            break;
        case 2:
        case 3:
            a = (char*)ConvertTaggedVec3ToFx32((struct TaggedValue02030b44*)a, tmp);
            CopyVec3(obj.f30, tmp);
            break;
        case 4:
        case 5: {
            struct Vec3 tmpC, tmpB;
            struct { struct Vec3 v; int dot; } diff;
            a = (char*)ConvertTaggedVec3ToFx32((struct TaggedValue02030b44*)a, (int*)&tmpC);
            a = (char*)ConvertTaggedVec3ToFx32((struct TaggedValue02030b44*)a, (int*)&tmpB);
            SubtractVec3(&tmpB, &tmpC, &diff.v);
            int len = Vec3LengthRounded((int*)&diff.v);
            func_020c2f18(&diff.v, &diff.v);
            diff.dot = DotFixedVec3((struct FixedVec3*)&diff.v, (struct FixedVec3*)&tmpC);

            int scratch[3];
            func_0200f374(scratch, 0xc);

            if (((unsigned int)(obj.code << 25) >> 25) == 4) {
                float v0 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)a);
                a = a + 8;
                scratch[0] = (int)(4096.0f * v0);
            }
            float v1 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)a);
            scratch[1] = (int)(4096.0f * v1);
            float v2 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)(a + 8));
            scratch[2] = (int)(4096.0f * v2);

            int tmp2[3];
            a = (char*)ConvertTaggedVec3ToFx32((struct TaggedValue02030b44*)(a + 0x10), tmp2);
            CopyStruct16((struct Struct0201d474*)obj.f30, (struct Struct0201d474*)&diff);
            obj.f40 = len;
            CopyVec3(obj.f44, scratch);
            CopyVec3(obj.f50, tmp2);
            break;
        }
        default:
            break;
        }

        obj.f5c[0] = 0;
        obj.f5c[1] = 0;
        obj.f5c[2] = 0;
        if (count > 0xc) {
            a = (char*)ConvertTaggedVec3ToFx32((struct TaggedValue02030b44*)a, tmp);
            CopyVec3(obj.f5c, tmp);
        }
    } else if (obj.mode == 5) {
        *(short*)&obj.code = (short)GetIntFromVariant02030b0c((struct Variant02030b0c*)(a + 8));
        *(unsigned short*)((char*)&obj + 0x2e) = (unsigned short)GetIntFromVariant02030b0c((struct Variant02030b0c*)(a + 0x10));
        *(short*)&obj.f30[0] = 0;
    }

    obj.f70 = 0;
    func_0201e710(data_020fdc20.arr, &obj);
    return 1;
}
