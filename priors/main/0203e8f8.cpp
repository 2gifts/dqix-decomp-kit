#include <globaldefs.h>
#include "Memory/SafeAllocator.h"

struct Actor0203f008;
struct Element02040468;
extern struct Element02040468* GetElementFromActor0203f008(struct Actor0203f008* actor);

struct Vec3_020406f8 { unsigned int v[3]; };
struct Node020406f8;
extern void SelectVec3FromSources020406f8(struct Vec3_020406f8* dst, struct Node020406f8* n);

struct Vec3_020407c4 { unsigned int v[3]; };
struct Self020407c4;
extern void FillVec3Default020407c4(struct Vec3_020407c4* out, struct Self020407c4* self);

struct Table02040184;
extern int FindEntryByName02040184(struct Table02040184* table, const char* name);

extern void CopyVec3(int* dst, int* src);
extern void SetBitsInField0x6c(unsigned char* obj, unsigned int mask);
extern void SetBitsInField4(unsigned int* obj, unsigned int mask);
extern void ClearBitsInField4(unsigned int* obj, unsigned int mask);
extern unsigned int CopyRegionAndFlushCache(void* dst, const void* src, unsigned int length);
extern void CleanInvalidateCacheRange(const void* addr, unsigned int size);
extern void InvokeHandlerAndClearFlags020b3814(void);
extern int HardwareSqrt(int value);
extern void GetCurrentTimestamp();
extern int CallWithAddr4000330(int a);
extern void Set3DClearColor(int color, int alpha, int depth, int polygonId, int fogEnable);
extern void WriteControlAndToggle020d86d0(int a, int b);
extern void DelayThenSyncBit0(void);
extern void PushOneBitField0x3c();
extern unsigned short GetGlobalHalf0x8(void);
extern "C" void func_020c40f0(int handle);
extern void EnableVramBanksByMask(unsigned short mask);
extern void ResetGxEngineState020c52e8(void);
extern "C" void func_020c5414(void);
extern "C" void func_020b36c0(void);
extern "C" int func_ov017_0218b5b0(void);
extern "C" void func_ov017_0218eafc(int arg0, int arg1);
extern void ResetCommandQueue(void);
extern "C" void func_020c29ec(int a0, int a1, int a2, int a3, int b0, int b1, int b2, void* outBuf);
extern "C" void func_020c20d4(void* eye, void* up, void* target, void* out);
extern "C" void func_020c6688(void);
extern "C" int func_020c66bc(int p0, int p1, int p2);
extern "C" void func_020c6728(void);
extern void PackFieldsAt0x94(unsigned int a, unsigned int b, int flag);
extern void* GetData02107930(void);
extern "C" void* func_02012fe4(void);
extern "C" int func_0203e7b4(void* out, void* buf1, void* buf2, void* addr, int a, int b, int c, int d, int e);
extern void HalveField0x3c020dc098(void);

#define FixedMulRound_0203e8f8(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

struct Target0203e8f8 {
    char pad0[4];
    short f4;
    char pad6[0x3e];
    int f44v[3];
    int f50[3];
};

struct SelfObj0203e8f8 {
    char pad0[0x14];
    void* f14;
    void* f18;
    struct Target0203e8f8* f1c;
};

struct CombatantEntry_020e7990 {
    const char* name;
    unsigned char f4;
    unsigned char f5;
    short f6;
    short f8;
    short fA;
    short fC;
    char pad14[2];
};

struct LoopCEntry_0203e8f8 {
    unsigned short f0;
    unsigned short f2;
    unsigned short f4;
    unsigned short f6;
    char pad8[4];
    void* fc;
};

struct EntryB_0203e8f8 {
    char pad0[4];
    unsigned short count;
    char pad6[2];
    struct LoopCEntry_0203e8f8* entries;
};

struct CameraCache_020efd8c {
    int vec[3];
    int cosVal;
    int sinVal;
};

extern char data_020e9a50;
extern char data_021075d8;
extern char data_0210a010;
extern unsigned short data_020e7980[8];
extern int data_0210a018;
extern int data_0210a250[3];
extern int data_0210a25c[3];
extern int data_0210a268[3];
extern int data_020e7968[3];
extern int data_020e7974[3];
extern int data_0210a05c[12];
extern struct CombatantEntry_020e7990 data_020e7990[];
extern struct CameraCache_020efd8c data_020efd8c;

// SCRATCH-USA: func_0203e8f8
extern "C" ARM int func_0203e8f8(struct Actor0203f008* actor, struct SelfObj0203e8f8* sl, SafeAllocator* allocator) {
    char* elem;
    unsigned short paletteBuf[8];
    struct Vec3_020406f8 vecCopy1;
    struct Vec3_020407c4 vecCopy2;
    int v[3];
    int blitSetup[3];
    struct Vec3_020406f8 tmpA;
    struct Vec3_020407c4 tmpB;
    int sinVal;
    unsigned short half;
    unsigned int ptr28i;

    if (allocator == 0) return 0;
    if (sl == 0) return 0;
    if (sl->f14 == 0) return 0;
    if (sl->f1c == 0) return 0;

    elem = (char*)GetElementFromActor0203f008(actor);
    if (elem == 0) return 0;

    ptr28i = (unsigned int)func_ov017_0218b5b0();

    GetCurrentTimestamp();
    {
        unsigned short* s = data_020e7980;
        unsigned short* d = paletteBuf;
        int n = 8;
        do {
            unsigned short t = *s++;
            n--;
            *d++ = t;
        } while (n != 0);
    }
    CallWithAddr4000330((int)paletteBuf);

    Set3DClearColor(0x7c00, 0, 0x7fff, 0, 0);

    WriteControlAndToggle020d86d0(0, 0);
    DelayThenSyncBit0();
    PushOneBitField0x3c();

    half = GetGlobalHalf0x8();
    func_020c40f0(half & ~8);

    EnableVramBanksByMask(8);

    SetBitsInField0x6c((unsigned char*)sl->f1c, 0x10);

    SelectVec3FromSources020406f8(&tmpA, (struct Node020406f8*)sl);
    vecCopy1 = tmpA;

    FillVec3Default020407c4(&tmpB, (struct Self020407c4*)sl);
    vecCopy2 = tmpB;

    ResetGxEngineState020c52e8();
    func_020c5414();

    {
        unsigned int* flagBase = (unsigned int*)&data_021075d8;
        if (!(flagBase[1] & 1)) {
            flagBase[1] |= 1;
            data_020efd8c.sinVal = *(short*)(&data_020e9a50 + 0x64);
        }
    }
    {
        unsigned int* flagBase = (unsigned int*)((__typeof__(&data_021075d8))0x021075D8);
        if (!(flagBase[4] & 1)) {
            flagBase[4] |= 1;
            data_020efd8c.cosVal = *(short*)(&data_020e9a50 + 0x66);
        }
    }

    func_020b36c0();

    PackFieldsAt0x94(*(unsigned short*)((char*)GetData02107930() + 0x46), 0, 0);

    {
        struct Target0203e8f8* t = sl->f1c;
        t->f44v[0] = 0x64000;
        t->f44v[1] = 0;
        t->f44v[2] = 0;
    }

    SetBitsInField4((unsigned int*)ptr28i, 0x1000);
    func_ov017_0218eafc((int)ptr28i, sl->f1c->f4);
    ClearBitsInField4((unsigned int*)ptr28i, 0x1000);

    CopyVec3(sl->f1c->f44v, data_020e7968);
    CopyVec3(sl->f1c->f50, data_020efd8c.vec);
    InvokeHandlerAndClearFlags020b3814();

    {
        struct CombatantEntry_020e7990* r8 = data_020e7990;
        int cosVal = data_020efd8c.cosVal;
        int sb;
        sinVal = data_020efd8c.sinVal;
        for (sb = 0; sb < 3; sb++, r8++) {
            int aRaw = r8->fA;
            int bRaw = r8->fC;
            int b12 = bRaw << 12;
            int a12 = aRaw << 12;
            int mag = HardwareSqrt(FixedMulRound_0203e8f8(a12, a12) + FixedMulRound_0203e8f8(b12, b12));

            v[0] = FixedMulRound_0203e8f8(cosVal, b12);
            v[1] = FixedMulRound_0203e8f8(sinVal, mag);
            v[2] = FixedMulRound_0203e8f8(cosVal, a12);

            {
                int rectA = r8->f6;
                int rectB = r8->f8;
                func_020c29ec((0x60 - rectB) * 0x78, -(rectB + 0x60) * 0x78,
                               (rectA - 0x80) * 0x78, (rectA + 0x80) * 0x78,
                               0x1000, 0x64000, 0x1000, &data_0210a018);
            }

            *(unsigned int*)(&data_0210a010 + 0xfc) &= ~0x50;
            CopyVec3(data_0210a250, v);
            CopyVec3(data_0210a25c, data_020e7974);
            CopyVec3(data_0210a268, data_020e7968);
            func_020c20d4(v, data_020e7974, data_020e7968, data_0210a05c);
            *(unsigned int*)(&data_0210a010 + 0xfc) &= ~0xe8;
            InvokeHandlerAndClearFlags020b3814();

            func_ov017_0218eafc((int)ptr28i, sl->f1c->f4);
        }
    }

    ResetCommandQueue();
    WriteControlAndToggle020d86d0(0, 0);
    DelayThenSyncBit0();
    *(volatile unsigned int*)0x4000064 = 0x812b0010;
    DelayThenSyncBit0();
    CleanInvalidateCacheRange((void*)0x06870000, 0x10000);

    {
        void* buf1 = allocator->Allocate(*(unsigned int*)(elem + 0x1c) << 1);
        void* buf2 = allocator->Allocate(0x3c00);
        if (buf1 != 0 && buf2 != 0) {
            int ok = func_0203e7b4(blitSetup, buf1, buf2, (void*)0x06870000,
                                    *(unsigned int*)(elem + 0x1c), 0xc0, 0x50, 0x100, 0x80);
            if (ok != 0) {
                CleanInvalidateCacheRange(buf1, *(unsigned int*)(elem + 0x1c) << 1);
                func_020c6688();
                func_020c66bc((int)buf1, *(int*)(elem + 0x24), *(unsigned int*)(elem + 0x1c) << 1);
                func_020c6728();
                CleanInvalidateCacheRange(buf2, 0x8000);

                {
                    struct CombatantEntry_020e7990* member = data_020e7990;
                    int m;
                    for (m = 0; m < 3; m++, member++) {
                        int idx = FindEntryByName02040184((struct Table02040184*)(elem + 0x10), member->name);
                        void* entryPtr = 0;
                        if (*(unsigned int*)(elem + 0x28) > (unsigned int)idx) {
                            entryPtr = *(char**)(elem + 0x2c) + idx * 0x14;
                        }
                        if (entryPtr == 0) continue;
                        void* val = *(void**)((char*)entryPtr + 0x10);
                        if (val == 0) continue;

                        {
                            unsigned short f16 = *(unsigned short*)(elem + 0x16);
                            int firstVal = *(int*)val;
                            struct EntryB_0203e8f8* entryB = 0;
                            if (f16 > (unsigned int)firstVal) {
                                entryB = (struct EntryB_0203e8f8*)(*(char**)(elem + 0x18) + firstVal * 0x14);
                            }
                            if (entryB == 0) continue;

                            {
                                struct LoopCEntry_0203e8f8* r7 = entryB->entries;
                                int k;
                                for (k = 0; k < entryB->count; k++, r7++) {
                                    int heightShift = r7->f6 + 3;
                                    int widthShift = r7->f4 + 3;
                                    char* dstCursor = (char*)r7->fc;
                                    unsigned int lengthPerRow = 1u << widthShift;
                                    char* srcBase = (char*)buf2 + (member->f5 + r7->f2) * 0xc0;
                                    char* srcCursor = srcBase + (member->f4 + r7->f0);
                                    unsigned int count2;
                                    for (count2 = 0; count2 < (1u << heightShift); count2++) {
                                        CopyRegionAndFlushCache(dstCursor, srcCursor, lengthPerRow);
                                        dstCursor += lengthPerRow;
                                        srcCursor += 0xc0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    CopyVec3(sl->f1c->f44v, (int*)vecCopy1.v);
    CopyVec3(sl->f1c->f50, (int*)vecCopy2.v);

    GetCurrentTimestamp();
    func_020c40f0(half);
    HalveField0x3c020dc098();

    {
        char* d2 = (char*)GetData02107930();
        void* ctx = func_02012fe4();
        int f90 = *(int*)(d2 + 0x90);
        char* base2 = (char*)ctx + 0x10c;
        int idx2 = (f90 != 0) ? f90 : (*(int*)(d2 + 0x98));
        int mode = *(int*)(base2 + 0x304);
        unsigned short color;
        if (mode == 1) {
            color = *(unsigned short*)(base2 + idx2 * 2 + 0x8c);
        } else {
            color = *(unsigned short*)(base2 + idx2 * 2 + 0x100 + 0x26);
        }
        Set3DClearColor(color, 0x10, 0x7fff, 0, 0);
    }

    return 1;
}
