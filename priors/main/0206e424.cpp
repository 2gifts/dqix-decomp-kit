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

extern "C" struct CombatantStruct* _ZN9GameState21GetPartyMemberByIndexEi(struct BattleStruct* battleStruct, int combatantId);

extern "C" void* _Z15GetFieldAt0x150Ph(unsigned char* p);
#define GetFieldAt0x150 _Z15GetFieldAt0x150Ph

extern "C" int _Z18TestBitInByteArrayiPhi(int owner, unsigned char* bits, int index);
#define TestBitInByteArray _Z18TestBitInByteArrayiPhi

extern "C" void _Z20SetOrClearBitInArrayPvPhii(void* owner, unsigned char* bits, int index, int set);
#define SetOrClearBitInArray _Z20SetOrClearBitInArrayPvPhii

struct S_a04c8;

extern "C" void _Z23LoadBattleBlock020ac4c0Pv(void* block);
#define LoadBattleBlock020ac4c0 _Z23LoadBattleBlock020ac4c0Pv

extern "C" void _Z26AddClamped11BitFieldAt0x14P7S_a04c8j(struct S_a04c8* block, unsigned int amount);
#define AddClamped11BitFieldAt0x14 _Z26AddClamped11BitFieldAt0x14P7S_a04c8j

extern "C" void _Z23CopyInBattleField0x7540Pv(void* block);
#define CopyInBattleField0x7540 _Z23CopyInBattleField0x7540Pv

struct SpellSlot0206e424 {
    unsigned int unk0;
    unsigned int unk4;
    unsigned int kind : 4;
    unsigned int unk8 : 28;
    unsigned int unkc : 12;
    unsigned int spellId : 11;
    unsigned int unkc2 : 9;
    unsigned int unk10[4];
};

struct SlotOrder0206e424 {
    signed char slots[9];
};

struct BattleBlock0206e424 {
    unsigned int words[0xb0 / 4];
};

extern struct SlotOrder0206e424 data_020e87b4;

inline bool IsPartyMember0206e424(int combatantId) {
    return combatantId >= 0 && combatantId <= 3;
}

inline bool IsUsableSlot0206e424(struct SpellSlot0206e424* slot) {
    return slot->kind <= 7;
}

// USA: func_0206e424
ARM void MarkLearnedSpellsFromTable0206e424(void* owner, int combatantId) {
    bool inParty = combatantId >= 0 && combatantId <= 3;
    if (!inParty) {
        return;
    }

    struct BattleStruct* battle = _ZN9GameState11GetInstanceEv();
    struct CombatantStruct* combatant = _ZN9GameState21GetPartyMemberByIndexEi(battle, combatantId);
    if (combatant == 0) {
        return;
    }

    void* spellData = GetFieldAt0x150((unsigned char*)combatant);
    if (spellData == 0) {
        return;
    }

    struct SlotOrder0206e424 order = data_020e87b4;
    struct SpellSlot0206e424* slots = (struct SpellSlot0206e424*)((char*)spellData + 0x194);
    unsigned char learned = 0;
    unsigned char i = 0;

    while (order.slots[i] >= 0) {
        struct SpellSlot0206e424* slot = &slots[order.slots[i]];
        if (IsUsableSlot0206e424(slot)) {
            unsigned short spellId = slot->spellId;
            if (spellId != 0) {
                int index = spellId + 0x76;
                if (!TestBitInByteArray((int)owner, (unsigned char*)owner + 0x8c, index + 0xc00)) {
                    SetOrClearBitInArray(owner, (unsigned char*)owner + 0x8c, index + 0xc00, 1);
                    learned = (unsigned char)(learned + 1);
                }
            }
        }
        i = (unsigned char)(i + 1);
    }

    struct BattleBlock0206e424 block;
    LoadBattleBlock020ac4c0(&block);
    AddClamped11BitFieldAt0x14((struct S_a04c8*)&block, learned);
    CopyInBattleField0x7540(&block);
}
