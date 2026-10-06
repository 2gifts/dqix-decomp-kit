#include <globaldefs.h>
#include "Combat/Main/BattleList.h"
struct BattleStruct;
struct CombatantStruct;
extern "C" struct BattleStruct* _ZN9GameState11GetInstanceEv();

extern "C" struct CombatantStruct* _ZN9GameState21GetPartyMemberByIndexEi(struct BattleStruct* battleStruct, int combatantId);

struct U16Field0x6_020375f8 { char unk[0x6]; unsigned short field; };
extern "C" unsigned short _ZNK8Object3D10GetField06Ev(struct U16Field0x6_020375f8* obj);

struct Entry_02028bd0;
struct Entry_02028bd0* GetEntryTableBase(void);
struct Entry_02028bd0* FindInlineEntryById(struct Entry_02028bd0* base, int key);

extern "C" void* func_0202ae18(void);

struct CheckField0AndGlobalHalfStruct0202c508 { int field0; };
extern "C" int func_0202c508(struct CheckField0AndGlobalHalfStruct0202c508* obj);

struct Obj02079040;
unsigned short FindAndCacheListIndex(struct Obj02079040* obj, unsigned char* outFlag);

extern "C" unsigned short func_02078e60(void* obj, char* out);

extern "C" int _Z18TrySetMode02076cccPvi(void*, int);

struct Actor02076df4 {
    char pad0[0x134];
    int field134;
    unsigned char field138;
    char pad138[0x13];
    unsigned int field14c;
    char pad14c[0x14];
    unsigned short field164;
    unsigned short field166;
    char pad166[0x13];
    unsigned char field17b;
    unsigned char field17c;
    unsigned char field17d;
};

// USA: func_02076df4
extern "C" ARM void func_02076df4(struct Actor02076df4* obj) {
    struct BattleStruct* battleStruct = _ZN9GameState11GetInstanceEv();
    void* g = func_0202ae18();
    struct Entry_02028bd0* table = GetEntryTableBase();
    struct CombatantStruct* combatant = _ZN9GameState21GetPartyMemberByIndexEi(battleStruct, obj->field166);
    int key = _ZNK8Object3D10GetField06Ev((struct U16Field0x6_020375f8*)obj);
    struct Entry_02028bd0* entry = FindInlineEntryById(table, key);
    if (entry == 0) {
        return;
    }
    if (func_0202c508((struct CheckField0AndGlobalHalfStruct0202c508*)g) == 0) {
        return;
    }
    if (obj->field17c != 0) {
        return;
    }
    int modeOk = 1;
    if (combatant != 0 && obj->field17b != 0) {
        if (*((unsigned char*)combatant + 0xbe) == 0) {
            int cid = _ZNK8Object3D10GetField06Ev((struct U16Field0x6_020375f8*)combatant);
            int oid = _ZNK8Object3D10GetField06Ev((struct U16Field0x6_020375f8*)obj);
            if (cid == oid) {
                modeOk = 0;
            }
        }
    }
    if (obj->field14c <= 0xfa0) {
        return;
    }
    if (modeOk == 0) {
        return;
    }
    unsigned short oldField164 = obj->field164;
    unsigned char localByte = 0;
    int flag40 = 0;
    if ((obj->field17d & 0x40) == 0) {
        flag40 = 1;
    }
    obj->field17b = 0;
    switch (obj->field138) {
    case 0:
        return;
    case 1:
    case 3:
    case 4:
    case 5:
        if (obj->field134 == 4 || obj->field134 == 5 || obj->field134 == 6) {
            obj->field164 = FindAndCacheListIndex((struct Obj02079040*)obj, &localByte);
        } else {
            obj->field164 = func_02078e60((void*)obj, (char*)&localByte);
        }
        if (flag40 == 0 && localByte == 0) {
            _Z18TrySetMode02076cccPvi(obj, 2);
            return;
        }
        if ((oldField164 == obj->field164 && flag40 == 0) || localByte != 0) {
            _Z18TrySetMode02076cccPvi(obj, 1);
        } else {
            _Z18TrySetMode02076cccPvi(obj, 2);
        }
        return;
    case 2:
        obj->field17d |= 0x40;
        obj->field164 = func_02078e60((void*)obj, (char*)&localByte);
        if (oldField164 == obj->field164 && flag40 == 0) {
            _Z18TrySetMode02076cccPvi(obj, 1);
        } else {
            _Z18TrySetMode02076cccPvi(obj, 3);
        }
        return;
    }
}
