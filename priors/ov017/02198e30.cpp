#include <globaldefs.h>
#include "Combat/Main/BattleList.h"

struct BattleStruct* GetBattleStruct(void);
extern "C" void* func_0205ec34(void);
struct CombatantStruct* GetCombatantAtField0x397c(struct BattleStruct* battleStruct);
extern "C" int func_020321e0(void* a, void* b, int c, void* d, unsigned int e);
int LookupAndForEachNode020649b0(void* a, int mode, void* c);
extern "C" void func_0206f81c(void* p);

struct LookupBuf02198e30 {
    char pad0[4];
    int id;
    char pad1[0x30 - 8];
    int field30;
};

// USA: func_ov017_02198e30  (semantic: SyncField491FromNodeSearch_02198e30)
extern "C" ARM void func_ov017_02198e30(unsigned char* self) {
    struct BattleStruct* battle = GetBattleStruct();
    unsigned char* mgr = (unsigned char*)func_0205ec34();
    struct CombatantStruct* combatant = GetCombatantAtField0x397c(battle);
    unsigned char* node = *(unsigned char**)(mgr + 0x494);

    while (node != 0) {
        unsigned int f10 = *(unsigned int*)(node + 0x10);
        if (func_020321e0((char*)combatant + 0x44, node + 0x14, *(short*)(node + 2), node + 4, f10)) {
            break;
        }
        node = *(unsigned char**)(node + 0x2c);
    }

    signed char flagVal = *(signed char*)(mgr + 0x491);

    if (node == 0) {
        if (flagVal <= -1) return;
        struct LookupBuf02198e30 c1;
        c1.id = flagVal;
        if (LookupAndForEachNode020649b0(mgr, 5, &c1)) {
            func_0206f81c(&c1);
        }
        *(mgr + 0x491) = (unsigned char)-1;
        return;
    }

    unsigned short nodeId = *(unsigned short*)node;
    if (nodeId == flagVal) return;
    if (flagVal <= -1) return;

    struct LookupBuf02198e30 c2;
    c2.id = flagVal;
    if (LookupAndForEachNode020649b0(mgr, 5, &c2)) {
        func_0206f81c(&c2);
    }

    c2.id = -1;
    c2.field30 = 0;

    unsigned short id2 = *(unsigned short*)node;
    *(self + 0x4000 + 0x446) = (unsigned char)id2;

    unsigned short id3 = *(unsigned short*)node;
    c2.id = id3;
    if (LookupAndForEachNode020649b0(mgr, 2, &c2)) {
        func_0206f81c(&c2);
    }

    unsigned short id4 = *(unsigned short*)node;
    mgr = (unsigned char*)func_0205ec34();
    *(mgr + 0x491) = (unsigned char)id4;
}
