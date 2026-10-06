#include <globaldefs.h>
#include "Memory/SafeAllocator.h"

struct Variant02030b0c { int tag; union { int i; float f; } u; };
int GetIntFromVariant02030b0c(struct Variant02030b0c* p);

struct TaggedValue02030b44 { int type; union { int i; float f; } value; };
float GetTaggedValueAsFloat(struct TaggedValue02030b44* v);

struct Struct02030b7c { int field0; void* field4; };
void* GetField4IfField0Zero(struct Struct02030b7c* s);

extern "C" void* memcpy(void*, const void*, unsigned);

// USA: func_0205ec70  (semantic: ParseVariantList_0205ec70)
extern "C" ARM void func_0205ec70(SafeAllocator* allocator, int count, unsigned char* list, void** outSimpleHead, void** outCompoundHead) {
    int consumed = 0;
    unsigned char* volatile prevSimple = 0;
    unsigned char* volatile prevCompound = 0;

    while (consumed < count) {
        int variant = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
        int value = variant;
        int tag = variant >> 16;
        list += 8;

        if (tag < 0x64 || tag >= 0x1f4) {
            unsigned char* sb = (unsigned char*)allocator->Allocate(0x10);
            if (!sb) return;
            if (*outSimpleHead == 0) *outSimpleHead = sb;
            else *(void**)(prevSimple + 0xc) = sb;

            switch (tag) {
            case 29: {
                int v1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 0));
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 4) = (short)value;
                *(short*)(sb + 6) = (short)v1;
                int v2 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 8));
                list += 0x10;
                float t = (float)v2 * 3.14159274f;
                t = t / 180.0f;
                if (t > (3.14159274f * 2.0f)) {
                    t = t - (3.14159274f * 2.0f);
                } else if (t < 0.0f) {
                    t = t + (3.14159274f * 2.0f);
                }
                *(short*)(sb + 2) = (short)(int)(t * 4096.0f);
                consumed += 2;
                break;
            }
            case 32: {
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                list += 8;
                *(unsigned char*)(sb + 4) = (unsigned char)((unsigned int)v >> 24);
                *(unsigned char*)(sb + 5) = (unsigned char)((unsigned int)v >> 16);
                *(unsigned char*)(sb + 6) = (unsigned char)((unsigned int)v >> 8);
                *(unsigned char*)(sb + 7) = (unsigned char)v;
                consumed += 1;
                break;
            }
            case 33: {
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                list += 8;
                *(unsigned char*)(sb + 4) = (unsigned char)((unsigned int)v >> 24);
                *(unsigned char*)(sb + 5) = (unsigned char)((unsigned int)v >> 16);
                *(unsigned char*)(sb + 6) = (unsigned char)((unsigned int)v >> 8);
                *(unsigned char*)(sb + 7) = (unsigned char)v;
                consumed += 1;
                break;
            }
            case 44: case 47: case 48: case 49: case 70: case 73: case 75: case 76: case 83: case 90: case 500: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 4) = (short)v;
                list += 8;
                consumed += 1;
                break;
            }
            case 46: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                list += 8;
                *(short*)(sb + 4) = (short)(v >> 16);
                *(short*)(sb + 6) = (short)v;
                consumed += 1;
                break;
            }
            case 66: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                list += 8;
                *(short*)(sb + 4) = (short)(v >> 16);
                *(short*)(sb + 6) = (short)v;
                consumed += 1;
                break;
            }
            case 50: case 82: {
                unsigned short utag = (unsigned short)tag;
                int q0 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 0));
                int q1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 8));
                int q2 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 0x10));
                int q3 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 0x18));
                list += 0x20;
                *(unsigned short*)(sb + 0) = utag;
                *(unsigned short*)(sb + 2) = 1;
                *(short*)(sb + 4) = (short)value;
                *(short*)(sb + 6) = (short)(q0 >> 16);
                *(short*)(sb + 8) = (short)q0;
                *(short*)(sb + 0xa) = (short)(q1 >> 16);
                unsigned char* nn = (unsigned char*)allocator->Allocate(0x10);
                *(unsigned short*)(nn + 0) = utag;
                *(unsigned short*)(nn + 2) = 2;
                *(short*)(nn + 4) = (short)q1;
                *(short*)(nn + 6) = (short)(q2 >> 16);
                *(short*)(nn + 8) = (short)q2;
                *(short*)(nn + 0xa) = (short)q3;
                *(void**)(sb + 0xc) = nn;
                sb = nn;
                consumed += 4;
                break;
            }
            case 52: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                list += 8;
                *(short*)(sb + 4) = (short)(v >> 16);
                *(short*)(sb + 6) = (short)v;
                consumed += 1;
                break;
            }
            case 55: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                list += 8;
                *(short*)(sb + 4) = (short)(v >> 16);
                *(short*)(sb + 6) = (short)v;
                consumed += 1;
                break;
            }
            case 62: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                list += 8;
                *(short*)(sb + 4) = (short)(v >> 16);
                *(short*)(sb + 6) = (short)v;
                consumed += 1;
                break;
            }
            case 63: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                list += 8;
                *(short*)(sb + 4) = (short)(v >> 16);
                *(short*)(sb + 6) = (short)v;
                consumed += 1;
                break;
            }
            case 53: case 54: case 56: case 57: case 58: case 59: case 60: case 61: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v0 = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 4) = (short)(v0 >> 16);
                *(short*)(sb + 6) = (short)v0;
                int v1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 8));
                list += 0x10;
                *(short*)(sb + 8) = (short)(v1 >> 16);
                *(short*)(sb + 0xa) = (short)v1;
                consumed += 2;
                break;
            }
            case 64: case 65: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 4) = (short)value;
                int v0 = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 6) = (short)(v0 >> 16);
                *(short*)(sb + 8) = (short)v0;
                int v1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 8));
                list += 0x10;
                *(short*)(sb + 0xa) = (short)(v1 >> 16);
                consumed += 2;
                break;
            }
            case 69: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                void* buf = allocator->Allocate(0xb);
                *(void**)(sb + 4) = buf;
                void* src = GetField4IfField0Zero((struct Struct02030b7c*)list);
                list += 8;
                memcpy(buf, src, 0xb);
                consumed += 1;
                break;
            }
            case 74: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 2) = (short)v;
                list += 8;
                consumed += 1;
                break;
            }
            case 78: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v0 = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 4) = (short)(v0 >> 16);
                *(short*)(sb + 6) = (short)v0;
                int v1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 8));
                list += 0x10;
                *(short*)(sb + 8) = (short)v1;
                consumed += 2;
                break;
            }
            case 79: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(unsigned char*)(sb + 4) = (unsigned char)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 2) = (short)v;
                list += 8;
                consumed += 1;
                break;
            }
            case 91: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(unsigned char*)(sb + 4) = (unsigned char)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 2) = (short)v;
                list += 8;
                consumed += 1;
                break;
            }
            case 85: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(int*)(sb + 4) = v;
                list += 8;
                consumed += 1;
                break;
            }
            default:
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                break;
            }

            *(void**)(sb + 0xc) = 0;
            prevSimple = sb;
        } else {
            unsigned char* sb = (unsigned char*)allocator->Allocate(0x1c);
            if (!sb) return;
            if (*outCompoundHead == 0) *outCompoundHead = sb;
            else *(void**)(prevCompound + 0x14) = sb;

            switch (tag) {
            case 122: {
                float f0 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)(list + 0));
                float f1 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)(list + 8));
                float f2 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)(list + 0x10));
                list += 0x18;
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int r0v; if (f0 > 0.0f) r0v = (int)(4096.0f * f0 + 0.5f); else r0v = (int)(4096.0f * f0 - 0.5f);
                *(int*)(sb + 4) = r0v;
                int r1v; if (f1 > 0.0f) r1v = (int)(4096.0f * f1 + 0.5f); else r1v = (int)(4096.0f * f1 - 0.5f);
                *(int*)(sb + 8) = r1v;
                int r2v; if (f2 > 0.0f) r2v = (int)(4096.0f * f2 + 0.5f); else r2v = (int)(4096.0f * f2 - 0.5f);
                *(int*)(sb + 0xc) = r2v;
                consumed += 3;
                break;
            }
            case 108: case 109: case 136: case 152: case 153: case 154: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 4) = (short)value;
                int v1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 6) = (short)v1;
                int v2 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 8));
                list += 0x10;
                float t = (float)v2 * 3.14159274f;
                t = t / 180.0f;
                if (t > (3.14159274f * 2.0f)) {
                    t = t - (3.14159274f * 2.0f);
                } else if (t < 0.0f) {
                    t = t + (3.14159274f * 2.0f);
                }
                *(short*)(sb + 2) = (short)(int)(t * 4096.0f);
                consumed += 2;
                break;
            }
            case 139: case 140: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(int*)(sb + 4) = v;
                list += 8;
                consumed += 1;
                break;
            }
            case 143: {
                float f0 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)(list + 0));
                int r0v; if (f0 > 0.0f) r0v = (int)(4096.0f * f0 + 0.5f); else r0v = (int)(4096.0f * f0 - 0.5f);
                int tmp[5];
                float f1 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)(list + 8));
                if (f1 > 0.0f) tmp[0] = (int)(4096.0f * f1 + 0.5f); else tmp[0] = (int)(4096.0f * f1 - 0.5f);
                float f2 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)(list + 0x10));
                if (f2 > 0.0f) tmp[1] = (int)(4096.0f * f2 + 0.5f); else tmp[1] = (int)(4096.0f * f2 - 0.5f);
                float f3 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)(list + 0x18));
                if (f3 > 0.0f) tmp[2] = (int)(4096.0f * f3 + 0.5f); else tmp[2] = (int)(4096.0f * f3 - 0.5f);
                float f4 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)(list + 0x20));
                if (f4 > 0.0f) tmp[3] = (int)(4096.0f * f4 + 0.5f); else tmp[3] = (int)(4096.0f * f4 - 0.5f);
                float f5 = GetTaggedValueAsFloat((struct TaggedValue02030b44*)(list + 0x28));
                if (f5 > 0.0f) tmp[4] = (int)(4096.0f * f5 + 0.5f); else tmp[4] = (int)(4096.0f * f5 - 0.5f);
                int r6v = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 0x30));
                list += 0x38;
                *(unsigned short*)(sb + 2) = 1;
                *(unsigned short*)(sb + 4) = (unsigned short)value;
                *(int*)(sb + 8) = r0v;
                *(int*)(sb + 0xc) = tmp[0];
                *(int*)(sb + 0x10) = tmp[1];
                unsigned char* nn = (unsigned char*)allocator->Allocate(0x1c);
                *(unsigned short*)(nn + 0) = *(unsigned short*)(sb + 0);
                *(unsigned short*)(nn + 2) = 2;
                *(int*)(nn + 4) = tmp[2];
                *(int*)(nn + 8) = tmp[3];
                *(int*)(nn + 0xc) = tmp[4];
                *(int*)(nn + 0x10) = r6v;
                *(void**)(sb + 0x14) = nn;
                *(int*)(sb + 0x18) = 0;
                sb = nn;
                consumed += 7;
                break;
            }
            case 156: case 157: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                list += 8;
                *(short*)(sb + 4) = (short)(v >> 16);
                *(short*)(sb + 6) = (short)v;
                consumed += 1;
                break;
            }
            case 167: case 168: case 169: case 171: case 172: case 174: case 196: case 200: case 201: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(unsigned char*)(sb + 2) = (unsigned char)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(unsigned char*)(sb + 4) = (unsigned char)(v >> 16);
                list += 8;
                consumed += 1;
                break;
            }
            case 178: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(unsigned char*)(sb + 2) = (unsigned char)value;
                int v0 = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(unsigned char*)(sb + 4) = (unsigned char)(v0 >> 16);
                *(unsigned char*)(sb + 5) = (unsigned char)v0;
                int v1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 8));
                *(short*)(sb + 6) = (short)v1;
                int v2 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 0x10));
                list += 0x18;
                *(unsigned char*)(sb + 8) = (unsigned char)(v2 >> 16);
                consumed += 3;
                break;
            }
            case 188: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v0 = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 4) = (short)(v0 >> 16);
                *(short*)(sb + 6) = (short)v0;
                int v1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 8));
                list += 0x10;
                *(short*)(sb + 8) = (short)v1;
                consumed += 2;
                break;
            }
            case 189: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(unsigned char*)(sb + 2) = (unsigned char)((unsigned short)value != 0 ? 1 : 0);
                consumed += 0;
                break;
            }
            case 190: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v0 = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 4) = (short)(v0 >> 16);
                *(short*)(sb + 6) = (short)v0;
                int v1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 8));
                list += 0x10;
                *(short*)(sb + 8) = (short)v1;
                consumed += 2;
                break;
            }
            case 195: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(unsigned char*)(sb + 2) = (unsigned char)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(unsigned char*)(sb + 4) = (unsigned char)(v >> 16);
                list += 8;
                consumed += 1;
                break;
            }
            case 207: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v0 = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 4) = (short)(v0 >> 16);
                *(short*)(sb + 6) = (short)v0;
                int v1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 8));
                list += 0x10;
                *(short*)(sb + 8) = (short)(v1 >> 16);
                *(short*)(sb + 0xa) = (short)v1;
                consumed += 2;
                break;
            }
            case 213: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 4) = (short)value;
                int v0 = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 6) = (short)(v0 >> 16);
                *(short*)(sb + 8) = (short)v0;
                int v1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 8));
                list += 0x10;
                *(short*)(sb + 0xa) = (short)v1;
                consumed += 2;
                break;
            }
            case 215: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v0 = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 4) = (short)(v0 >> 16);
                *(short*)(sb + 6) = (short)v0;
                int v1 = GetIntFromVariant02030b0c((struct Variant02030b0c*)(list + 8));
                list += 0x10;
                *(short*)(sb + 8) = (short)(v1 >> 16);
                *(short*)(sb + 0xa) = (short)v1;
                consumed += 2;
                break;
            }
            case 220: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)((value & 0xff00) >> 8);
                *(unsigned char*)(sb + 4) = (unsigned char)value;
                consumed += 0;
                break;
            }
            case 116: case 117: case 118: case 130: case 131: case 133: case 134: case 135: case 138:
            case 144: case 149: case 150: case 155: case 158: case 159: case 176: case 177: case 179:
            case 180: case 226: case 230: case 232: {
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                int v = GetIntFromVariant02030b0c((struct Variant02030b0c*)list);
                *(short*)(sb + 4) = (short)(v >> 16);
                list += 8;
                consumed += 1;
                break;
            }
            default:
                *(unsigned short*)(sb + 0) = (unsigned short)tag;
                *(short*)(sb + 2) = (short)value;
                break;
            }

            *(void**)(sb + 0x14) = 0;
            *(int*)(sb + 0x18) = 0;
            prevCompound = sb;
        }

        consumed += 1;
    }
}
