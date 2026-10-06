#include <globaldefs.h>

struct Vec3_020406f8 {
    int v[3];
};

struct SlotA020cf0fc {
    int f0;
    int f4;
    int f8;
    int fc;
};

struct SlotB020cf1a8 {
    int f0;
    int f4;
    int f8;
};

struct Name56_0218d7b0 {
    unsigned char f0;
    unsigned char f1;
    unsigned short f2;
    unsigned char f4;
    unsigned char f5;
};

union Packed0209c830 {
    unsigned int w;
    struct {
        unsigned int a : 7;
        unsigned int b : 4;
        unsigned int c : 5;
        unsigned int d : 5;
        unsigned int e : 6;
    } b;
};

extern "C" void _Z27SetEntryByte14ByKey0205a42cP17Container0205a3d0ii(void* c, int key, int val);
extern "C" void _Z22IterateEntries0205a330P17Container0205a330i(void* c, int step);
extern "C" void func_0206b104(void* obj);
extern "C" void _Z24ReinitController02043204Pc(void* obj);
extern "C" void _Z25CheckAndClearFlag020656e4Pv(void* obj, int step);
extern "C" int _Z28UpdateCounterAndFlag02065758P14Struct02065758i(void* obj, int step);
extern "C" void _Z24UpdateTimedState0202f380Pvi(void* obj, int step);
extern "C" int _Z15IsField440EmptyP7CE28Obj(void* obj);
extern "C" void _Z18SetupField440Type1P19Field440Obj0202f6a4(void* obj);
extern "C" void _Z25ProcessTextTokens0206b61cP11Ctx0206b61c(void* obj);
extern "C" int func_020657c8(void* obj, int step);
extern "C" void _Z17UpdateStatusTimerPhj(void* obj, unsigned int step);
extern "C" int _Z25IsAnimationActive0209ca2cPv(void* p);
extern "C" void _Z19StepCounter020456d0P14Battle020456d0(void* obj);
extern "C" int func_020440cc(void* obj);
extern "C" void _Z29ResetOrConsumeCounter02065920Phj(void* obj, unsigned int step);
extern "C" void _Z25HandleRepeatInput02045740P14Repeat02045740(void* obj);
extern "C" void func_020676e0(void* obj, int step);
extern "C" void _Z26ProcessPendingFlag02045688P13State02045688(void* obj);
extern "C" void _Z25HandleStateChange020455f8P13State020455f8(void* obj);
extern "C" void* _ZN9GameState11GetInstanceEv();
extern "C" int _ZNK9GameState12GetTickCountEv(void* bs);
extern "C" int _Z20IsHighByteFF02044494PvS_(void* obj, void* tok);
extern "C" int _Z16Dispatch02044420PvS_(void* obj, void* tok);
extern "C" void func_02043124(void* obj);
extern "C" unsigned char* func_ov017_0218b5b0();
extern "C" void _Z25SetName56AndFlag_0218d7b0PhPc(unsigned char* owner, char* name);
extern "C" void _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(void* obj, int a, int b);
extern "C" void func_ov016_0218b5c0(int a, signed char b);
extern "C" void func_ov017_0218b5f8(signed char a);
extern "C" int _Z14ListContainsIdP16ListHead02046b60i(void* list, int id);
extern "C" void _Z25CallHelperIfFlag_021a5ab0v(void* p);
extern "C" int _Z25TestFlag0SetAndFlag1ClearPti(void* p, int mask);
extern "C" void* _ZN9GameState20GetUnknownGameObjectEv(void* bs);
extern "C" void* _Z20GetGlobalPtr021075f4v();
extern "C" void* _Z24GetEntryUnlessFlag0x8000P17EntryList_203dce4i(void* list, int id);
extern "C" void _Z29SelectVec3FromSources020406f8P13Vec3_020406f8P12Node020406f8(struct Vec3_020406f8* out, void* node);
extern "C" int fix32_Atan2(int x, int z);
extern "C" int _Z22fix32ReduceAngle0To2Pii(int angle);
extern "C" void func_02065418(int a);
extern "C" int _Z28CallFunc020e0434With02153694i(int a);
extern "C" void _Z27InitCombatPairState020652d0Pciii(char* obj, int a, int b, int c);
extern "C" unsigned char* _Z10GetWord0x0Pi(void* bs);
extern "C" void _Z18SetFieldD20208bf28P11Obj0208bf28ih(void* obj, int a, unsigned char b);
extern "C" unsigned char* _Z17GetGlobal02109418v();
extern "C" int _Z16GetTableByteOrFFP20GetTableByteOrFFDataj(unsigned char* p, unsigned int i);
extern "C" void _Z21RegisterSlotA020cf0fci(struct SlotA020cf0fc* out);
extern "C" void _Z21RegisterSlotB020cf1a8i(struct SlotB020cf1a8* out);
extern "C" void _Z27SetValueAndActivate0209c830P14Struct0209c830t(void* p, int v);
extern "C" void _Z41ClearField0xd0AndSetFlag0x1IfByte0xd4Not2Ph(void* p);
extern "C" void _Z24SetFlag0x2IfByte0xd4Not3Ph(void* p);
extern "C" void func_0205e944(void* p, int v);

extern unsigned char data_02109bf4;
extern unsigned char data_02108760;
extern unsigned char data_02114e30;
extern unsigned char data_02114e54;

// USA: func_02065990
extern "C" ARM void func_02065990(unsigned char* self, int amount) {
    unsigned short tok;
    struct Name56_0218d7b0 nm;
    union Packed0209c830 packed;
    struct Vec3_020406f8 here;
    struct Vec3_020406f8 dst;
    struct SlotA020cf0fc sa;
    struct SlotB020cf1a8 sb;
    struct Vec3_020406f8 src;
    int step = amount;
    if (step == 0) step = 1;

    if (*(void**)(self + 0x2e0) != 0 && self[0x2e6] != 0 && *(int*)(self + 0x9a0) == 6) {
        if ((*(unsigned short*)0x4000304 & 0x8000) >> 15 == 1) {
            _Z27SetEntryByte14ByKey0205a42cP17Container0205a3d0ii(*(void**)(self + 0x2e0), 0, 0x1e);
        }
        _Z22IterateEntries0205a330P17Container0205a330i(*(void**)(self + 0x2e0), step);
    }

    if (*(int*)(self + 0x998) == 0) return;

    if ((self + 0x1000)[0x9d1] != 0) {
        (self + 0x1000)[0x958] = *(signed char*)(self + 0x1958) - step;
        if (*(signed char*)(self + 0x1958) >= 0) return;
        func_0206b104(self);
        _Z24ReinitController02043204Pc(self);
        return;
    }

    _Z25CheckAndClearFlag020656e4Pv(self, step);
    if (_Z28UpdateCounterAndFlag02065758P14Struct02065758i(self, step) != 0) return;

    if ((self + 0x1000)[0x9b1] != 0) {
        _Z24UpdateTimedState0202f380Pvi(self + 0x19e0, step);
        if (_Z15IsField440EmptyP7CE28Obj(self + 0x19e0) != 0) {
            _Z18SetupField440Type1P19Field440Obj0202f6a4(self + 0x19e0);
        }
        unsigned char* w = *(unsigned char**)(self + 0x1e20);
        int ready;
        if (w != 0) {
            ready = *(int*)(w + 8) == 2;
        } else {
            ready = 0;
        }
        if (ready == 0) return;
    }

    if ((self + 0x1000)[0x9ce] != 0) *(int*)(self + 0x44) = *(int*)(self + 0x78);

    if ((self + 0x1000)[0x9b4] != 0) {
        *(int*)(self + 0x1868) = *(int*)(self + 0x1868) + step;
        if (*(int*)(self + 0x1868) < *(int*)(self + 0x1864)) return;
        *(int*)(self + 0x1868) = 0;
        _Z25ProcessTextTokens0206b61cP11Ctx0206b61c(self);
        (self + 0x1000)[0x9b4] = 0;
        *(int*)(self + 0x9a0) = 5;
        (self + 0x1000)[0x9b5] = 0;
        return;
    }

    if (func_020657c8(self, step) != 0) return;
    _Z17UpdateStatusTimerPhj(self, step);

    if (((self + 0x1000)[0x95b] & 1) != 0) {
        if (_Z25IsAnimationActive0209ca2cPv(&data_02109bf4) == 0) {
            (self + 0x1000)[0x95b] = (self + 0x1000)[0x95b] & ~1;
        }
    } else {
        if (*(unsigned char**)(self + 0x8c) != 0 && (*(unsigned char**)(self + 0x8c))[0xd4] == 2) return;

        if ((self + 0x1000)[0x9c9] != 0) {
            if (_Z25IsAnimationActive0209ca2cPv(&data_02109bf4) != 0) return;
            *(int*)(self + 0x78) = *(int*)(self + 0x78) + 1;
            (self + 0x1000)[0x9c9] = 0;
            return;
        }

        _Z19StepCounter020456d0P14Battle020456d0(self);
        if (func_020440cc(self) != 0) {
            (self + 0x1000)[0x9d1] = 1;
            (self + 0x1000)[0x958] = 5;
            return;
        }

        if (*(int*)(self + 0x9a0) != 5) {
            _Z29ResetOrConsumeCounter02065920Phj(self, step);
            if ((self + 0x1000)[0x9bc] != 0) {
                _Z25HandleRepeatInput02045740P14Repeat02045740(self);
            }
            func_020676e0(self, step);
            _Z26ProcessPendingFlag02045688P13State02045688(self);
            return;
        }
    }

    if (*(short*)(self + 0x1946) < 0 && (self + 0x1000)[0x9c8] == 0) {
        *(signed char*)(self + 0x912) = 0;
        _Z26ProcessPendingFlag02045688P13State02045688(self);
    } else if (*(unsigned int*)(self + 0x88) < *(unsigned int*)(self + 0x78)) {
        if (*(signed char*)(self + 0x912) >= 4) {
            *(signed char*)(self + 0x912) = *(signed char*)(self + 0x912) - 4;
            _Z26ProcessPendingFlag02045688P13State02045688(self);
            _Z25HandleStateChange020455f8P13State020455f8(self);
        }
        *(signed char*)(self + 0x912) =
            *(signed char*)(self + 0x912) + _ZNK9GameState12GetTickCountEv(_ZN9GameState11GetInstanceEv());
    }

    if ((self + 0x1000)[0x9c5] != 0) step = 0x10;
    if (((self + 0x1000)[0x95b] & 4) != 0 && *(int*)(self + 0x9a0) != 0) step = 0x18;

    int prev = *(int*)(self + 0x80);
    int cur = prev + step * 16;
    int n = (cur >> 4) - (prev >> 4);
    *(int*)(self + 0x80) = cur;
    if (cur >= 0xffffff) *(int*)(self + 0x80) = 0;

    while (n != 0) {
        if (*(unsigned int*)(self + 0x6c) <= *(unsigned int*)(self + 0x78)) return;
        tok = ((unsigned short*)*(void**)(self + 0x4c))[*(unsigned int*)(self + 0x78)];
        if (_Z20IsHighByteFF02044494PvS_(self, &tok) != 0) {
            int adv = _Z16Dispatch02044420PvS_(self, &tok);
            if (adv != 0) {
                *(int*)(self + 0x78) = *(int*)(self + 0x78) + adv + 1;
                continue;
            }
            if (tok == 0xff01) {
                if (((self + 0x1000)[0x95b] & 4) != 0) {
                    (self + 0x1000)[0x95b] = (self + 0x1000)[0x95b] | 8;
                }
                *(int*)(self + 0x9a0) = 0;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                return;
            } else if (tok == 0xff02) {
                *(int*)(self + 0x9a0) = 0;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                return;
            } else if (tok == 0xff03) {
                *(int*)(self + 0x9a0) = 0;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                _Z24ReinitController02043204Pc(self);
                func_02043124(self);
                return;
            } else if (tok == 0xff0b) {
                *(int*)(self + 0x9a0) = 4;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                (self + 0x1000)[0x95f] = (self + 0x1000)[0x95e];
                *(signed char*)(self + 0x912) = 0;
                if (*(short*)(self + 0x19da) >= 0) {
                    (self + 0x1000)[0x95f] = (unsigned char)*(short*)(self + 0x19da);
                    *(short*)(self + 0x19da) = -1;
                }
                return;
            } else if (tok == 0xff0c) {
                (self + 0x1000)[0x961] = (self + 0x1000)[0x960];
                *(int*)(self + 0x9a0) = 1;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                return;
            } else if (tok == 0xff0d) {
                *(int*)(self + 0x9a0) = 1;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                return;
            } else if (tok == 0xff0a) {
                *(int*)(self + 0x9a0) = 3;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                return;
            } else if (tok == 0xff1b) {
                nm.f0 = 0;
                nm.f1 = 0;
                nm.f2 = 0;
                nm.f4 = 0;
                nm.f5 = 0;
                nm.f0 = (self + 0x1000)[0x9c6] == 0;
                nm.f1 = (self + 0x1000)[0x9c7] == 0;
                nm.f2 = *(unsigned short*)(self + 0x187e);
                nm.f4 = 1;
                _Z25SetName56AndFlag_0218d7b0PhPc(func_ov017_0218b5b0(), (char*)&nm);
                *(int*)(self + 0x78) = *(int*)(self + 0x78) + 1;
                return;
            } else if (tok == 0xff1c) {
                int slot = *(signed char*)(self + 0x1957);
                if (slot >= 0 && slot < *(signed char*)(self + 0x1956)) {
                    int val;
                    int kind = *(signed char*)(self + 0x1952 + slot);
                    val = *(signed char*)(self + 0x194e + slot);
                    if (kind > 0) {
                        _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(&data_02108760, 0x65, 0);
                        if (kind == 2) {
                            val = -1;
                            func_ov016_0218b5c0(1, val);
                            func_ov017_0218b5f8(val);
                        }
                    }
                    if (val >= 0xa && val <= 0x32) {
                        int tens = val / 0xa - 1;
                        int okt = tens >= 0 && tens <= 3;
                        if (okt) {
                            func_ov016_0218b5c0(1, tens);
                            func_ov017_0218b5f8(tens);
                        }
                        val = val - (tens + 1) * 0xa;
                    }
                    int okv = val >= 0 && val <= 3;
                    if (okv) {
                        func_ov016_0218b5c0(1, val);
                        func_ov017_0218b5f8(val);
                        unsigned char* who = func_ov017_0218b5b0();
                        if (_Z14ListContainsIdP16ListHead02046b60i(*(void**)(who + 0x36fc), 0xc) != 0) {
                            _Z25CallHelperIfFlag_021a5ab0v(*(void**)(who + 0x371c));
                        }
                    }
                }
                (self + 0x1000)[0x957] = *(signed char*)(self + 0x1957) + 1;
                *(int*)(self + 0x78) = *(int*)(self + 0x78) + 1;
                return;
            } else if (tok == 0xff1a) {
                if ((self + 0x1000)[0x9d0] != 0) return;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                ((int*)(self + 0x186c))[*(signed char*)(self + 0x19ac)] =
                    ((int*)(self + 0x186c))[*(signed char*)(self + 0x19ac)] - 1;
                if (((int*)(self + 0x186c))[*(signed char*)(self + 0x19ac)] > 0) return;
                ((int*)(self + 0x186c))[*(signed char*)(self + 0x19ac)] = 0;
                *(int*)(self + 0x78) = *(int*)(self + 0x78) + 1;
                (self + 0x1000)[0x9ac] = *(signed char*)(self + 0x19ac) + 1;
                (self + 0x1000)[0x9ac] = *(signed char*)(self + 0x19ac) & 3;
                *(signed char*)(self + 0x912) = 0;
                return;
            } else if (tok == 0xff1d) {
                *(int*)(self + 0x9a0) = 2;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                return;
            } else if (tok == 0xff1e) {
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                ((int*)(self + 0x186c))[*(signed char*)(self + 0x19ac)] =
                    ((int*)(self + 0x186c))[*(signed char*)(self + 0x19ac)] - 1;
                if (((int*)(self + 0x186c))[*(signed char*)(self + 0x19ac)] > 0) {
                    if (_Z25TestFlag0SetAndFlag1ClearPti(&data_02114e30, 0xfe3) == 0 &&
                        (&data_02114e54)[0x55] == 0) {
                        return;
                    }
                }
                ((int*)(self + 0x186c))[*(signed char*)(self + 0x19ac)] = 0;
                *(int*)(self + 0x78) = *(int*)(self + 0x78) + 1;
                (self + 0x1000)[0x9ac] = *(signed char*)(self + 0x19ac) + 1;
                (self + 0x1000)[0x9ac] = *(signed char*)(self + 0x19ac) & 3;
                *(signed char*)(self + 0x912) = 0;
                return;
            } else if (tok == 0xff1f) {
                (self + 0x1000)[0x9ca] = 0;
                *(int*)(self + 0x9a0) = 2;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                return;
            } else if (tok == 0xff26) {
                (self + 0x1000)[0x9b1] = 1;
                (self + 0x1000)[0x9cd] = 1;
            } else if (tok == 0xff27) {
                (self + 0x1000)[0x9b1] = 0;
                (self + 0x1000)[0x9cd] = 1;
            } else if (tok == 0xff28) {
                (self + 0x1000)[0x9b8] = 1;
            } else if (tok == 0xff29) {
                (self + 0x1000)[0x9b8] = 0;
            } else if (tok == 0xff2a) {
                *(int*)(self + 0x1844) = *(int*)(self + 0x1840);
                (self + 0x1000)[0x9b6] = 1;
                *(int*)(self + 0x78) = *(int*)(self + 0x78) + 1;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                return;
            } else if (tok == 0xff2c) {
                *(int*)(self + 0x1844) = *(int*)(self + 0x183c);
                (self + 0x1000)[0x9b6] = 1;
                *(int*)(self + 0x78) = *(int*)(self + 0x78) + 1;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                return;
            } else if (tok == 0xff2d) {
                unsigned char* who = (unsigned char*)_ZN9GameState20GetUnknownGameObjectEv(_ZN9GameState11GetInstanceEv());
                void* node = _Z24GetEntryUnlessFlag0x8000P17EntryList_203dce4i(
                    _Z20GetGlobalPtr021075f4v(), *(int*)(self + 0x1838));
                if (who != 0 && node != 0) {
                    here = *(struct Vec3_020406f8*)(who + 0x44);
                    _Z29SelectVec3FromSources020406f8P13Vec3_020406f8P12Node020406f8(&src, node);
                    dst = src;
                    *(int*)(self + 0x1844) = _Z22fix32ReduceAngle0To2Pii(fix32_Atan2(here.v[0] - dst.v[0], here.v[2] - dst.v[2]));
                    (self + 0x1000)[0x9b6] = 1;
                    _Z26ProcessPendingFlag02045688P13State02045688(self);
                }
                *(int*)(self + 0x78) = *(int*)(self + 0x78) + 1;
                return;
            } else if (tok == 0xff2e) {
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                (self + 0x1000)[0x959] = 0x3c;
                (self + 0x1000)[0x9b7] = 1;
                (self + 0x1000)[0x95c] = 0;
                *(int*)(self + 0x78) = *(int*)(self + 0x78) + 1;
                *(signed char*)(self + 0x912) = 0;
                _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(&data_02108760, 6, 0);
                return;
            } else if (tok == 0xff2f) {
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                (self + 0x1000)[0x959] = 0x3c;
                (self + 0x1000)[0x9b7] = 1;
                (self + 0x1000)[0x95c] = 1;
                *(int*)(self + 0x78) = *(int*)(self + 0x78) + 1;
                *(signed char*)(self + 0x912) = 0;
                _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(&data_02108760, 0x1c, 0);
                return;
            } else if (tok == 0xff04 || tok == 0xff05 || tok == 0xff06 || tok == 0xff07) {
                func_02065418((self + 0x1000)[0x9cc]);
                int first = 0;
                if (*(int*)(self + 0x2c8) == 1 || tok == 0xff05) first = 1;
                *(int*)(self + 0x2c8) = 0;
                int a = _Z28CallFunc020e0434With02153694i(0x1b);
                int b = _Z28CallFunc020e0434With02153694i(0x1c);
                _Z27InitCombatPairState020652d0Pciii((char*)(self + 0x90), first, a, b);
                *(int*)(self + 0x954) = first;
                (self + 0x1000)[0x9ba] = 1;
                *(int*)(self + 0x98c) = 0;
                *(int*)(self + 0x9a0) = 6;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(&data_02108760, 5, 0);
                int alive = 1;
                unsigned char* bs = (unsigned char*)_ZN9GameState11GetInstanceEv();
                unsigned char* w = _Z10GetWord0x0Pi(bs);
                if ((bs + 0x5000)[0xcc8] == 2) alive = 0;
                if (w != 0 && alive != 0) (w + 0x4000)[0x354] = 0xa;
                return;
            } else if (tok == 0xff08) {
                volatile unsigned int* reg = (volatile unsigned int*)0x4000000;
                unsigned int bits = (*reg & 0x1f00) >> 8;
                unsigned int keep = *reg & ~0x1f00;
                *reg = keep | ((bits | 0x10) << 8);
                func_02065418((self + 0x1000)[0x9cc]);
                int a = _Z28CallFunc020e0434With02153694i(0x1d);
                int b = _Z28CallFunc020e0434With02153694i(0x1e);
                _Z27InitCombatPairState020652d0Pciii((char*)(self + 0x90), 0, a, b);
                *(int*)(self + 0x954) = 0;
                (self + 0x1000)[0x9ba] = 1;
                *(int*)(self + 0x98c) = 1;
                *(int*)(self + 0x9a0) = 6;
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(&data_02108760, 5, 0);
                unsigned char* w = _Z10GetWord0x0Pi(_ZN9GameState11GetInstanceEv());
                if (w != 0) (w + 0x4000)[0x354] = 0xa;
                return;
            } else if (tok == 0xff0f) {
                unsigned char* ent = *(unsigned char**)(self + 0x8c);
                if (ent != 0) {
                    *(unsigned short*)(ent + 0xda) = *(unsigned short*)(ent + 0xda) | 0x40;
                    _Z18SetFieldD20208bf28P11Obj0208bf28ih(*(void**)(self + 0x8c), *(short*)(self + 0x1948), 0);
                    *(self + 0x195b) |= 1;
                    unsigned char* g = _Z17GetGlobal02109418v();
                    int idx = _Z16GetTableByteOrFFP20GetTableByteOrFFDataj(g, (unsigned char)*(short*)(self + 0x1948));
                    _Z21RegisterSlotA020cf0fci(&sa);
                    _Z21RegisterSlotB020cf1a8i(&sb);
                    packed.b.a = sa.f0;
                    packed.b.b = sa.f4;
                    packed.b.c = sa.f8;
                    packed.b.d = sb.f0;
                    packed.b.e = sb.f4;
                    *(unsigned int*)(g + idx * 4 + 0x178) = packed.w;
                    _Z27SetValueAndActivate0209c830P14Struct0209c830t(&data_02109bf4, 0x32);
                }
            } else if (tok > 0xff34 && tok < 0xff49) {
                _Z26ProcessPendingFlag02045688P13State02045688(self);
                _Z27SetValueAndActivate0209c830P14Struct0209c830t(&data_02109bf4, tok - 0xff03);
                (self + 0x1000)[0x9c9] = 1;
                return;
            } else if (tok >= 0xff4a && tok <= 0xff4c) {
                int arg = -1;
                if (tok == 0xff4b) arg = 0xe;
                if (arg >= 0) {
                    _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(&data_02108760, 0xe, 0);
                }
            } else if (tok == 0xff10) {
                unsigned char* ent = *(unsigned char**)(self + 0x8c);
                if (ent != 0) *(short*)(ent + 0xd2) = *(short*)(self + 0x1948);
            } else if (tok == 0xff11) {
                unsigned char* ent = *(unsigned char**)(self + 0x8c);
                if (ent != 0) {
                    *(unsigned short*)(ent + 0xda) = *(unsigned short*)(ent + 0xda) | 0x200;
                    _Z41ClearField0xd0AndSetFlag0x1IfByte0xd4Not2Ph(*(void**)(self + 0x8c));
                    *(self + 0x195b) |= 1;
                    _Z27SetValueAndActivate0209c830P14Struct0209c830t(&data_02109bf4, 0x33);
                }
            } else if (tok == 0xff13) {
                unsigned char* ent = *(unsigned char**)(self + 0x8c);
                if (ent != 0) {
                    *(unsigned short*)(ent + 0xda) = *(unsigned short*)(ent + 0xda) | 0x300;
                    _Z41ClearField0xd0AndSetFlag0x1IfByte0xd4Not2Ph(*(void**)(self + 0x8c));
                    *(self + 0x195b) |= 1;
                    _Z27SetValueAndActivate0209c830P14Struct0209c830t(&data_02109bf4, 0x44);
                }
            } else if (tok == 0xff12) {
                if (*(void**)(self + 0x8c) != 0) {
                    _Z24SetFlag0x2IfByte0xd4Not3Ph(*(void**)(self + 0x8c));
                }
            } else if (tok >= 0xff4d && tok <= 0xff50) {
                int v = *(signed char*)(self + 0x194a + (tok - 0xff4d));
                func_0205e944(&data_02108760, ((v * 0x21dd) >> 10) & 0x7f);
            } else if (tok == 0xff0e) {
                (self + 0x1000)[0x9c0] = 1;
                (self + 0x1000)[0x95d] = 0x1e;
            }
        }
        *(int*)(self + 0x9a0) = 5;
        *(int*)(self + 0x78) = *(int*)(self + 0x78) + 1;
        n--;
    }
}
