#include <globaldefs.h>
#include "Combat/Main/BattleList.h"
#include "Grotto/Main/GrottoStruct.h"
#include "Resource/GameResources.h"
#include "System/Memory.h"
#include "Memory/AllocatorUnion.h"
#include "std_library_functions.h"
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
extern "C" void* _Z17GetPtrField0x2a04P9GameState(struct BattleStruct* battleStruct);
extern "C" struct CombatantStruct* _Z25GetCombatantWithFlag0x100P9GameStatei(struct BattleStruct* battleStruct, int combatantId);
extern "C" struct BattleStruct* _ZN9GameState11GetInstanceEv();
extern "C" GrottoStruct* _ZN9GameState15GetGrottoStructEv(BattleStruct* battle);

extern "C" void* func_02012fe4(void);
extern "C" void* func_0202ae18(void);
extern "C" void* func_0205ec34(void);
extern "C" int func_0202c540(void* p);
extern "C" int func_0202c508(void* p);
extern "C" int func_01ff85b8(void* buf, int size);
extern "C" int func_02075acc(int id, void* buf, int size, int flag);
extern "C" void func_020830cc(void* field150, void* b);
extern "C" void func_020a0c0c(void);
extern "C" void func_020a0cc4(unsigned int a);
void EnqueueEventTag23Field_021d0d58(void);

void* GetGlobal02109418(void);
int CheckField0NonZero(int* obj);
int TestBitInByteArray(int, unsigned char*, int);
extern "C" float _ZNK9GameState11GetDayTimerEv(struct BattleStruct*);
extern "C" int _Z18GetField0x3acValueP9GameState(struct BattleStruct*);
int GetFieldAt0x150(unsigned char*);
void* GetPointerFromArray0xbd0(unsigned char*, unsigned int);
void* GetPointerAt0xbf0(void*, unsigned int);
signed short GetShortFromArray0xc10(unsigned char*, unsigned int);
unsigned char GetByteFromArray0xc28(unsigned char*, unsigned int);
void CopyVec3(int*, int*);
void TailForward02012da4(AllocatorUnion*, void*);
void* GetField0x74deForValidIndex(char*, unsigned int);
unsigned char GetField0xcc0209ca98(char*);
unsigned char GetByteFieldAt0xcc(unsigned char*);
int GetField5cb0Value(char*);
int GetField5cb4Value(char*);
int GetField5cb8Value(char*);
int GetField5cbcValue(char*);
int GetFieldAt0x0(int*);
int UpdatePlayClocks020ac4f8(int);
int IsInRange0201b588(int);
extern "C" struct CombatantStruct* _ZN9GameState20GetUnknownGameObjectEv(struct BattleStruct*);
void* GetGlobalPtr021075f4(void);
void* AllocateAligned4(AllocatorUnion*, unsigned int);
unsigned char CopyOutRegion0x5718(char*, void*);
int SendBattleSaveBufferOrSetFlag020ac7b4(int flag);

struct Half70_a99d0 { unsigned short v[70]; };
struct Byte564_a99d0 { unsigned char v[564]; };
struct Half6_a99d0 { unsigned short v[6]; };
struct Byte102_a99d0 { unsigned char v[102]; };
struct Struct020a99d0 {
    struct Half70_a99d0 a;
    struct Byte564_a99d0 b;
    struct Half6_a99d0 c;
    struct Byte102_a99d0 d;
    unsigned char e;
};
void CopyStruct020a99d0(struct Struct020a99d0* dst, struct Struct020a99d0* src);

struct Entry_02028bd0;
struct Entry_02028bd0* GetEntryTableBase(void);
struct Entry_02028bd0* FindInlineEntryById(struct Entry_02028bd0* base, int key);

struct U16Field0x6_020375f8;
unsigned short GetU16At0x6(struct U16Field0x6_020375f8* obj);

struct EntryList_203dce4;
struct Vec3s32_020c3030;
struct OutVec3s16_0203e524;
int FindNearestEntryDistance0203e524(struct EntryList_203dce4* list, struct Vec3s32_020c3030* pos, struct OutVec3s16_0203e524* out);

struct Entry02087574;
struct EntryTable02087574;
struct Entry02087574* FindEntryBySignedId02087574(struct EntryTable02087574* obj, int id);

struct TreasureMapMetadata;
extern "C" void _ZN19TreasureMapMetadata26ClearInitialByteUnknownBitEv(struct TreasureMapMetadata* self);

extern int data_020f1bf0;
extern AllocatorUnion data_02114e20;
extern int data_02109bf4;
extern int data_02108760;

struct Buf14_020a9eb8 {
    int hdr;
    unsigned char data[0x10];
};

// USA: func_020a9eb8
extern "C" ARM int SerializeBattleSaveState_020a9eb8(void* arg0, char* region, unsigned int flags1, int mode) {
    struct BattleStruct* bs1 = _ZN9GameState11GetInstanceEv();
    void* ctxBase = func_02012fe4();
    void* p5a = func_0202ae18();
    void* structA = func_0205ec34();
    void* glob38 = GetGlobal02109418();
    void* ptr34 = _Z17GetPtrField0x2a04P9GameState(bs1);

    *(unsigned char*)((char*)bs1 + 0x5cc8) = 1;

    int local30 = 0;
    int local2c = 0;

    if ((flags1 & 1) || (flags1 & 8)) {
        local30 = 1;
        if (CheckField0NonZero((int*)p5a)) {
            if (func_0202c540(p5a) != 0) local30 = 0;
        }
        if (flags1 & 8) local30 = 1;
    }

    if (local30) {
        if (TestBitInByteArray((int)(char*)structA, (unsigned char*)structA + 0x8c, 0x113c)) {
            local2c = (unsigned short)(*(unsigned int*)((char*)ptr34 + 0x2c94));
        }
        *(unsigned int*)((char*)ptr34 + 0x2c94) = *(unsigned short*)ctxBase;
        if (flags1 & 8) {
            *(unsigned int*)((char*)ptr34 + 0x2c94) = 0x6d;
        }
    }

    _ZN9GameState11GetInstanceEv();
    char localBuf0d0[0x10];
    strcpy(localBuf0d0, (char*)&data_020f1bf0);

    int flag4 = flags1 & 4;
    int accVal;
    if (flag4) {
        accVal = *(int*)((char*)ctxBase + 0x3e4);
    } else {
        accVal = (int)_ZNK9GameState11GetDayTimerEv((struct BattleStruct*)ctxBase);
        *(int*)((char*)ctxBase + 0x3e4) = accVal;
    }

    struct Buf14_020a9eb8 buf14;
    buf14.data[0] = (mode != 0) ? 1 : 0;
    buf14.data[1] = 1;
    buf14.hdr = func_01ff85b8(buf14.data, 0x10);
    func_02075acc(0x10, &buf14, 0x14, 0);
    SendBattleSaveBufferOrSetFlag020ac7b4(0);

    int local28 = 0;
    if (mode == 0) {
        if (flags1 & 0x10) local28 = 1;
    }
    char* regionBase = region - 0x84;

    if (mode != 0) {
        goto AFTER_BIGBUF;
    }

    {
    struct BattleStruct* fp_bs = _ZN9GameState11GetInstanceEv();
    void* r7a = _Z17GetPtrField0x2a04P9GameState(fp_bs);
    void* r_c = func_0202ae18();
    void* r6a = func_02012fe4();
    func_ov017_0218b5b0();
    void* glob10 = GetGlobal02109418();
    func_020a0cc4(0x2ca8);
    char* bigBuf = (char*)AllocateAligned4(&data_02114e20, 0x2ca8);
    if (bigBuf == NULL) goto AFTER_BIGBUF;

    memset(bigBuf, 0, 0x2ca8);
    int resultCount = CopyOutRegion0x5718((char*)fp_bs, bigBuf + 0x60);

    unsigned char byte3 = *(unsigned char*)((char*)r7a + 0x2c8c);
    memcpy(bigBuf, (char*)r7a + 0xf80, byte3 * 0x23c);

    char* localArr = bigBuf + 0x60;

    for (int i1 = 0; i1 < resultCount; i1++) {
        unsigned char cnt = *(unsigned char*)((char*)r7a + 0x2c8c);
        unsigned char id1 = localArr[i1];
        int computedId = i1 + cnt;
        struct CombatantStruct* c = _Z25GetCombatantWithFlag0x100P9GameStatei(fp_bs, id1);
        if (c) {
            void* field150 = (void*)GetFieldAt0x150((unsigned char*)c);
            if (field150) {
                func_020830cc(field150, bigBuf + computedId * 0x23c);
            }
        }
    }

    *(unsigned char*)(bigBuf + 0x1d0c) = byte3;
    char* loop2Base = bigBuf + 0x1d12;
    for (int i2 = 0; i2 < resultCount; i2++) {
        unsigned char id2 = localArr[i2];
        struct CombatantStruct* c2 = _Z25GetCombatantWithFlag0x100P9GameStatei(fp_bs, id2);
        if (c2) {
            void* f150b = (void*)GetFieldAt0x150((unsigned char*)c2);
            if (f150b) {
                memcpy(loop2Base + i2 * 0x12, (char*)f150b + 0x454, 0x10);
            }
        }
    }

    memcpy(bigBuf + 0x1d0d, localArr, 4);
    *(unsigned char*)(bigBuf + 0x1d11) = (unsigned char)resultCount;

    if (_Z18GetField0x3acValueP9GameState(fp_bs) != 0 || func_0202c540(r_c) != 0) {
        *(unsigned char*)(bigBuf + 0x1d0d) = 0;
        *(unsigned char*)(bigBuf + 0x1d11) = 1;
    }

    memcpy(bigBuf + 0x1d5c, r7a, 0x1d4);

    int loopOff3 = 0;
    char* loop3Base1 = bigBuf + 0x1f30;
    char* loop3Base2 = bigBuf + 0x2710;
    for (int i3 = 0; i3 < 8; i3++) {
        void* p1 = GetPointerFromArray0xbd0((unsigned char*)r7a + 0x1d4, i3);
        void* p2 = GetPointerAt0xbf0((unsigned char*)r7a + 0x1d4, i3);
        signed short cnt3 = GetShortFromArray0xc10((unsigned char*)r7a + 0x1d4, i3);
        memcpy(loop3Base1 + loopOff3 * 2, p1, cnt3 * 2);
        memcpy(loop3Base2 + loopOff3, p2, cnt3);
        loopOff3 += cnt3;
        unsigned char b3 = GetByteFromArray0xc28((unsigned char*)r7a + 0x1d4, i3);
        *(unsigned char*)(bigBuf + i3 + 0x2b00) = b3;
    }

    memcpy(bigBuf + 0x2b08, (char*)r7a + 0xe04, 0x128);

    *(int*)(bigBuf + 0x2c30) = *(int*)((char*)r7a + 0xf6c);
    *(int*)(bigBuf + 0x2c34) = *(int*)((char*)r7a + 0xf68);
    *(int*)(bigBuf + 0x2c38) = *(int*)((char*)r7a + 0xf70);
    *(int*)(bigBuf + 0x2c3c) = *(int*)((char*)r7a + 0xf74);
    *(int*)(bigBuf + 0x2c40) = 0;
    memcpy(bigBuf + 0x2c44, (char*)r7a + 0x2c8d, 7);

    if (local30) {
        unsigned short v = *(unsigned short*)((char*)r7a + 0x2c94);
        *(unsigned short*)(bigBuf + 0x2c4c) = v;
        *(int*)((char*)fp_bs + 0x7e74) = *(int*)((char*)r7a + 0x2c94);
        if (func_0202c508(r_c)) {
            EnqueueEventTag23Field_021d0d58();
        }
    } else {
        *(unsigned short*)(bigBuf + 0x2c4c) = *(unsigned short*)((char*)fp_bs + 0x7e74);
    }
    *(unsigned short*)(bigBuf + 0x2c4e) = 0;

    {
        int idx = _Z18GetField0x3acValueP9GameState(fp_bs);
        unsigned short* pField = (unsigned short*)GetField0x74deForValidIndex((char*)fp_bs, idx);
        *(unsigned short*)(bigBuf + 0x2c50) = pField[0];
        *(unsigned short*)(bigBuf + 0x2c52) = pField[1];
        *(unsigned short*)(bigBuf + 0x2c54) = pField[2];
        *(unsigned short*)(bigBuf + 0x2c56) = pField[3];
    }

    *(unsigned char*)(bigBuf + 0x2c58) = GetField0xcc0209ca98((char*)&data_02109bf4);
    *(unsigned char*)(bigBuf + 0x2c59) = GetByteFieldAt0xcc((unsigned char*)&data_02108760);
    *(unsigned char*)(bigBuf + 0x2c5a) = *(unsigned char*)((char*)r7a + 0xf7d);
    *(unsigned char*)(bigBuf + 0x2c5b) = *(unsigned char*)((char*)fp_bs + 0x63dc);

    {
        char tmp6[6];
        memcpy(tmp6, (char*)fp_bs + 0x74fe, 6);
        memcpy(bigBuf + 0x2c78, tmp6, 6);
    }

    if (func_0202c540(r_c) != 0) {
        char* q = (char*)r6a + 0x1840;
        *(int*)(bigBuf + 0x2c80) = *(int*)(q + 0xb50);
        *(int*)(bigBuf + 0x2c84) = *(int*)(q + 0xb54);
        char* q2 = (char*)r6a + 0x2700;
        *(unsigned short*)(bigBuf + 0x2c5c) = *(unsigned short*)(q2 + 0xaa);
        *(short*)(bigBuf + 0x2c5e) = *(signed short*)(q2 + 0xa8);
        CopyVec3((int*)(bigBuf + 0x2c60), (int*)((char*)r6a + 0x2798));
        char* q3 = (char*)r6a + 0x2000;
        *(int*)(bigBuf + 0x2c6c) = *(int*)(q3 + 0x7a4);
        *(unsigned char*)(bigBuf + 0x2c74) = *(unsigned char*)(q3 + 0x7ac);
        *(int*)(bigBuf + 0x2c70) = *(int*)(q3 + 0x7b0);
        *(unsigned char*)(bigBuf + 0x2c75) = (unsigned char)(*(unsigned short*)(q2 + 0xd2));
        *(unsigned char*)(bigBuf + 0x2c76) = (unsigned char)(*(unsigned short*)(q2 + 0xd4));
        *(unsigned char*)(bigBuf + 0x2c77) = *(unsigned char*)(q3 + 0x7d1);
    } else {
        char* q = (char*)r6a + 0x1840;
        *(int*)(bigBuf + 0x2c80) = *(int*)(q + 0xb48);
        *(int*)(bigBuf + 0x2c84) = *(int*)(q + 0xb4c);
        char* q2 = (char*)r6a + 0x2700;
        *(int*)(q + 0xb50) = *(int*)(bigBuf + 0x2c80);
        *(int*)(q + 0xb54) = *(int*)(bigBuf + 0x2c84);
        *(unsigned short*)(bigBuf + 0x2c5c) = *(unsigned short*)(q2 + 0x86);
        *(short*)(bigBuf + 0x2c5e) = *(signed short*)(q2 + 0x84);
        CopyVec3((int*)((char*)r6a + 0x2774), (int*)(bigBuf + 0x2c60));
        char* q4 = (char*)r6a + 0x2000;
        *(int*)(bigBuf + 0x2c6c) = *(int*)(q4 + 0x780);
        *(unsigned char*)(bigBuf + 0x2c74) = *(unsigned char*)(q4 + 0x788);
        *(int*)(bigBuf + 0x2c70) = *(int*)(q4 + 0x794);
        char* q5 = (char*)r6a + 0x2700;
        *(unsigned char*)(bigBuf + 0x2c75) = (unsigned char)(*(unsigned short*)(q5 + 0xb4));
        *(unsigned char*)(bigBuf + 0x2c76) = (unsigned char)(*(unsigned short*)(q5 + 0xb6));
        *(unsigned char*)(bigBuf + 0x2c77) = *(unsigned char*)(q4 + 0x7d0);
        *(int*)(q4 + 0x7a4) = *(int*)(bigBuf + 0x2c6c);
        *(unsigned char*)(q4 + 0x7ac) = *(unsigned char*)(bigBuf + 0x2c74);
        *(int*)(q4 + 0x7b0) = *(int*)(bigBuf + 0x2c70);
        *(unsigned short*)(q5 + 0xb4) = *(unsigned char*)(bigBuf + 0x2c75);
        *(unsigned short*)(q5 + 0xb6) = *(unsigned char*)(bigBuf + 0x2c76);
        *(unsigned char*)(q4 + 0x7d1) = *(unsigned char*)(bigBuf + 0x2c77);
    }

    memcpy(bigBuf + 0x2c88, (char*)glob10 + 0x4a8, 0x1a);
    *(unsigned char*)(bigBuf + 0x2ca2) = (unsigned char)local28;

    func_02075acc((int)((region + 4) - regionBase), bigBuf, 0x2ca8, 0);
    TailForward02012da4(&data_02114e20, bigBuf);
    func_020a0c0c();

    if (TestBitInByteArray((int)(char*)structA, (unsigned char*)structA + 0x8c, 0x113c)) {
        *(unsigned int*)((char*)ptr34 + 0x2c94) = (unsigned int)local2c;
    }
    }

AFTER_BIGBUF:
    {
    struct BattleStruct* bs5 = _ZN9GameState11GetInstanceEv();
    func_0202ae18();

    if (flag4 != 0) {
        void* r_c2b;
        {
            int q = (int)(region + 0x2ca7 - regionBase);
            char* buf2 = (char*)structA + 0x1eb;
            func_02075acc((int)q, buf2, 0xd5, 0);
        }
    } else {
        int f0 = GetField5cb0Value((char*)bs5);
        int f4 = GetField5cb4Value((char*)bs5);
        int f8 = GetField5cb8Value((char*)bs5);
        int fc = GetField5cbcValue((char*)bs5);
        char stackBuf[0x400];
        *(int*)(stackBuf + 0x168) = f0;
        *(int*)(stackBuf + 0x16c) = f4;
        *(int*)(stackBuf + 0x170) = f8;
        *(int*)(stackBuf + 0x174) = fc;
        CopyStruct020a99d0((struct Struct020a99d0*)(stackBuf + 0x1d8), (struct Struct020a99d0*)structA);
        func_02075acc((int)(region + 0x2cac - regionBase), stackBuf + 0xc8, 0x344, 0);
    }

    if (flag4 == 0) {
        struct BattleStruct* bs6 = _ZN9GameState11GetInstanceEv();
        func_020a0cc4(0xda8);
        char* bigBuf2 = (char*)AllocateAligned4(&data_02114e20, 0x194);
        if (bigBuf2 != NULL) {
            bigBuf2[0] = *(unsigned char*)((char*)bs6 + 0x5cda);
            memcpy(bigBuf2 + 4, (char*)bs6 + 0x5cdc, 0x190);
            func_02075acc((int)(region + 0x33c0 - regionBase), bigBuf2, 0x194, 0);
            TailForward02012da4(&data_02114e20, bigBuf2);
        }

        void* ctx2 = func_02012fe4();
        int r6b = (int)(region + 0x2c4 - regionBase);
        void* ctx3 = func_02012fe4();
        char* r7b = (char*)ctx3 + 0x840;
        func_02075acc((int)r6b, r7b, 0xd98, 0);
        func_020a0cc4(0xda8);
        char* bigBuf3 = (char*)AllocateAligned4(&data_02114e20, 0xda8);
        if (bigBuf3 != NULL) {
            VectorizedInvertedMemcpy(r7b + 0xd98, bigBuf3, 0xd98);
            r7b += 0x1000;
            *(int*)(bigBuf3 + 0xd98) = *(int*)(r7b + 0xb38);
            *(int*)(bigBuf3 + 0xd9c) = *(int*)(r7b + 0xb34);
            *(int*)(bigBuf3 + 0xda0) = *(int*)(r7b + 0xb44);
            *(unsigned short*)((bigBuf3 + 0xd00) + 0xa4) = *(unsigned short*)(r7b + 0xb3c);
            *(unsigned char*)(bigBuf3 + 0xda6) = *(unsigned char*)(r7b + 0xb61);
            *(unsigned char*)(bigBuf3 + 0xda7) = 0;
            func_02075acc((int)(char*)bigBuf3, bigBuf3, 0xda8, 0);
            TailForward02012da4(&data_02114e20, bigBuf3);
        }
    }

    func_020a0c0c();
    struct BattleStruct* bs7 = _ZN9GameState11GetInstanceEv();
    char stackBuf2[0x420];
    *(int*)(stackBuf2 + 0x164) = *(int*)((char*)bs7 + 0x5f7c);
    memcpy(stackBuf2 + 0xdc4 - 0xd00, (char*)bs7 + 0x5f80, 0x200);
    memcpy(stackBuf2 + 0xfc4 - 0xd00, (char*)bs7 + 0x6180, 0x200);
    func_02075acc((int)(region + 0x5a04 - regionBase), stackBuf2 + 0xdc4 - 0xd00, 0x404, 0);

    struct BattleStruct* bs8 = _ZN9GameState11GetInstanceEv();
    func_02075acc((int)(region + 0x6c3c - regionBase), (char*)bs8 + 0x626c, 0x110, 0);
    func_02075acc((int)(region + 0x2ff0 - regionBase), glob38, 0xa0, 0);

    void* glob38b = GetGlobal02109418();
    func_02075acc((int)(region + 0x3090 - regionBase), (char*)glob38b + 0x178, 0x330, 0);
    func_02075acc((int)(region + 0x3554 - regionBase), (char*)bs1 + 0x75f0, 0x4d0, 0);
    func_02075acc((int)(region + 0x3a24 - regionBase), (char*)bs1 + 0x7ac0, 0x3b4, 0);

    void* p5c = func_0202ae18();
    int fa0 = GetFieldAt0x0((int*)p5c);
    unsigned char byteD = *(unsigned char*)((char*)p5c + 0x100d);
    int clockFlag = (fa0 == 5 && byteD > 1) || (fa0 == 6);
    UpdatePlayClocks020ac4f8(clockFlag);

    struct BattleStruct* bs9 = _ZN9GameState11GetInstanceEv();
    void* r5clock = (void*)((char*)region + 0x208 + 0x5c00 - regionBase);
    void* r7clock = _ZN9GameState11GetInstanceEv();
    void* r6clock = func_02012fe4();
    char stackBuf3[0x2f0];
    if (mode != 0) {
        r5clock = (char*)r5clock + 0x8000;
    }
    VectorizedInvertedMemcpy((char*)r7clock + 0x64f4, stackBuf3, 0xad6);

    if (IsInRange0201b588(*(unsigned short*)r6clock)) {
        int cnt = *(unsigned char*)(stackBuf3);
        for (int im = 0; im < cnt; im++) {
            _ZN19TreasureMapMetadata26ClearInitialByteUnknownBitEv((struct TreasureMapMetadata*)(stackBuf3 + 2 + im * 0x1c));
        }
    }

    func_02075acc((int)r5clock, stackBuf3, 0xad6, 0);

    struct BattleStruct* bs10 = _ZN9GameState11GetInstanceEv();
    func_02075acc((int)(region + 0x68e0 - regionBase), (char*)bs10 + 0x569c, 0x7c, 0);

    if (mode != 0) {
        struct BattleStruct* bs11 = _ZN9GameState11GetInstanceEv();
        func_ov017_0218b5b0();
        struct CombatantStruct* comb = _ZN9GameState20GetUnknownGameObjectEv(bs11);
        if (comb != NULL) {
            char payload[0x1c];
            *(unsigned short*)(payload) = GetU16At0x6((struct U16Field0x6_020375f8*)comb);
            CopyVec3((int*)(payload + 4), (int*)((char*)comb + 0x44));
            *(unsigned short*)(payload + 2) = *(unsigned short*)((char*)comb + 0x54);

            struct Entry_02028bd0* entry = FindInlineEntryById(GetEntryTableBase(), *(unsigned short*)r6clock);
            if (entry == NULL) {
                *(unsigned short*)(payload + 0x12) = 0;
                *(unsigned short*)(payload + 0x10) = 0;
            } else {
                *(unsigned short*)(payload + 0x12) = *(unsigned short*)((char*)entry + 4);
                *(unsigned short*)(payload + 0x10) = (unsigned short)(((*(unsigned short*)((char*)entry + 2)) << 16) >> 20);

                unsigned char bit2 = (*(unsigned char*)((char*)&data_02109bf4 + 0xc8) >> 2) & 1;
                *(unsigned char*)(payload + 0x14) = bit2;

                void* listBase = GetGlobalPtr021075f4();
                int dist = FindNearestEntryDistance0203e524((struct EntryList_203dce4*)listBase, (struct Vec3s32_020c3030*)((char*)comb + 0x44), (struct OutVec3s16_0203e524*)(payload + 0x16));
                *(unsigned char*)(payload + 0x15) = (unsigned char)dist;

                struct GrottoStruct* grotto = _ZN9GameState15GetGrottoStructEv(_ZN9GameState11GetInstanceEv());
                unsigned int hword = *(unsigned int*)(payload + 0x30) /* unused placeholder */;
                (void)hword;
                unsigned int flags2dc = *(unsigned int*)((char*)comb + 0x1cc + 0x7000);
                if (*(unsigned char*)((char*)grotto + 3) != 0) {
                    flags2dc = flags2dc | 1;
                }
                func_02075acc((int)(region + 0x14c + 0x6c00 - regionBase), payload, 0x1c, 0);
            }
        }
    }

    struct BattleStruct* bs12 = _ZN9GameState11GetInstanceEv();
    func_02075acc((int)(region + 0x6978 - regionBase), (char*)bs12 + 0x71fc, 0x2c4, 0);

    if (mode != 0) {
        /* handled above via early jump for the entry-scan branch */
    } else if (flags1 & 0x10) {
        void* sbPtr = _Z17GetPtrField0x2a04P9GameState(_ZN9GameState11GetInstanceEv());
        char matchArr[0x40];
        memset(matchArr, 0, 0x40);
        int matchCount = 0;
        int idx7 = 0;
        char* entryBase = (char*)sbPtr + 0x2000;
        char* destArr = matchArr + 4;
        while (idx7 < *(unsigned char*)(entryBase + 0xc8c)) {
            char* entry7 = (char*)sbPtr + idx7 * 0x23c + 0xf00;
            signed char raw = *(signed char*)(entry7 + 0x80);
            int id7 = (int)(signed char)((raw << 26) >> 26);
            struct Entry02087574* found = FindEntryBySignedId02087574((struct EntryTable02087574*)sbPtr, id7);
            if (found != NULL) {
                memcpy(destArr + matchCount * 0x14, found, 0x14);
                matchCount++;
            }
            idx7++;
        }
        if (matchCount != 0) {
            *(unsigned short*)matchArr = 1;
            *(unsigned short*)(matchArr + 2) = (unsigned short)matchCount;
        }
        func_02075acc((int)(region + 0x6d4c - regionBase), matchArr, 0x40, 0);
    }

    int* p0 = *(int**)((char*)arg0 + 4) ? (int*)*(int**)((char*)arg0 + 4) : NULL;
    if (p0 != NULL) {
        int csum = func_01ff85b8((char*)p0 + 0x88, 0x6f5c);
        *(int*)region = csum;
        func_02075acc((int)(region - regionBase), region, 4, 0);
    }

    return 1;
    }
}
