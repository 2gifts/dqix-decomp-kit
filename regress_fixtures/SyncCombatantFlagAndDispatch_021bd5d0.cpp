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
struct BattleStruct;
struct CombatantStruct;
extern "C" struct BattleStruct* _ZN9GameState11GetInstanceEv();

struct SearchStruct0202c1a4;
struct Foo02033b58;

struct Struct0130_021bd5d0 {
    unsigned int word0;
    char pad4[2];
    unsigned short half6;
    unsigned char byte8;
};

extern "C" void* func_0202ae18(void);
extern "C" void* func_ov017_0218b5b0(void);
extern "C" struct CombatantStruct* _ZN9GameState20GetGameObjectByIndexEi(struct BattleStruct* battleStruct, int combatantId);
int CheckField0NonZero(int* obj);
signed char GetSearchStructCurrentArrEntry(struct SearchStruct0202c1a4* obj);
extern "C" void _Z21ClearFieldBit02053fe4P9T02053fe4(void* obj, int field2, int flag);
int TryClearFlags0x130(unsigned char* obj, unsigned short flag);
void SetByteSavingPrevious(struct Foo02033b58* p, unsigned char v);
extern "C" void func_ov017_021c9d2c(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8);
extern "C" void func_ov017_02191108(void* unused, int c, int d, int e, int flag);

#pragma opt_propagation off
// USA: func_ov017_021bd5d0
extern "C" ARM void func_ov017_021bd5d0(void) {
    struct BattleStruct* battle = _ZN9GameState11GetInstanceEv();
    void* g = func_0202ae18();
    struct CombatantStruct* combatant = _ZN9GameState20GetGameObjectByIndexEi(battle, 0);
    void* other = func_ov017_0218b5b0();
    struct Struct0130_021bd5d0* p = *(struct Struct0130_021bd5d0**)((char*)combatant + 0x130);

    if (p->word0 & 1) {
        if (CheckField0NonZero((int*)g) && GetSearchStructCurrentArrEntry((struct SearchStruct0202c1a4*)g) == 0) {
            _Z21ClearFieldBit02053fe4P9T02053fe4(combatant, 1, 0);
            TryClearFlags0x130((unsigned char*)combatant, 1);

            p = *(struct Struct0130_021bd5d0**)((char*)combatant + 0x130);
            struct BaseCombatStats* q = combatant->baseStats;
            unsigned char byte8 = p->byte8;
            unsigned short maxMP = q->primaryStats.maxMP;
            unsigned short maxHP = q->primaryStats.maxHP;
            unsigned short half6 = p->half6;
            unsigned int word0 = p->word0;
            func_ov017_021c9d2c(*(short*)((char*)combatant + 4), 1, maxHP, half6, maxMP, word0, byte8, 1, 0);

            SetByteSavingPrevious((struct Foo02033b58*)combatant, 0);
            *(unsigned short*)((char*)combatant + 0xb2) = 0;
        } else if (CheckField0NonZero((int*)g) == 0) {
            _Z21ClearFieldBit02053fe4P9T02053fe4(combatant, 1, 0);
            TryClearFlags0x130((unsigned char*)combatant, 1);
            SetByteSavingPrevious((struct Foo02033b58*)combatant, 0);
            *(unsigned short*)((char*)combatant + 0xb2) = 0;
        }
        func_ov017_02191108(other, 1, 1, 1, 0);
    }
}
