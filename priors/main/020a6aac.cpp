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

extern "C" void* func_02012fe4(void);
void* GetData02107930(void);
extern "C" void* func_0202ae18(void);
extern "C" void* func_0205ec34(void);
void* GetFieldIfFlag4(char* obj);

struct PointerField32c_ffc0 {
    char unk[0x32c];
    void* field;
};
void* GetPointerAt0x32c(struct PointerField32c_ffc0* obj);

extern "C" void* _Z20GetField0x3f8AddressP9GameState(struct BattleStruct* battleStruct);
extern "C" struct CombatantStruct* _ZN9GameState20GetUnknownGameObjectEv(struct BattleStruct* battleStruct);

extern "C" void func_020da77c(void* target, int type, void* msg);

void SetField0x21c(void* obj, short value);
void ClearBitsInField0x6c(unsigned char* obj, unsigned int mask);
void ClearFlag0x1InField0x6c(unsigned char* obj);

struct Shorts5c_374e0;
void SetShorts0x5cTo0x60(struct Shorts5c_374e0* obj, short a, short b, short c);

void SetBitsInField4(unsigned int* obj, unsigned int mask);
void ClearBitsInField4(unsigned int* obj, unsigned int mask);
void CopyVec3(int* dst, int* src);
void StoreVec3AtField0x50(unsigned char* obj, int a, int b, int c);

extern "C" int func_0202c508(void* obj);
extern "C" int func_0202c540(void* obj);

extern "C" void func_ov017_021d1a18(int a, short b, short c, unsigned char d);

struct Obj02033874;
void SetVecYFromValue02033874(struct Obj02033874* obj, int arg);

extern "C" void* func_0201b678(void* base, void* ptr);
extern "C" int func_02018fbc(int seed, void* v);
extern "C" void func_ov017_0219c598(int* srcVec3, short* srcAngle, int forceFlag);

void* FindNodeByField0x2c0201b754(void* obj, int key);
void SetBitsInField0x6c(unsigned char* obj, unsigned int mask);
void SetFlag0x6cBit0(unsigned char* obj);
extern "C" void _Z18ClearCombatantSlotP9GameStatei(struct BattleStruct* battleStruct, int id);

struct Obj0201b600;
struct Elem0201b600;
struct Struct02013380;
struct Elem0201b600* FindElemByKeys(struct Obj0201b600*, int, short);
void SetFlag0x40AndToggle0x4(struct Struct02013380* obj, int unused, int clear4);
void ApplyElementFlags0201b8c8(struct Obj0201b600* obj, int a1, int a2, int a3);

extern "C" void func_020da244(void* sb);

struct Vec3Target020a6aa4 { int x; int y; int z; };
void SetVec3At0x0020a6aa4(struct Vec3Target020a6aa4* obj, int x, int y, int z);

extern int data_ov017_021d82fc;
extern int data_020e909c;

struct Vec3Words020a6aac { int w[3]; };
struct LocalMsg020a6aac { int id; unsigned char flag; };

// USA: func_020a6aac
ARM void HandleBattleEvent_020a6aac(unsigned int* obj, int value1, int value2) {
    struct BattleStruct* battleStruct = _ZN9GameState11GetInstanceEv();
    char* base = (char*)func_02012fe4();
    GetData02107930();
    void* v4 = func_0202ae18();
    func_0205ec34();
    unsigned char* p6 = (unsigned char*)GetFieldIfFlag4((char*)battleStruct);
    unsigned char* p7 = (unsigned char*)GetPointerAt0x32c((struct PointerField32c_ffc0*)battleStruct);
    struct CombatantStruct* p8 = _ZN9GameState20GetUnknownGameObjectEv(battleStruct);
    unsigned char* fieldPtr = (unsigned char*)_Z20GetField0x3f8AddressP9GameState(battleStruct);

    if ((unsigned int)(value1 - 0x170c) <= 3) {
        p6[0x1f8] = 1;
        *(int*)(p6 + 0x200) = 0x3f800000;
        *(int*)(p6 + 0x204) = 0x1000;
    } else {
        p6[0x1f8] = 0;
    }

    if (value1 == 0x2710) {
        int* fp2 = (int*)_Z20GetField0x3f8AddressP9GameState(battleStruct);
        int flagMatch = 0;
        if (*(int*)((char*)fp2 + 0x20) == 0x25f1) flagMatch = 1;
        struct LocalMsg020a6aac local;
        local.id = -1;
        local.flag = 0;
        if (flagMatch) {
            local.id = 0xa0;
            func_020da77c(&data_ov017_021d82fc, 2, &local);
        } else {
            local.id = 0xc9;
            func_020da77c(&data_ov017_021d82fc, 1, &local);
        }
        SetField0x21c(p6, 0xc9);
        ClearBitsInField0x6c(p7, 0x100);
        ClearFlag0x1InField0x6c(p7);
        p7[0x154] = 1;
        SetShorts0x5cTo0x60((struct Shorts5c_374e0*)p7, 0xa0, 0xa0, 0xa0);
        SetBitsInField4(obj, 0x10004);
        int* src2774a = (int*)(base + 0x2774);
        CopyVec3((int*)(p7 + 0x44), src2774a);
        int* src2774b = (int*)(base + 0x2774);
        CopyVec3((int*)(p7 + 0x144), src2774b);
        int val780 = *(int*)(base + 0x2780);
        StoreVec3AtField0x50(p7, 0, val780, 0);
        if (func_0202c508(v4) != 0 && flagMatch == 0) {
            *(unsigned char*)(base + 0x2788) = 1;
            func_ov017_021d1a18(1, -1, 1, 0);
        }
        if (func_0202c540(v4) != 0) {
            *(int*)(p7 + 0x44) = 0x19000;
            *(int*)(p7 + 0x48) = 0x199;
            *(int*)(p7 + 0x4c) = 0x38ccc;
            SetVecYFromValue02033874((struct Obj02033874*)p7, 0);
        }
    } else if (value1 == 0x2774) {
        p7[0x154] = 0;
        ClearFlag0x1InField0x6c(p7);
    } else if (value1 >= 0x4e20 && value1 <= 0x752f) {
        if (p7 == 0) return;
        {
            SetShorts0x5cTo0x60((struct Shorts5c_374e0*)p7, 0x180, 0x180, 0x180);
            p7[0x154] = 0;
            if (value2 == 0x2710) {
                void* node1 = func_0201b678(base, base + 0x2774);
                if (node1 != 0) {
                    struct Vec3Words020a6aac localVec44;
                    localVec44 = *(struct Vec3Words020a6aac*)((char*)node1 + 0x30);
                    int local48 = func_02018fbc((int)base, &localVec44);
                    if (fieldPtr[5] == 0) {
                        CopyVec3((int*)((char*)p8 + 0x44), localVec44.w);
                        StoreVec3AtField0x50((unsigned char*)p8, 0, *(short*)((char*)node1 + 0x2e), 0);
                        func_ov017_0219c598(localVec44.w, (short*)((char*)node1 + 0x2e), 0);
                    }
                    *(unsigned short*)(base + 0x2784) = *(unsigned short*)((char*)node1 + 0x2c);
                    *(unsigned short*)(base + 0x2786) = (unsigned short)value1;
                }
                SetField0x21c(p6, *(short*)((char*)p8 + 4));
                ClearBitsInField4(obj, 0x10004);
                *(unsigned char*)(base + 0x2788) = 0;
                short val86a2 = *(short*)(base + 0x2786);
                short val84a2 = *(short*)(base + 0x2784);
                func_ov017_021d1a18(val86a2, val84a2, 0, 0);
            } else if (value2 == 0x170c) {
                unsigned short val84a = *(unsigned short*)(base + 0x2784);
                void* node2 = FindNodeByField0x2c0201b754(base, val84a);
                if (node2 != 0) {
                    struct Vec3Words020a6aac localVec38;
                    localVec38 = *(struct Vec3Words020a6aac*)((char*)node2 + 0x30);
                    int local3c = func_02018fbc((int)base, &localVec38);
                    if (fieldPtr[5] == 0) {
                        CopyVec3((int*)((char*)p8 + 0x44), localVec38.w);
                        func_ov017_0219c598(localVec38.w, (short*)((char*)node2 + 0x2e), 0);
                    }
                }
                SetField0x21c(p6, *(short*)((char*)p8 + 4));
                ClearBitsInField4(obj, 0x10004);
            }
            unsigned short val86b = *(unsigned short*)(base + 0x2786);
            if ((unsigned short)value1 == val86b) {
                short val84b = *(short*)(base + 0x2784);
                void* node3 = FindNodeByField0x2c0201b754(base, val84b);
                if (node3 != 0) {
                    SetBitsInField0x6c(p7, 0x100);
                    CopyVec3((int*)(p7 + 0x44), (int*)((char*)node3 + 8));
                    CopyVec3((int*)(p7 + 0x144), (int*)((char*)node3 + 8));
                    SetVecYFromValue02033874((struct Obj02033874*)p7, *(short*)((char*)node3 + 0x20));
                    struct Vec3Words020a6aac localVec2c;
                    localVec2c = *(struct Vec3Words020a6aac*)((char*)node3 + 0x40);
                    CopyVec3((int*)(base + 0x2774), localVec2c.w);
                    ClearFlag0x1InField0x6c(p7);
                }
            } else {
                SetFlag0x6cBit0(p7);
            }
        }
    } else if (value1 == 0x170c) {
        if (value2 == 0x2710) {
            struct CombatantStruct* c1 = _ZN9GameState20GetUnknownGameObjectEv(battleStruct);
            SetField0x21c(p6, *(short*)((char*)c1 + 4));
            ClearBitsInField4(obj, 0x10004);
            _Z18ClearCombatantSlotP9GameStatei(battleStruct, 0xc9);
            if (func_0202c540(v4) != 0) {
                if (*(unsigned char*)(base + 0x2788) == 0) {
                    ApplyElementFlags0201b8c8((struct Obj0201b600*)base, 0, 1, 0);
                } else {
                    ApplyElementFlags0201b8c8((struct Obj0201b600*)base, 1, 0, 1);
                }
            } else {
                ApplyElementFlags0201b8c8((struct Obj0201b600*)base, 1, 0, 1);
                *(unsigned char*)(base + 0x2788) = 1;
                func_ov017_021d1a18(1, -1, 1, 0);
            }
        } else {
            if (*(unsigned char*)(base + 0x2788) != 0) {
                ApplyElementFlags0201b8c8((struct Obj0201b600*)base, 1, 0, 1);
                func_ov017_021d1a18(1, -1, 1, 0);
            } else {
                ApplyElementFlags0201b8c8((struct Obj0201b600*)base, 0, 1, 0);
                short val86c = *(short*)(base + 0x2786);
                short val84c = *(short*)(base + 0x2784);
                func_ov017_021d1a18(val86c, val84c, 0, 0);
            }
        }
    } else if (value1 == 0x76c) {
        char* base2 = (char*)func_02012fe4();
        if (value2 == 0x2710) {
            struct CombatantStruct* c2 = _ZN9GameState20GetUnknownGameObjectEv(battleStruct);
            short angle570 = 0x570;
            struct Vec3Words020a6aac localVec20;
            localVec20 = *(struct Vec3Words020a6aac*)&data_020e909c;
            CopyVec3((int*)((char*)c2 + 0x44), localVec20.w);
            SetVecYFromValue02033874((struct Obj02033874*)c2, angle570);
            func_ov017_0219c598(localVec20.w, &angle570, 0);
            *(unsigned char*)(base2 + 0x2788) = 0;
            func_ov017_021d1a18((short)value1, -1, 0, 0);
            *(unsigned short*)(base2 + 0x2784) = 0;
            *(unsigned short*)(base2 + 0x2786) = (unsigned short)value1;
            short val4c3 = *(short*)((char*)_ZN9GameState20GetUnknownGameObjectEv(battleStruct) + 4);
            SetField0x21c(p6, val4c3);
            ClearBitsInField4(obj, 0x10004);
            struct Elem0201b600* e1 = FindElemByKeys((struct Obj0201b600*)base2, 0, 4);
            if (e1 != 0) SetFlag0x40AndToggle0x4((struct Struct02013380*)e1, 0, 1);
            struct Elem0201b600* e2 = FindElemByKeys((struct Obj0201b600*)base2, 0, 0x28);
            if (e2 != 0) SetFlag0x40AndToggle0x4((struct Struct02013380*)e2, 0, 1);
        } else {
            unsigned short val86d = *(unsigned short*)(base2 + 0x2786);
            if (val86d == 0x76c) {
                struct Elem0201b600* e3 = FindElemByKeys((struct Obj0201b600*)base2, 0, 4);
                if (e3 != 0) SetFlag0x40AndToggle0x4((struct Struct02013380*)e3, 0, 1);
                struct Elem0201b600* e4 = FindElemByKeys((struct Obj0201b600*)base2, 0, 0x28);
                if (e4 != 0) SetFlag0x40AndToggle0x4((struct Struct02013380*)e4, 0, 1);
            } else {
                struct Elem0201b600* e5 = FindElemByKeys((struct Obj0201b600*)base2, 0, 4);
                if (e5 != 0) SetFlag0x40AndToggle0x4((struct Struct02013380*)e5, 0, 0);
                struct Elem0201b600* e6 = FindElemByKeys((struct Obj0201b600*)base2, 0, 0x28);
                if (e6 != 0) SetFlag0x40AndToggle0x4((struct Struct02013380*)e6, 0, 0);
            }
        }
        _Z18ClearCombatantSlotP9GameStatei(battleStruct, 0xc9);
    } else {
        if (value2 == 0x2710) {
            ClearBitsInField4(obj, 0x10004);
            _Z18ClearCombatantSlotP9GameStatei(battleStruct, 0xc9);
            struct CombatantStruct* c4 = _ZN9GameState20GetUnknownGameObjectEv(battleStruct);
            SetField0x21c(p6, *(short*)((char*)c4 + 4));
            short val86e = *(short*)(base + 0x2786);
            short val84e = *(short*)(base + 0x2784);
            unsigned char flag2788 = *(unsigned char*)(base + 0x2788);
            func_ov017_021d1a18(val86e, val84e, flag2788, 1);
            unsigned char statusByte = *((unsigned char*)&data_ov017_021d82fc + 0xe);
            if (!(statusByte != 1 && statusByte != 2)) {
                func_020da244(&data_ov017_021d82fc);
            }
        }
    }

    struct Vec3Words020a6aac localVec14;
    localVec14 = *(struct Vec3Words020a6aac*)(base + 0x2774);
    if (localVec14.w[0] == 0 && localVec14.w[2] == 0) {
        SetVec3At0x0020a6aa4((struct Vec3Target020a6aa4*)&localVec14, 0x19000, 0x199, 0x38ccc);
        CopyVec3((int*)(base + 0x2774), localVec14.w);
    }
}
