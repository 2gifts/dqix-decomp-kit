#include <globaldefs.h>
#include "std_library_functions.h"

extern "C" void _Z25LoadStateBytePair0206b804PcPiS0_(char* obj, int* out1, int* out2);
extern "C" void func_02044c74(void* obj, void* out);
extern "C" void func_02066cf0(void* self, int flag);
extern "C" void _Z25ZeroFourHalfwords02044074P14Struct02044074(void* s);
extern "C" void func_02042b98(void* self, int arg1, int arg2, int arg3, int arg4);
extern "C" void _Z16SetFourHalfwordsP14Struct0204408cssss(void* s, short x, short y, short z, short w);
extern "C" void func_02043600(void* a, void* box);
extern "C" int _Z26EncodeStreamFields020dc0e0iiiihh(int a, int b, int c, int d, unsigned char e, unsigned char f);
extern "C" int _Z25EncodeStreamValue020dc0c8i(int value);
extern "C" void _Z23InitCombatSlots02043040Pc(char* obj);
extern "C" void func_02043124(void* self);
extern "C" ARM void _Z28ResetControllerState020430b0Pc(char* self);
extern "C" void func_020437bc(void* buf, void* box, int index);

struct Struct0204394c;
extern "C" int _Z33PrepareEncodeStreamBuffer0204394cP14Struct0204394c(struct Struct0204394c* obj);

struct Entry020e2cc4;
extern "C" void _Z23SetEntryEnabled020e2cc4P13Entry020e2cc4i(struct Entry020e2cc4* obj, int enabled);

extern "C" void _Z25RestorePairTables0207df90Pc(char* obj);
struct Obj02041754;
extern "C" void _Z26DisableField16Bit002041754P11Obj02041754(struct Obj02041754* obj);
extern "C" void _Z24BackupPairTables0207dfacPc(char* obj);
void ClearBitInArray(int unused, unsigned char* arr, int index);

struct Rect020e2d2c;
void SetRectFromPosAndSize(struct Rect020e2d2c* s, int a, int b, int c, int d);

struct Entry020e2c34;
extern "C" void _Z26UpdateAndDrawEntry020e2c34P13Entry020e2c34(struct Entry020e2c34* obj);

struct Foo02042fcc {
    short a0;
    short a2;
    short a4;
    short a6;
    int a8;
};

struct Holder02042c24 {
    char pad[0x440];
    void* field440;
};

extern void* data_02107800;

// SCRATCH-USA: func_020439b0
extern "C" ARM void func_020439b0(char* self, char *flag_p) {
    int flag = (int)(flag_p - (char *)0);
    struct Struct0204408c { short x, y, z, w; };
    int dx, dy;
    Struct0204408c local18, local10, local8;

    if (*(unsigned char*)(self + 0x19cc) != 0) {
        _Z25LoadStateBytePair0206b804PcPiS0_(self, &dx, &dy);
        func_02044c74(self, *(void**)(self + 0x34));

        void* h = *(void**)(self + 0x34);
        int hv = *(int*)((char*)h + 8);
        if (hv != 0) {
            *(int*)((char*)h + 8) = hv;

            *(unsigned char*)(self + 0x19cc) = 0;
            func_02066cf0(self, 1);
            *(unsigned char*)(self + 0x19cc) = 1;
            if (*(unsigned char*)(self + 0x19c0) != 0) {
                *(int*)(self + 0x40) = 0;
            }

            _Z25ZeroFourHalfwords02044074P14Struct02044074(&local18);
            _Z25ZeroFourHalfwords02044074P14Struct02044074(&local10);
            _Z25ZeroFourHalfwords02044074P14Struct02044074(&local8);

            if (*(int*)(self + 0x998) != 0 && *(unsigned char*)(self + 0x19b1) != 0) {
                goto Ilabel;
            }
            if (*(unsigned char*)(self + 0x19cb) != 0) {
            Ilabel:
                if (*(unsigned char*)(self + 0x19b1) != 0) {
                    func_02042b98(self, 2, 0x74, 0xfc, 0x4a);
                    _Z16SetFourHalfwordsP14Struct0204408cssss(&local18, (short)(dx + 2), (short)(dy + 0x74), (short)0xfc, (short)0x4a);
                    func_02043600(*(void**)(self + 0x34), &local18);
                    _Z16SetFourHalfwordsP14Struct0204408cssss(&local18, 2, 0x74, 0xfc, 0x4a);

                    if (*(unsigned char*)(self + 0x19c4) != 0 && *(unsigned char*)(self + 0x19c3) != 0) {
                        struct Foo02042fcc* target = (struct Foo02042fcc*)(self + 0x930);
                        struct Holder02042c24* holder = (struct Holder02042c24*)(self + 0x19e0);
                        holder->field440 = (void*)target;
                        int cond = (target != 0) ? (target->a8 == 2) : 0;
                        if (cond) {
                            short sx, sy, sz, sw;
                            if (target != 0) {
                                void* cur1 = holder->field440;
                                sx = ((short*)cur1)[0];
                                sy = ((short*)cur1)[1];
                            }
                            if (target != 0) {
                                void* cur2 = holder->field440;
                                sz = ((short*)cur2)[2];
                                sw = ((short*)cur2)[3];
                            }
                            _Z16SetFourHalfwordsP14Struct0204408cssss(&local10, (short)(sx + dx), (short)(sy + dy), sz, sw);
                            func_02043600(*(void**)(self + 0x34), &local10);
                            _Z16SetFourHalfwordsP14Struct0204408cssss(&local10, sx, sy, sz, sw);
                            holder->field440 = (void*)(self + 0x914);
                        }
                    }

                    if (*(int*)(self + 0x9a0) == 6) {
                        if (*(unsigned char*)(self + 0x19ba) != 0 && *(unsigned char*)(self + 0x2c3) != 0) {
                            _Z16SetFourHalfwordsP14Struct0204408cssss(&local8, *(short*)(self + 0x144), *(short*)(self + 0x146),
                                              *(short*)(self + 0x148), *(short*)(self + 0x14a));
                            func_02043600(*(void**)(self + 0x34), &local8);
                        }
                    }
                }
            }

            void* h2 = *(void**)(self + 0x34);
            if (h2 != 0) {
                short sx = *(short*)((char*)h2 + 0xac);
                short sy = *(short*)((char*)h2 + 0xae);
                short sw = *(short*)((char*)h2 + 0xa8);
                short count = *(short*)((char*)h2 + 0xaa);
                int base = (sx + (sy << 5)) << 5;
                int val = *(int*)((char*)h2 + 8);
                short i = 0;
                int step = sw << 5;
                for (; i < count; i++) {
                    _Z26EncodeStreamFields020dc0e0iiiihh(0xa, val, base, step, 1, 0);
                    val += step;
                    base += 0x400;
                }
            }

            int oldX = *(int*)(self + 0x94c);
            int oldY = *(int*)(self + 0x950);
            *(volatile unsigned int*)0x4000018 = ((-oldX) & 0x1ff) | (((-oldY) << 16) & 0x1ff0000);
            *(int*)(self + 0x94c) = dx;
            *(int*)(self + 0x950) = dy;

            if (*(int*)(self + 0x9a0) != 1 && *(int*)(self + 0x9a0) != 0) {
                memset(data_02107800, 0, 0x800);

                if (*(int*)(self + 0x998) != 0 && *(unsigned char*)(self + 0x19b1) != 0) {
                    goto Rlabel;
                }
                if (*(unsigned char*)(self + 0x19cb) != 0) {
                Rlabel:
                    if (*(unsigned char*)(self + 0x19b1) != 0) {
                        void* rbuf = data_02107800;
                        func_020437bc(rbuf, &local18, 0);
                        func_020437bc(rbuf, &local10, 1);
                        func_020437bc(rbuf, &local8, 2);
                        _Z26EncodeStreamFields020dc0e0iiiihh(7, (int)rbuf, 0, 0x800, 1, 0);
                    }
                }

                if (*(unsigned char*)(self + 0x19b1) == 0) {
                    _Z26EncodeStreamFields020dc0e0iiiihh(7, (int)data_02107800, 0, 0x800, 1, 0);
                }

                if (!(*(int*)(self + 0x74) != *(int*)(self + 0x78) && *(int*)(self + 0x998) != 0)) {
                    short* buf = (short*)data_02107800;
                    unsigned short i;
                    for (i = 0; i < 0x100; i++) {
                        buf[0x400 + i] = 0;
                    }
                    for (i = 0x100; i < 0x300; i++) {
                        buf[0x400 + i] = (short)i;
                    }
                    _Z26EncodeStreamFields020dc0e0iiiihh(9, (int)((char*)buf + 0x800), 0, 0x800, 1, 0);
                }
            }

            *(volatile unsigned int*)0x4000000 = (*(volatile unsigned int*)0x4000000 & ~0x1f00) | 0x1d00;
        }

        _Z25EncodeStreamValue020dc0c8i(*(int*)(*(char**)(self + 0x34) + 8));

        if (*(int*)(self + 0x998) == 0) {
            _Z23InitCombatSlots02043040Pc(self);
            func_02043124(self);
            _Z28ResetControllerState020430b0Pc(self);
        }
    }

    if (*(unsigned char*)(self + 0x19ce) != 0 && *(int*)(self + 0x44) == 0) {
        _Z33PrepareEncodeStreamBuffer0204394cP14Struct0204394c((struct Struct0204394c*)(*(void**)(self + 0x1e28)));

        int i;
        for (i = 0; i < 2; i++) {
            _Z23SetEntryEnabled020e2cc4P13Entry020e2cc4i((struct Entry020e2cc4*)((char*)(self + 0x1964) + i * 0x24), 0);
        }
    }

    if (flag != 0) {
        if (*(int*)(self + 0x998) == 0) {
            return;
        }
        void* fe28 = *(void**)(self + 0x1e28);
        if (fe28 != 0) {
            int j;
            j = 0;
            if (j < 0x80) {
                do {
                    if (*(unsigned char*)(self + 0x17b8 + j) != 0) {
                        char* entry = (char*)(self + 0x9b8) + j * 0x1c;
                        struct Bit0x16 { unsigned char bit0 : 1; };
                        struct Bit0x16* b16 = (struct Bit0x16*)(entry + 0x16);
                        if (b16->bit0) {
                            if (*(unsigned char*)(self + 0x19cc) != 0 || *(unsigned char*)(self + 0x19ce) != 0) {
                                b16->bit0 = 0;
                            } else {
                                _Z25RestorePairTables0207df90Pc(*(char**)(self + 0x1e28));
                                _Z26DisableField16Bit002041754P11Obj02041754((struct Obj02041754*)entry);
                                _Z24BackupPairTables0207dfacPc(*(char**)(self + 0x1e28));
                            }
                            ClearBitInArray(0, (unsigned char*)(self + 0x2cc), *(int*)(entry + 8));
                        }
                    }
                    j++;
                } while (j < 0x80);
            }
        }
    }

    if (*(int*)(self + 0x91c) >= 2) {
        SetRectFromPosAndSize((struct Rect020e2d2c*)(self + 0x1964), 4, 0x76, 0xf8, 0x46);
        if (*(unsigned char*)(self + 0x1962) != 0) {
            _Z23SetEntryEnabled020e2cc4P13Entry020e2cc4i((struct Entry020e2cc4*)(self + 0x1964), 1);
        }
    }

    if (*(int*)(self + 0x14c) >= 2) {
        int left = *(short*)(self + 0x144);
        int top = *(short*)(self + 0x146);
        int right = *(short*)(self + 0x148);
        int bottom = *(short*)(self + 0x14a);
        SetRectFromPosAndSize((struct Rect020e2d2c*)(self + 0x1988), left + 2, top + 2, right - 4, bottom - 4);
        if (*(unsigned char*)(self + 0x1962) != 0) {
            _Z23SetEntryEnabled020e2cc4P13Entry020e2cc4i((struct Entry020e2cc4*)(self + 0x1988), 1);
        }
    }

    int k;
    for (k = 0; k < 2; k++) {
        _Z26UpdateAndDrawEntry020e2c34P13Entry020e2c34((struct Entry020e2c34*)((char*)(self + 0x1964) + k * 0x24));
    }
}
