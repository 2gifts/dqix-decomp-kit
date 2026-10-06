#include <globaldefs.h>

struct Param02030b0c { unsigned char _u[8]; };
extern "C" int _ZNK6Script9Parameter5ToIntEv(const struct Param02030b0c* self);

struct Record12 {
    unsigned short f0a : 11;
    unsigned short f0b : 5;
    unsigned short f2;
    unsigned int f4a : 5;
    unsigned int f4b : 7;
    unsigned int f4c : 8;
    unsigned int f4d : 5;
    unsigned int f4e : 3;
    unsigned int f4f : 4;
    unsigned short f8;
    unsigned short fa;
};

struct RecordList0209a55c;
void AppendRecord12Capped(struct RecordList0209a55c* list, struct Record12* src);

extern struct RecordList0209a55c* data_02109ba4;

// USA: func_0209a218
extern "C" ARM int func_0209a218(struct Param02030b0c* args) {
    struct Record12 rec;

    rec.f0a = _ZNK6Script9Parameter5ToIntEv(&args[0]);
    rec.f4a = _ZNK6Script9Parameter5ToIntEv(&args[1]);
    rec.f4b = _ZNK6Script9Parameter5ToIntEv(&args[2]);
    rec.f2 = _ZNK6Script9Parameter5ToIntEv(&args[3]);
    rec.f0b = _ZNK6Script9Parameter5ToIntEv(&args[4]);
    rec.f4c = _ZNK6Script9Parameter5ToIntEv(&args[5]);
    rec.f8 = _ZNK6Script9Parameter5ToIntEv(&args[6]);
    rec.fa = _ZNK6Script9Parameter5ToIntEv(&args[7]);
    rec.f4d = _ZNK6Script9Parameter5ToIntEv(&args[8]);
    rec.f4f = 0;
    rec.f4e = 0;

    if (rec.f4c != 0) {
        AppendRecord12Capped(data_02109ba4, &rec);
    }
    return 1;
}
