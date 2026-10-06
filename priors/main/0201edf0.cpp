#include <globaldefs.h>

struct Variant02030b0c { int tag; union { int i; float f; } u; };
int GetIntFromVariant02030b0c(Variant02030b0c* p);

struct TaggedValue02030b44 { int type; union { int i; float f; } value; };
float GetTaggedValueAsFloat(TaggedValue02030b44* v);

struct Struct02030b7c { int field0; void* field4; };
void* GetField4IfField0Zero(Struct02030b7c* s);

struct Inner3_0201f214 { unsigned int v[3]; };
struct Element0201f214 {
    short s0;
    short s2;
    short s4;
    unsigned short h6;
    Inner3_0201f214 mid8;
    unsigned short h14;
    unsigned short h16;
    unsigned short h18;
    unsigned short h1a;
    unsigned short h1c;
    unsigned short h1e;
};
struct List0201f214;
void AppendCappedElement0201f214(List0201f214* list, Element0201f214* src);

struct Container0201edf0 {
    int f0;
    void* alloc;
    List0201f214* list;
};
extern Container0201edf0 data_020fdc40;

// USA: func_0201edf0
extern "C" ARM int func_0201edf0(char* p, int count) {
    Element0201f214 elem;

    int s0 = GetIntFromVariant02030b0c((Variant02030b0c*)p);
    int s2 = GetIntFromVariant02030b0c((Variant02030b0c*)(p + 8));
    float f10 = GetTaggedValueAsFloat((TaggedValue02030b44*)(p + 0x10));
    float f18 = GetTaggedValueAsFloat((TaggedValue02030b44*)(p + 0x18));
    float f20 = GetTaggedValueAsFloat((TaggedValue02030b44*)(p + 0x20));
    int s4 = GetIntFromVariant02030b0c((Variant02030b0c*)(p + 0x28));
    Struct02030b7c* t0 = (Struct02030b7c*)(p + 0x30);
    p += 0x38;
    GetField4IfField0Zero(t0);

    float a = 1.0f, b = 1.0f, c = 1.0f;
    if (count > 10) {
        a = GetTaggedValueAsFloat((TaggedValue02030b44*)p);
        b = GetTaggedValueAsFloat((TaggedValue02030b44*)(p + 8));
        TaggedValue02030b44* t = (TaggedValue02030b44*)(p + 0x10);
        p += 0x18;
        c = GetTaggedValueAsFloat(t);
    }

    float d = 0.0f, e = 0.0f, f = 0.0f;
    if (count > 7) {
        d = GetTaggedValueAsFloat((TaggedValue02030b44*)p);
        e = GetTaggedValueAsFloat((TaggedValue02030b44*)(p + 8));
        TaggedValue02030b44* t2 = (TaggedValue02030b44*)(p + 0x10);
        p += 0x18;
        f = GetTaggedValueAsFloat(t2);
    }

    unsigned char flags = 0xf;
    if (count > 13) {
        flags = (unsigned char)GetIntFromVariant02030b0c((Variant02030b0c*)p);
        if (flags & 0x10) flags = flags & ~0x10; else flags = flags | 0x10;
        if (flags & 0x20) flags = flags & ~0x20; else flags = flags | 0x20;
    }

    elem.s0 = (short)s0;
    elem.s2 = (short)s2;
    elem.s4 = (short)s4;
    elem.mid8.v[0] = (unsigned int)(int)(4096.0f * f10);
    elem.mid8.v[1] = (unsigned int)(int)(4096.0f * f18);
    elem.mid8.v[2] = (unsigned int)(int)(4096.0f * f20);
    elem.h14 = (unsigned short)(int)(4096.0f * d);
    elem.h16 = (unsigned short)(int)(4096.0f * e);
    elem.h18 = (unsigned short)(int)(4096.0f * f);
    elem.h1a = (unsigned short)(int)(4096.0f * a);
    elem.h1c = (unsigned short)(int)(4096.0f * b);
    elem.h1e = (unsigned short)(int)(4096.0f * c);
    elem.h6 = flags;

    AppendCappedElement0201f214(data_020fdc40.list, &elem);
    return 1;
}
