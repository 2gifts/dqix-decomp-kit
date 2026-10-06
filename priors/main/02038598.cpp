#include <globaldefs.h>
#include "Combat/Main/BattleList.h"
struct BattleStruct {
    int unk0;
    int unk4;
    struct CombatantStruct* combatantList[0xe9];
};
struct CombatantStruct {
    unsigned short flags;
    char unk[0x132];
    struct BaseCombatStats* baseStats;
    struct ModifiableCombatStats* currentStats;
};
extern "C" struct BattleStruct* _ZN9GameState11GetInstanceEv();

extern "C" int func_ov017_0218b5b0(void);
extern "C" void* func_02012fe4(void);
void* GetDataPtr02114e04_020d6c00(void);

struct Vec3 { int x; int y; int z; };
void SubtractVec3(struct Vec3* a, struct Vec3* b, struct Vec3* out);
void CopyVec3(int* dst, int* src);

extern "C" void func_020c2f18(struct Vec3* out, struct Vec3* in);
extern "C" int func_020c338c(int x, int z);
extern "C" int func_02030f30(int angle);
extern "C" int func_02036e34(void* obj, char* data, int mode);
extern "C" void func_ov017_021d3f00(void* obj, int b, int c);
extern "C" void func_ov017_02191108(void* unused, int c, int d, int e, int flag);

struct Obj02033834;
void SetVecYByMode02033834(struct Obj02033834* obj, int arg);

struct Obj020397cc;
void CancelPendingAction020397cc(struct Obj020397cc* obj, int arg1);

void SetBitsInField0x6c(unsigned char* obj, unsigned int mask);
void ClearBitsInField0x6c(unsigned char* obj, unsigned int mask);
void OrBitsIntoField0(unsigned int* p, unsigned int mask);

struct Bytes02033b88;
int SetByte0xbeShiftPrev(struct Bytes02033b88* p, int val);

int AbsInt(int x);

struct BitFlags02037170;
int GetByte0x40Bit0(struct BitFlags02037170* obj);

struct Self02034e38;
void DispatchSelectedMember02034e38(struct Self02034e38* self);

struct Vec3038500 { int x; int y; int z; };
void SetVec3038500(struct Vec3038500* obj, int x, int y, int z);

struct FlagWord020466f4 { unsigned int flags; };
void ClearFlags020466f4(struct FlagWord020466f4* word, unsigned int mask);

struct Obj02033874;
void SetVecYFromValue02033874(struct Obj02033874* obj, int arg);

extern "C" int GetField4328_0218d268(void* obj);

int IsFlag0x80Set(unsigned short* flags);
int IsFlag0x40Set(unsigned short* flags);

int Vec3LengthRounded(int* v);

struct ListNode02046b60;
struct ListHead02046b60;
int ListContainsId(struct ListHead02046b60* list, int id);

int HwDivideRounded020c2bf4(unsigned int numerHi, unsigned int denomLo);

struct Vec3_37774 { int a[3]; };
struct Src_37774;
void BuildVec3FromScatteredFields(struct Vec3_37774* dst, struct Src_37774* src);

extern "C" unsigned int _ZNK9GameState12GetTickCountEv(struct BattleStruct* battleStruct);

struct S1a0;
void ShiftField0x1cInto0x20(struct S1a0* obj, unsigned int v);

void SetHalf0xc6AndCopyVec3(void* obj, int* src, short val);

struct Foo02033b58;
void SetByteSavingPrevious(struct Foo02033b58* p, unsigned char v);

struct ShortField02033ec8;
int CheckField0xc6Zero(struct ShortField02033ec8* p);

struct Vec3Block02038508 { int v[3]; };
struct SubBlock02038508 {
    unsigned char a, b, c, d;    // 0x0-0x3
    unsigned short e;            // 0x4
    short f, g, h;               // 0x6, 0x8, 0xa
    struct Vec3Block02038508 v0; // 0xc
    struct Vec3Block02038508 v1; // 0x18
    struct Vec3Block02038508 v2; // 0x24
    struct Vec3Block02038508 v3; // 0x30
    struct Vec3Block02038508 v4; // 0x3c
    int field48;                 // 0x48
    int field4c;                 // 0x4c
    short field50;                // 0x50
    unsigned char field52;        // 0x52
    unsigned char field53;        // 0x53
    unsigned char field54;        // 0x54
    unsigned char* field58;       // 0x58
    unsigned char* field5c;       // 0x5c
    unsigned char* field60;       // 0x60
};
void InitBlock02038508(struct SubBlock02038508* p);

extern "C" struct CombatantStruct* _ZN9GameState20GetUnknownGameObjectEv(struct BattleStruct* battleStruct);

struct Struct020372b8;
void ScaleColorChannel020372b8(struct Struct020372b8* obj, int a, int b);

extern "C" void* _Z20GetField0x3f8AddressP9GameState(struct BattleStruct* battleStruct);
extern "C" int _Z18GetField0x3acValueP9GameState(struct BattleStruct* battleStruct);

extern "C" void VectorizedMemset(void*, int, int);
extern "C" void* _Z28CallFunc0200fbb4AtField0x3f8Pv(void* obj, void* event);

void InitAndResetHeader_0219e310(unsigned char* obj, int flag);

struct TailList020469b4;
struct TailNode020469b4;
void AppendNodeToTail(struct TailList020469b4* list, struct TailNode020469b4* node);

extern "C" unsigned int _fflt(int);
extern "C" unsigned int _ffltu(unsigned int);
extern "C" unsigned int _fdiv(unsigned int, unsigned int);
extern "C" unsigned int _fmul(unsigned int, unsigned int);
extern "C" int _ffix(unsigned int);

extern char data_020efb44;
extern char data_020efb4e;
extern unsigned short data_02114e30;
extern char data_02114e54[0x44];
extern char data_020efb5a;

// SCRATCH-USA: func_02038598
extern "C" ARM void func_02038598(unsigned char* self) {
    int base = func_ov017_0218b5b0();
    void* list = *(void**)(base + 0x3000 + 0x6fc);
    void* g = func_02012fe4();
    struct SubBlock02038508* blk = (struct SubBlock02038508*)(self + 0x26c);
    unsigned int* word = (unsigned int*)GetDataPtr02114e04_020d6c00();

    if (*(short*)(self + 4) != *(signed char*)(self + 0x1ca)) goto Exit;
    if (blk->a == 0) goto Exit;

    unsigned char state = blk->b;

    if (state == 0) {
        unsigned char* post = (blk->d == 0) ? blk->field58 : blk->field5c;
        struct Vec3 tmp;
        SubtractVec3((struct Vec3*)(post + 8), (struct Vec3*)(self + 0x44), &tmp);
        func_020c2f18(&tmp, &tmp);
        int angle = func_02030f30(func_020c338c(tmp.x, tmp.z));
        SetVecYByMode02033834((struct Obj02033834*)self, angle);
        CancelPendingAction020397cc((struct Obj020397cc*)self, 1);
        self[0xe0] = (self[0xe0] & ~1) | 1;
        SetBitsInField0x6c(self, 0x180);
        SetByte0xbeShiftPrev((struct Bytes02033b88*)self, 0);
        blk->field4c = *(int*)(blk->field58 + 0xc);
        blk->field50 = *(int*)((char*)g + 0x3c);
        if (AbsInt(*(short*)(self + 0xae)) - angle >= 0x333) goto Exit;
        func_ov017_021d3f00(blk, 0, 0);
        blk->b = 1;
        CopyVec3(blk->v2.v, (int*)(self + 0x44));
        if (blk->d == 0) {
            func_02036e34(self, &data_020efb44, 1);
            goto Exit;
        }
        if (blk->d != 1) goto Exit;
        func_02036e34(self, ((__typeof__(&data_020efb44))0x020EFB44), 1);
        goto Exit;
    }

    if (state == 1) {
        SetVecYByMode02033834((struct Obj02033834*)self, blk->f);
        CancelPendingAction020397cc((struct Obj020397cc*)self, 1);
        int field24 = *(int*)(self + 0x24);

        if (blk->d == 0) {
            if (!GetByte0x40Bit0((struct BitFlags02037170*)self)) {
                DispatchSelectedMember02034e38((struct Self02034e38*)self);
            }
            if (!GetByte0x40Bit0((struct BitFlags02037170*)self)) {
                DispatchSelectedMember02034e38((struct Self02034e38*)self);
            }
        }

        if (blk->d == 1) {
            int tick = *(int*)((char*)g + 0x3c);
            unsigned int fA = _fdiv(_fflt(field24), 0x45800000u);
            unsigned int fB = _ffltu((unsigned int)tick);
            int fixResult = _ffix(_fmul(fA, fB));
            blk->field50 = tick - (fixResult << 2);
            if (blk->field50 < 0) blk->field50 = 0;
        }

        struct Vec3038500 tmpVec;
        struct Vec3038500 newPos;
        if (blk->d == 0) {
            SetVec3038500(&tmpVec, blk->v0.v[0], blk->v0.v[1] + 0x51, blk->v0.v[2]);
        } else if (blk->d == 1) {
            SetVec3038500(&tmpVec, blk->v1.v[0], blk->v1.v[1] - 0xa3, blk->v1.v[2]);
        }

        if (!GetByte0x40Bit0((struct BitFlags02037170*)self)) {
            int dz = tmpVec.z - blk->v2.v[2];
            int dy = tmpVec.y - blk->v2.v[1];
            int dx = tmpVec.x - blk->v2.v[0];
            int rz = (int)(((long long)field24 * dz + 0x800) >> 12);
            int ry = (int)(((long long)dy * field24 + 0x800) >> 12);
            int rx = (int)(((long long)dx * field24 + 0x800) >> 12);
            SetVec3038500(&newPos, blk->v2.v[0] + rx, blk->v2.v[1] + ry, blk->v2.v[2] + rz);
            CopyVec3((int*)(self + 0x44), (int*)&newPos);
        }

        if (GetByte0x40Bit0((struct BitFlags02037170*)self)) {
            SetVec3038500(&newPos, tmpVec.x, tmpVec.y, tmpVec.z);
            CopyVec3((int*)(self + 0x44), (int*)&newPos);
            blk->b = 2;
            func_02036e34(self, &data_020efb4e, 0);
            goto Exit;
        }

        func_ov017_021d3f00(blk, 0, 0);
        goto Exit;
    }

    if (state == 2) {
        struct Vec3_37774 evPos;
        struct Vec3 tmpA, tmpB;

        ClearFlags020466f4((struct FlagWord020466f4*)word, 0x400);
        SetVecYFromValue02033874((struct Obj02033874*)self, blk->f);
        CancelPendingAction020397cc((struct Obj020397cc*)self, 1);

        struct Vec3 posCopy = *(struct Vec3*)(self + 0x44);

        int base3 = func_ov017_0218b5b0();
        unsigned char* sl_pt = (unsigned char*)GetField4328_0218d268((void*)base3);

        int side = 0;
        if (IsFlag0x80Set(&data_02114e30)) {
            side = 1;
        } else if (IsFlag0x40Set(&data_02114e30)) {
            side = 2;
        }

        if (*(unsigned int*)((char*)data_02114e54 + 0x40) > 5) {
            int v222 = sl_pt[0x222];
            int v220 = sl_pt[0x220];
            int v221 = sl_pt[0x221];
            int v223 = sl_pt[0x223];
            tmpA.x = v222 << 12;
            tmpA.y = v223 << 12;
            tmpA.z = 0;
            tmpB.x = v220 << 12;
            tmpB.y = v221 << 12;
            tmpB.z = 0;
            SubtractVec3(&tmpA, &tmpB, &tmpA);
            if (Vec3LengthRounded((int*)&tmpA) >= 0x8000) {
                func_020c2f18(&tmpA, &tmpA);
                if (tmpA.x >= -1638 && tmpA.x <= 1638) {
                    if (tmpA.y >= 0)
                        side = 1;
                    else
                        side = 2;
                }
            }
        }

        int sl = 0;
        if (ListContainsId((struct ListHead02046b60*)list, 4) ||
            ListContainsId((struct ListHead02046b60*)list, 0x26) ||
            ListContainsId((struct ListHead02046b60*)list, 1) ||
            ListContainsId((struct ListHead02046b60*)list, 3) ||
            ListContainsId((struct ListHead02046b60*)list, 0x3e)) {
            sl = 1;
            side = 0;
        }

        if (blk->field53 == 0) side = 0;

        if (side == 1) {
            posCopy.y -= 0xf5;
        } else if (side == 2) {
            posCopy.y += 0xf5;
        }

        int t = HwDivideRounded020c2bf4(posCopy.y - blk->v0.v[1], blk->v1.v[1] - blk->v0.v[1]);
        int dz = blk->v1.v[2] - blk->v0.v[2];
        int dx = blk->v1.v[0] - blk->v0.v[0];
        int newZ = blk->v0.v[2] + (int)(((long long)dz * t + 0x800) >> 12);
        int newX = blk->v0.v[0] + (int)(((long long)dx * t + 0x800) >> 12);
        struct Vec3038500 newPos;
        SetVec3038500(&newPos, newX, posCopy.y, newZ);
        CopyVec3((int*)(self + 0x44), (int*)&newPos);

        BuildVec3FromScatteredFields(&evPos, (struct Src_37774*)self);

        int diffZ = *(int*)((char*)&evPos + 8) - blk->field4c;
        int fixedT = (int)(((long long)diffZ * 0x14000u + 0x800) >> 12);
        unsigned int fA2 = _fdiv(_fflt(fixedT), 0x45800000u);
        int t2 = _ffix(fA2);
        int tick2 = *(int*)((char*)g + 0x3c);
        blk->field50 = tick2 - t2;
        if (blk->field50 < 0) blk->field50 = 0;

        if (blk->field58[0x31] & 2) {
            blk->field50 = 0;
        }

        blk->e = (unsigned short)(blk->e + _ZNK9GameState12GetTickCountEv(_ZN9GameState11GetInstanceEv()));

        if (side == 1) {
            ClearBitsInField0x6c(self, 0x1000);
            func_02036e34(self, &data_020efb4e, 0);
            if (blk->field54 != side || blk->e > 6) {
                func_ov017_021d3f00(blk, side, 0);
                blk->e = 0;
            }
            if (blk->field48 != 0) {
                ShiftField0x1cInto0x20((struct S1a0*)self, 0);
                blk->field48 = 0;
            }
        } else if (side == 2) {
            ClearBitsInField0x6c(self, 0x1000);
            func_02036e34(self, &data_020efb4e, 4);
            if (blk->field54 != side || blk->e > 6) {
                func_ov017_021d3f00(blk, side, 0);
                blk->e = 0;
            }
            if (blk->field48 != 0) {
                ShiftField0x1cInto0x20((struct S1a0*)self, 0);
                blk->field48 = 0;
            }
        } else {
            SetBitsInField0x6c(self, 0x1000);
            blk->field48 = *(int*)(self + 0x1c);
            if (blk->field54 != side || blk->e > 6) {
                func_ov017_021d3f00(blk, side, 0);
                blk->e = 0;
            }
        }
        blk->field54 = (unsigned char)side;

        if (sl != 0) goto Exit;

        if (posCopy.y < blk->v0.v[1]) {
            OrBitsIntoField0(word, 0x400);
            unsigned char* post = blk->field58;
            if (post[0x31] & 2) {
                blk->field60 = post;
                blk->b = 6;
                goto Exit;
            }
            blk->b = 5;
            CopyVec3(blk->v2.v, blk->v3.v);
            SetHalf0xc6AndCopyVec3(self, blk->v3.v, *(short*)(self + 0xb4));
            SetByte0xbeShiftPrev((struct Bytes02033b88*)self, 1);
            SetVecYByMode02033834((struct Obj02033834*)self, blk->g);
            func_ov017_021d3f00(blk, 0, 0);
            goto Exit;
        }

        if (posCopy.y <= blk->v1.v[1]) goto Exit;

        OrBitsIntoField0(word, 0x400);
        unsigned char* post2 = blk->field5c;
        if (post2[0x31] & 2) {
            blk->field60 = post2;
            blk->b = 6;
            goto Exit;
        }
        posCopy.y = blk->v1.v[1];
        CopyVec3((int*)(self + 0x44), (int*)&posCopy);
        CopyVec3(blk->v2.v, (int*)&posCopy);
        func_02036e34(self, &data_020efb5a, 1);
        blk->b = 4;
        func_ov017_021d3f00(blk, 0, 0);
        goto Exit;
    }

    if (state == 4) {
        CancelPendingAction020397cc((struct Obj020397cc*)self, 1);
        int r7v = *(int*)(self + 0x24);
        if (r7v != 0) {
            struct Vec3 tmp2;
            CopyVec3((int*)&tmp2, (int*)(self + 0x44));
            tmp2.y = blk->v2.v[1] + (int)(((long long)(blk->v4.v[1] - blk->v2.v[1]) * 0x199u + 0x800) >> 12);
            CopyVec3((int*)(self + 0x44), (int*)&tmp2);
            CopyVec3(blk->v2.v, (int*)&tmp2);

            int tick = *(int*)((char*)g + 0x3c);
            unsigned int fTick = _ffltu((unsigned int)tick);
            unsigned int fA = _fdiv(_fflt(r7v), 0x45800000u);
            unsigned int fB = _fdiv(_fflt(r7v), 0x45800000u);
            unsigned int fC = _fmul(fA, fB);
            unsigned int fD = _fmul(fTick, fC);
            blk->field50 = _ffix(fD);
            if (blk->field50 < 0) blk->field50 = 0;
        }

        if (GetByte0x40Bit0((struct BitFlags02037170*)self)) {
            blk->b = 5;
            CopyVec3(blk->v2.v, blk->v4.v);
            SetHalf0xc6AndCopyVec3(self, blk->v4.v, *(short*)(self + 0xb4));
            SetByte0xbeShiftPrev((struct Bytes02033b88*)self, 1);
            SetVecYByMode02033834((struct Obj02033834*)self, blk->h);
            func_ov017_021d3f00(blk, 0, 0);
            ClearFlags020466f4((struct FlagWord020466f4*)word, 0x400);
        }
        goto Exit;
    }

    if (state == 5) {
        unsigned char saved52 = blk->field52;
        CancelPendingAction020397cc((struct Obj020397cc*)self, 1);
        SetBitsInField0x6c(self, 0x80);
        SetByteSavingPrevious((struct Foo02033b58*)self, 1);
        if (!CheckField0xc6Zero((struct ShortField02033ec8*)self)) goto Exit;

        InitBlock02038508(blk);
        self[0x253] = 1;
        self[0xe0] = self[0xe0] & ~1;
        ClearBitsInField0x6c(self, 0x1180);
        func_ov017_021d3f00(blk, 0, 1);
        ClearFlags020466f4((struct FlagWord020466f4*)word, 0x400);
        if (saved52 == 0) goto Exit;

        self[0x252] = 1;
        func_ov017_02191108((void*)base, 1, 1, 1, 1);
        struct CombatantStruct* c = _ZN9GameState20GetUnknownGameObjectEv(_ZN9GameState11GetInstanceEv());
        if (c == 0) goto Exit;
        ScaleColorChannel020372b8((struct Struct020372b8*)c, 0x1f, 0x64);
        goto Exit;
    }

    if (state == 6) {
        struct BattleStruct* bs = _ZN9GameState11GetInstanceEv();
        unsigned char* event = (unsigned char*)_Z20GetField0x3f8AddressP9GameState(bs);
        int base2 = func_ov017_0218b5b0();
        _Z18GetField0x3acValueP9GameState(bs);
        VectorizedMemset(event, 0, 0x70);
        event[4] = 1;
        event[8] = 1;
        event[9] = 1;
        event[0xb] = (unsigned char)-1;
        *(int*)(event + 0x20) = -1;
        *(int*)(event + 0x24) = -1;
        *(int*)(event + 0x28) = -1;
        *(int*)(event + 0x2c) = -1;
        *(short*)(event + 0x1e) = -1;
        event[0xc] = 0;
        *(short*)(event + 0x6c) = -1;

        unsigned char* post = blk->field60;
        *(unsigned short*)(event + 0) = *(unsigned short*)(post + 0x32);
        CopyVec3((int*)(event + 0x10), (int*)(post + 0x38));
        *(short*)(event + 0x1c) = *(short*)(post + 0x68);
        event[0xb] = (unsigned char)*(short*)(post + 0x6a);
        CopyVec3((int*)(event + 0x30), (int*)(post + 0x38));
        CopyVec3((int*)(event + 0x3c), (int*)(post + 0x44));
        CopyVec3((int*)(event + 0x48), (int*)(post + 0x50));
        CopyVec3((int*)(event + 0x54), (int*)(post + 0x5c));
        event[0xc] = 1;

        if (post[0x31] & 4) {
            *(short*)(event + 0x6c) = *(short*)(post + 0x6e);
            *(short*)(event + 0x6a) = *(short*)(blk->field60 + 0x6c);
        }
        if (*(short*)(event + 0x6c) == -1) {
            *(short*)(event + 0x6c) = 9999;
        }
        event[7] = 1;
        _Z28CallFunc0200fbb4AtField0x3f8Pv(bs, event);

        void* node = *(void**)(base2 + 0x3000 + 0x70c);
        InitAndResetHeader_0219e310((unsigned char*)node, 0);
        void* listHead = *(void**)(base2 + 0x3000 + 0x6fc);
        AppendNodeToTail((struct TailList020469b4*)listHead, (struct TailNode020469b4*)node);
        ClearFlags020466f4((struct FlagWord020466f4*)word, 0x400);

        InitBlock02038508(blk);
        self[0x253] = 1;
        *(short*)(self + 0xb2) = 0;
        self[0xe0] = self[0xe0] & ~1;
        SetBitsInField0x6c(self, 0x1000);
        ClearBitsInField0x6c(self, 0x180);
        goto Exit;
    }
    Exit:;
}
