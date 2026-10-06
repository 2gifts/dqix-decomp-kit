#include <globaldefs.h>
#include "Combat/Main/BattleList.h"
struct BattleStruct;
extern "C" struct BattleStruct* _ZN9GameState11GetInstanceEv();

int CountEntriesType1WithId(int id);
struct Obj020bc180;
void CallFunc020c0204OnActiveNode(struct Obj020bc180* obj, int value, int arg);
extern "C" void* _Z24CallFunc020bc594OnField0Pv(void* obj, int unused);
void Forward0203aa80(void* obj, int arg2);
int TryAllocateOrAppend0203aaf8(void* obj, void* arg1, int* out);
struct Obj0203aba8;
int TryDispatchOrFallback0203aba8(struct Obj0203aba8* obj, void* key, void* target);
extern "C" void func_0209c2e0(void* obj, int a, int b);
extern "C" void func_020bbd9c(void);
void SetContextAndDispatch0203ac10(void* obj, void* a, int b);
int SetContextAndQuery0203ab6c(void* obj, void* arg1);
extern "C" int _ZNK9GameState21GetEffectiveDeltaTimeEv(struct BattleStruct* battleStruct);

struct Actor0209c840 {
    char pad0[0xb0];
    int state;                   // 0xb0
    int field_b4;                // 0xb4
    short field_b8;               // 0xb8
    short field_ba;                // 0xba
    void* fieldBC;                  // 0xbc
    void* fieldC0;                    // 0xc0
    void* fieldC4;                      // 0xc4
    unsigned char fc8_bit0 : 1;           // 0xc8
    unsigned char fc8_rest : 7;
    unsigned char field_c9;                 // 0xc9  phase
    unsigned short field_ca;                  // 0xca  timer
    unsigned char field_cc;                     // 0xcc
    unsigned char field_cd;                       // 0xcd
    short field_ce;                                 // 0xce
};

// USA: func_0209c840
extern "C" ARM void func_0209c840(struct Actor0209c840* actor) {
    struct BattleStruct* battle = _ZN9GameState11GetInstanceEv();

    if (actor->field_c9 == 1) {
        actor->field_ca = 0x320;
        if (actor->state == 1) {
            int count = CountEntriesType1WithId(actor->field_b8);
            if (count <= 0) {
                actor->field_ca = 0;
            } else {
                actor->fc8_bit0 = 1;
                CallFunc020c0204OnActiveNode((struct Obj020bc180*)&actor->fieldBC, 0, 0x14);
            }
        } else if (actor->state == 2) {
            actor->field_ca = 0;
        }
        actor->field_c9 = 2;
        return;
    }

    if (actor->field_c9 == 2) {
        unsigned int elapsed = _ZNK9GameState21GetEffectiveDeltaTimeEv(battle);
        if (elapsed < actor->field_ca) {
            actor->field_ca = actor->field_ca - elapsed;
            return;
        }
        _Z24CallFunc020bc594OnField0Pv(&actor->fieldBC, 1);
        if (actor->field_b4 > 0) {
            Forward0203aa80(actor, actor->field_b4);
        }
        TryAllocateOrAppend0203aaf8(actor, (void*)(int)actor->field_ce, 0);
        TryDispatchOrFallback0203aba8((struct Obj0203aba8*)actor, (void*)(int)actor->field_ce, &actor->fieldC4);
        func_0209c2e0(actor, actor->field_cd, 0);
        func_020bbd9c();
        actor->field_ca = 0x1f4;
        actor->field_c9 = 3;
        return;
    }

    if (actor->field_c9 != 3) {
        return;
    }

    if (actor->field_ce >= 0 && CountEntriesType1WithId(actor->field_ce) > 0) {
        return;
    }

    {
        unsigned int elapsed = _ZNK9GameState21GetEffectiveDeltaTimeEv(battle);
        if (elapsed < actor->field_ca) {
            actor->field_ca = actor->field_ca - elapsed;
            return;
        }
    }

    SetContextAndDispatch0203ac10(actor, &actor->fieldC4, 0);
    actor->field_ce = -1;
    if (actor->state == 1) {
        if (actor->field_b4 > 0) {
            Forward0203aa80(actor, actor->field_b4);
        }
        SetContextAndQuery0203ab6c(actor, (void*)(int)actor->field_b8);
        if (actor->fc8_bit0) {
            _Z24CallFunc020bc594OnField0Pv(&actor->fieldBC, 0);
            CallFunc020c0204OnActiveNode((struct Obj020bc180*)&actor->fieldBC, 0, 0);
            func_0209c2e0(actor, actor->field_cd, 0x1e);
        } else {
            func_0209c2e0(actor, actor->field_cd, 0);
        }
    }
    actor->field_c9 = 0;
}
