#include <globaldefs.h>
#include "std_library_functions.h"
#include "Combat/Main/BattleList.h"
struct CombatantStruct {
    unsigned short flags;
    char unk[0x132];
    struct BaseCombatStats* baseStats;
    struct ModifiableCombatStats* currentStats;
};

void GetFieldAt0x150(unsigned char* p);
void GetData02108e10();
extern "C" void func_ov024_021f8bd8(void* self, float* outWeight, void* obj8, void* bs);
extern "C" void __clear(void* buf, int n);

struct S_flag10000;
int IsFlagBit65536Set_021fb460(struct S_flag10000* p);
float ComputeQuarterDecayFactor020748d0(int v);
struct S_flag131072;
int IsFlagBit131072Set_021fb478(struct S_flag131072* p);
float ComputeQuarterDecayFactor(int v);
float LookupTableC0(int v);
extern "C" int _Z31IsCombatantFlag2Mask32_021e67b0P10GameObject(struct CombatantStruct* p);
extern "C" float func_ov024_021f875c(void* self, struct CombatantStruct* combatant, void* bs, int flag);

int CheckFlag0x14Bit0x10Set(unsigned char* p);
struct FlagObj_021da9b0;
int IsFlagBit8Set_021da9b0(struct FlagObj_021da9b0* p);
struct FlagObj_021dd010;
int IsFlagBit8Set_021dd010(struct FlagObj_021dd010* p);
struct FlagObj_021de25c;
int IsFlagBit5Set_021de25c(struct FlagObj_021de25c* p);
struct FlagObj_021da9c8;
int IsFlagBit19Set_021da9c8(struct FlagObj_021da9c8* p);
struct Struct_021fa76c;
int CheckAllFlagsClear_021fa76c(struct Struct_021fa76c* p);

struct AddEntryList_021f6a1c;
void AddEntryIfUnderLimit16_021f6a1c(struct AddEntryList_021f6a1c* obj, void* src);
extern "C" void func_ov024_021f9874(void* self, void* entryList, int b2, int b3);

struct SmallStruct10_021f6a4c {
    unsigned int w0;
    unsigned char b4, b5, b6, b7, b8;
    unsigned char tag6 : 6;
    unsigned char flag6 : 1;
    unsigned char flag7 : 1;
};

struct BsField08_021fdf04 { unsigned int lo8 : 8; unsigned int mode : 2; unsigned int hi22 : 22; };
struct BsField18_021fdf04 { unsigned int lo5 : 5; unsigned int val7 : 7; unsigned int hi20 : 20; };
struct WordFlags_021fdf04 { unsigned int bit0 : 1; unsigned int bit1 : 1; unsigned int rest : 30; };
struct BsField1c_021fdf04 { unsigned int lo14 : 14; unsigned int idx5 : 5; unsigned int hi13 : 13; };
struct PackedTag_021fdf04 { unsigned char tag : 6; unsigned char flag6 : 1; unsigned char flag7 : 1; };

extern unsigned char data_ov024_021fefeb[];
extern unsigned char data_ov024_021fefea[];

// USA: func_ov024_021fdf04
extern "C" ARM void func_ov024_021fdf04(char* self, int statusType) {
    struct CombatantStruct* obj8 = *(struct CombatantStruct**)(self + 8);
    GetFieldAt0x150((unsigned char*)obj8);
    GetData02108e10();

    char* bs = *(char**)(self + 0x64c);
    if (((struct BsField08_021fdf04*)(bs + 8))->mode == 2) return;

    float weight0 = 1.0f;
    func_ov024_021f8bd8(self, &weight0, obj8, bs);

    int count = *(int*)(self + 0x9c);
    float weights[8];
    struct CombatantStruct** combatantList = (struct CombatantStruct**)(self + 0x7c);
    __clear(weights, 0x20);

    for (int i = 0; i < count; i++) {
        float w = 1.0f;
        struct CombatantStruct* combatant = combatantList[i];
        float savedWeight = weight0;

        if (*(int*)(bs + 0x10) & 1) {
            if (IsFlagBit65536Set_021fb460((struct S_flag10000*)combatant)
                && (((struct BsField18_021fdf04*)(bs + 0x18))->val7 != 2)) {
                signed char t = (signed char)((*(int*)((char*)combatant->currentStats + 0x58) << 11) >> 29);
                w = w * ComputeQuarterDecayFactor020748d0(t);
            }
        }
        if (*(int*)(bs + 0x10) & 4) {
            if (IsFlagBit131072Set_021fb478((struct S_flag131072*)combatant)) {
                signed char t = (signed char)((*(int*)((char*)combatant->currentStats + 0x58) << 8) >> 29);
                w = w * ComputeQuarterDecayFactor(t);
            }
        }
        if (*(int*)(bs + 0x10) & 0x10) {
            unsigned char idx = *(unsigned char*)((char*)combatant->currentStats + 0x21);
            if (idx <= 3) {
                w = w * LookupTableC0((signed char)idx);
            } else if (_Z31IsCombatantFlag2Mask32_021e67b0P10GameObject(combatant)) {
                w = w * LookupTableC0(1);
            }
        }
        w = w * func_ov024_021f875c(self, combatant, bs, 1);
        weights[i] = savedWeight * w;
    }

    unsigned int idx = ((struct BsField1c_021fdf04*)(bs + 0x1c))->idx5;
    unsigned int raw14 = *(unsigned int*)(bs + 0x14);
    int category = raw14 >> 28;
    if (category == 5) {
        char* p150 = *(char**)((char*)obj8 + 0x150);
        int* sub = (int*)(p150 + 0x2f4);
        if (sub != 0) {
            struct WordFlags_021fdf04* word = (struct WordFlags_021fdf04*)sub;
            if (word->bit1) {
                category = 3;
            } else {
                category = word->bit0 ? 4 : 2;
            }
        }
    }
    if (category != 2 && category != 4 && category != 3) return;

    if (idx > 0xb) return;

    unsigned char idxTableB = data_ov024_021fefeb[idx * 2];
    int idxTableA = data_ov024_021fefea[idx * 2];
    unsigned char tag1cVal = idxTableB;

    unsigned char entryListBuf[0xc8];
    memset(entryListBuf, 0, sizeof(entryListBuf));
    memset(entryListBuf, 0, sizeof(entryListBuf));
    struct SmallStruct10_021f6a4c item;

    int matchCount = 0;
    int flag14 = 0;
    unsigned char kindByte = (unsigned char)statusType;
    int tableAtag = idxTableA & 0x3f;

    for (int j = 0; j < *(int*)(self + 0x9c); j++) {
        int flag10 = 1;
        matchCount = matchCount + 1;
        struct CombatantStruct* entry = combatantList[j];

        if (*(int*)(bs + 0x10) & 0x400) {
            if (*(int*)((char*)entry->currentStats + 0x14) & 0x200) flag14 = 1;
        }
        if (weights[j] < 30.0f) flag10 = 0;

        if (flag10) {
            int tag = 5;
            int flag8 = 0;
            int flag4 = 0;
            float sp0 = 33.3f;

            if (statusType == 4) {
                if (CheckFlag0x14Bit0x10Set((unsigned char*)entry->currentStats) == 0) flag8 = 1;
                flag4 = 1;
                sp0 = 35.0f;
            } else if (statusType == 5) {
                if ((*(int*)((char*)entry->currentStats + 0x14) & 0x40) == 0) flag8 = 1;
                flag4 = 1;
                sp0 = 35.0f;
            } else if (statusType == 3) {
                if (IsFlagBit8Set_021da9b0((struct FlagObj_021da9b0*)entry) == 0) flag8 = 1;
                flag4 = 1;
            } else if (statusType == 6) {
                if (IsFlagBit8Set_021dd010((struct FlagObj_021dd010*)entry) == 0) flag8 = 1;
                flag4 = 1;
            } else if (statusType == 7) {
                if (IsFlagBit5Set_021de25c((struct FlagObj_021de25c*)entry) == 0) flag8 = 1;
                flag4 = 1;
                sp0 = 35.0f;
            } else if (statusType == 8) {
                if (IsFlagBit19Set_021da9c8((struct FlagObj_021da9c8*)entry) == 0) flag8 = 1;
                flag4 = 1;
                if (*(short*)(bs + 0x30) == 2) {
                    void* f144 = *(void**)((char*)entry + 0x144);
                    if (f144 != 0) {
                        unsigned short hv = *(unsigned short*)((char*)f144 + 0xa);
                        if ((hv >> 11) & 1) flag8 = 0;
                    }
                }
                sp0 = 50.0f;
            } else if (statusType == 9) {
                if (weights[j] >= 30.0f) flag8 = 1;
                tag = 2;
            } else if (statusType == 0x11) {
                if (-1 <= entry->currentStats->attackBuff) flag8 = 1;
                if (entry->currentStats->primaryStats.attack <= 0x28) flag8 = 0;
                tag = 5;
            } else if (statusType == 0x12) {
                if (-1 <= entry->currentStats->defenseBuff) flag8 = 1;
                if (entry->currentStats->primaryStats.defense <= 0x14) flag8 = 0;
                tag = 5;
            } else if (statusType == 0x13) {
                if (-1 <= entry->currentStats->agilityBuff) flag8 = 1;
                if (entry->currentStats->primaryStats.agility <= 0x28) flag8 = 0;
                tag = 5;
            } else if (statusType == 0x14) {
                if (-1 <= entry->currentStats->magicalMightBuff) flag8 = 1;
                if ((unsigned short)entry->currentStats->primaryStats.magicalMight <= 0x64) flag8 = 0;
                tag = 5;
            } else if (statusType == 0x15) {
                if (-1 <= entry->currentStats->magicalMendingBuff) flag8 = 1;
                if ((unsigned short)entry->currentStats->primaryStats.magicalMending <= 0x64) flag8 = 0;
                tag = 5;
            } else if (statusType == 0x16) {
                if (-1 <= ((*(int*)((char*)entry->currentStats + 0x58) << 11) >> 29)) flag8 = 1;
                tag = 5;
            }

            if (!flag4 || CheckAllFlagsClear_021fa76c((struct Struct_021fa76c*)entry)) {
                if (weights[j] < sp0) flag8 = 0;
                if (flag8) {
                    *(float*)&item.w0 = (float)(*(short*)(bs + 0x30));
                    item.b6 = (unsigned char)j;
                    item.b4 = kindByte;
                    item.b5 = 1;
                    item.b7 = (unsigned char)(unsigned int)weights[j];
                    item.b8 = (unsigned char)tag;
                    item.tag6 = tableAtag;
                    item.flag6 = tag1cVal;
                    AddEntryIfUnderLimit16_021f6a1c((struct AddEntryList_021f6a1c*)entryListBuf, &item);
                }
            } else {
                continue;
            }
        }

        {
            int cond = 0;
            int lastIdx = *(int*)(self + 0x9c) - 1;
            if (j == lastIdx) {
                cond = 1;
            } else if (category == 2) {
                cond = 1;
            } else if (category == 4) {
                struct CombatantStruct* next = combatantList[j + 1];
                if (*(unsigned char*)((char*)entry + 0x17c) != *(unsigned char*)((char*)next + 0x17c)) cond = 1;
            }
            if (!cond) continue;
        }

        if (matchCount >= 2) {
            if (tag1cVal != 0) flag14 = 1;
        }
        if (matchCount <= 1) tag1cVal = 0;
        if (!flag14) {
            short someVal = *(short*)((char*)entry + 4);
            unsigned char b2 = *(unsigned char*)((char*)entry + 0x17c);
            unsigned char b3 = (unsigned char)someVal;
            if (category == 4) {
                b3 = 0xff;
            } else if (category == 3) {
                b3 = 0xff;
                b2 = b3;
            }
            func_ov024_021f9874(self, entryListBuf, b2, b3);
        }
        memset(entryListBuf, 0, sizeof(entryListBuf));
        matchCount = 0;
        flag14 = 0;
    }
}
