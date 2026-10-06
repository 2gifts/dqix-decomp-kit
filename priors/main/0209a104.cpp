#include <globaldefs.h>

struct Variant02030b0c;
extern "C" int _ZNK6Script9Parameter5ToIntEv(struct Variant02030b0c*);

struct W12 {
    unsigned int a : 5;
    unsigned int b : 7;
    unsigned int c : 8;
    unsigned int d : 5;
    unsigned int e : 7;
};
union U12 { struct W12 b; unsigned int raw; volatile unsigned int vraw; };
struct Record12 {
    unsigned short id : 11;
    unsigned short slot : 5;
    unsigned short f2;
    union U12 u;
    unsigned short f8;
    unsigned short fa;
};

struct RecordList0209a4f0;
void AppendRecord12Capped0209a4f0(struct RecordList0209a4f0* list, struct Record12* src);

extern struct RecordList0209a4f0* data_02109ba4;

// USA: func_0209a104
extern "C" ARM int func_0209a104(char* v) {
    struct Record12 rec;

    rec.id = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)v);
    rec.u.b.a = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)(v + 8));
    rec.u.b.b = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)(v + 0x10));
    rec.f2 = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)(v + 0x18));
    rec.slot = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)(v + 0x20));
    { unsigned int _vc = (unsigned int)_ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)(v + 0x28)) << 24; rec.u.raw &= ~0xff000u; rec.u.raw |= _vc >> 12; }
    rec.f8 = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)(v + 0x30));
    rec.fa = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)(v + 0x38));
    rec.u.b.d = _ZNK6Script9Parameter5ToIntEv((struct Variant02030b0c*)(v + 0x40));
    rec.u.vraw = rec.u.raw & 0x01ffffffu;

    AppendRecord12Capped0209a4f0(data_02109ba4, &rec);
    return 1;
}
