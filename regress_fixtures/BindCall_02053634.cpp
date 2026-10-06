#include <globaldefs.h>
#include "Filesystem/BackgroundLoader.h"
#include "Combat/Main/BattleList.h"
#include "Memory/SafeAllocator.h"
struct BattleStruct;
struct CombatantStruct;
extern "C" struct BattleStruct* _ZN9GameState11GetInstanceEv();

extern "C" int _Z26GetGlobalField0x1c020421a0v(void);
extern "C" unsigned char _Z19GetField0x397cValueP9GameState(struct BattleStruct* battleStruct);
extern "C" struct CombatantStruct* _ZN9GameState21GetPartyMemberByIndexEi(struct BattleStruct* battleStruct, int combatantId);
int GetSignedByte0x1c9(void* obj);
int GetIndexedEntryField0x178(signed char* obj);

// Object3D methods, declared by mangled name like the sibling files in this module.
extern "C" void _ZN8Object3D26RemoveAnimationPackageByIDEi(void* obj, int packageId);
extern "C" int _ZNK8Object3D19HasAnimationStoppedEv(void* obj);
extern "C" unsigned short _ZNK8Object3D10GetField06Ev(void* obj);

extern "C" void func_020531f0(void* obj);
extern "C" int _ZN8Object3D25LoadFromCCHROrCMOTArchiveEP21ObjectArchiveLoadInfoPFiPN4BCFG15AnimationRecordEE(void* obj, void* loadInfo, void* animCallback);
extern "C" int _ZN8Object3D24MaybeSetRegularAnimationEPKci(void* obj, const char* name, int flags);
extern "C" void* func_02012fe4(void);
extern "C" void func_0205308c(void* obj, int val);
extern "C" void* func_0205ec34(void);
extern "C" void func_0206f81c(void* p);

struct Obj0205eaa0;
extern "C" void _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(struct Obj0205eaa0* obj, int a, int b);

extern "C" void _Z32ForwardToTargetOrDefault0205eabcPvS_i(void* obj, void* target, int arg);
extern "C" int _Z32SetTargetFieldAndForEach02064a9cPvS_(void* param0, void* param1);
int GetFieldIfFlag4(char* obj);
int TestFlag0SetAndFlag1Clear(unsigned short* obj, int mask);
extern "C" void _Z25EnqueueEventTag6_021cc2f0h(int a0);
void SetFlagsAt0x244(unsigned char* obj, unsigned char mask);
void ClearFlagBits(unsigned char* obj, int mask);

struct Params02036804 {
    int flag;
    void* data;
    unsigned int size;
    void* alloc;
    int one;
    int pad18;
    int pad1c;
    int pad20;
};

struct Entry02053634 { short id; short kind; };
extern struct Entry02053634 data_020e7c94[];

extern char data_020f0477;
extern char data_020f047e;
extern int data_02108760;
extern char data_020f0481;
extern unsigned short data_02114e30;
extern char data_020f0486;

// USA: func_02053634
extern "C" ARM void func_02053634(void* obj) {
    char* ctx = (char*)obj;
    struct BattleStruct* bs = _ZN9GameState11GetInstanceEv();

    int hasEntries0 = ctx[0x17c] > 0 ? 1 : 0;
    if (hasEntries0 != 0) {
        short cid0 = *(short*)(ctx + 4);
        if (cid0 == _Z19GetField0x397cValueP9GameState(bs)
            && ((unsigned char*)_Z26GetGlobalField0x1c020421a0v())[0x19b1] != 0) {
            func_020531f0(ctx);
            return;
        }
    }

    struct CombatantStruct* combatant = _ZN9GameState21GetPartyMemberByIndexEi(bs, *(short*)(ctx + 4));
    int hasEntries = ctx[0x17c] > 0 ? 1 : 0;
    if (hasEntries != 0) {
        if (GetSignedByte0x1c9(combatant) > 0) {
            func_020531f0(ctx);
            return;
        }
    }

    signed char state = ctx[0x17e];
    if (state == 0) {
        BackgroundLoader* loader = BackgroundLoader::GetInstance();
        if (loader->GetTaskStatus(*(int*)(ctx + 0x180)) == 0) {
            return;
        }
        if (*(int*)(ctx + 0x194) >= 0) {
            if (loader->GetTaskStatus(*(int*)(ctx + 0x194)) == 0) {
                return;
            }
        }

        int moreAvailable = 0;
        if (loader->GetDetailedTaskStatus(*(int*)(ctx + 0x180)) != 2) {
            goto detailFail;
        }
        {
        _ZN8Object3D26RemoveAnimationPackageByIDEi(ctx, 2);
        void* filePtr;
        unsigned int fileLen;
        loader->GetLoadedFileByID(*(int*)(ctx + 0x180), &filePtr, &fileLen);
        if (filePtr != 0) {
            void* base = *(void**)(ctx + 0x154);
            void* alloc = (char*)base + 0x554;
            ((SafeAllocator*)alloc)->Reset();

            struct Params02036804 params;
            params.data = filePtr;
            params.flag = 0;
            params.pad18 = 0;
            params.pad1c = 0;
            params.size = fileLen;
            params.alloc = alloc;
            params.pad20 = 2;
            params.one = 1;
            if (_ZN8Object3D25LoadFromCCHROrCMOTArchiveEP21ObjectArchiveLoadInfoPFiPN4BCFG15AnimationRecordEE(ctx, &params, 0) == 0) {
                goto loadFail;
            }
            if (_ZN8Object3D24MaybeSetRegularAnimationEPKci(ctx, &data_020f0477, 1) == 0) {
                if (_ZN8Object3D24MaybeSetRegularAnimationEPKci(ctx, &data_020f047e, 1) == 0) {
                    func_020531f0(ctx);
                    return;
                }
                ctx[0x17f] = 1;
            } else {
                ctx[0x17f] = 0;
            }

            if (ctx[0x17f] == 0) {
                if (ctx[0x17d] + 1 < ctx[0x17c]) {
                    moreAvailable = 1;
                }
            }

            struct Entry02053634* e = data_020e7c94;
            for (; e->id > 0; e++) {
                if (e->id != GetIndexedEntryField0x178((signed char*)ctx)) continue;
                void* arg2 = 0;
                if (e->kind == 0x4e) {
                    arg2 = ctx + 0x198;
                }
                unsigned short val = *(unsigned short*)func_02012fe4();
                if (val == _ZNK8Object3D10GetField06Ev(ctx)) {
                    _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii((struct Obj0205eaa0*)&data_02108760, e->kind, (int)arg2);
                }
                break;
            }
            ctx[0x17e] = 1;
            goto loadOK;
        loadFail:
            func_020531f0(ctx);
            return;
        } else {
            goto loadOK;
        }
    loadOK:
        ctx[0x185] = 1;
        goto detailOK;
    detailFail:
        func_020531f0(ctx);
        return;
        }
    detailOK:
        loader->RemoveTask(*(int*)(ctx + 0x180));
        *(int*)(ctx + 0x180) = -1;
        if (moreAvailable != 0) {
            if (*(unsigned char*)(ctx + 0x185) != 0) {
                ctx[0x186] = 1;
            } else {
                signed char idx = ctx[0x17d];
                char* p = ctx + (idx + 1) + 0x100;
                func_0205308c(ctx, p[0x78]);
            }
        }
    } else {
        if (state == 1) {
            if (_ZNK8Object3D19HasAnimationStoppedEv(ctx) == 0) {
                return;
            }
            if (ctx[0x17f] == 0) {
                ctx[0x17d] = ctx[0x17d] + 1;
                if (ctx[0x17d] < ctx[0x17c]) {
                    ctx[0x17e] = 0;
                    *(int*)(ctx + 0x190) = 0;
                    return;
                }
                short cid = *(short*)(ctx + 4);
                if (cid == _Z19GetField0x397cValueP9GameState(bs)) {
                    void* p = func_0205ec34();
                    unsigned char req[0x34];
                    for (int i = 0; i < 4; i++) {
                        char* q = ctx + i;
                        q = q + 0x100;
                        ((signed char*)(req + 0x2a))[i] = q[0x78];
                    }
                    if (_Z32SetTargetFieldAndForEach02064a9cPvS_(p, req) != 0) {
                        func_0206f81c(req);
                    }
                }
                func_020531f0(ctx);
                return;
            } else if (ctx[0x17f] == 1) {
                int mode = 0;
                if (ctx[0x17c] > 1) {
                    mode = 1;
                    if (GetIndexedEntryField0x178((signed char*)ctx) == 0x19) {
                        _Z32ForwardToTargetOrDefault0205eabcPvS_i((void*)&data_02108760, ctx + 0x198, 0);
                    }
                }
                if (_ZN8Object3D24MaybeSetRegularAnimationEPKci(ctx, &data_020f0481, mode) != 0) {
                    ctx[0x17e] = 2;
                    return;
                }
                func_020531f0(ctx);
                return;
            } else {
                func_020531f0(ctx);
                return;
            }
        } else if (state == 2) {
            bs = _ZN9GameState11GetInstanceEv();
            int target = GetFieldIfFlag4((char*)bs);
            int flag = 0;
            if (ctx[0x17c] > 1) {
                if (_ZNK8Object3D19HasAnimationStoppedEv(ctx) != 0) {
                    flag = 1;
                }
            } else {
                short cid = *(short*)(ctx + 4);
                if (cid == _Z19GetField0x397cValueP9GameState(bs)
                    && TestFlag0SetAndFlag1Clear(&data_02114e30, 0xc03) != 0) {
                    flag = 1;
                    _Z25EnqueueEventTag6_021cc2f0h(*(short*)(ctx + 4));
                } else {
                    if (*(unsigned char*)(ctx + 0x184) != 0) {
                        ctx[0x184] = *(unsigned char*)(ctx + 0x184) - 1;
                        flag = 1;
                    } else if (target != 0) {
                        SetFlagsAt0x244((unsigned char*)target, 1);
                    }
                }
                if (flag != 0 && target != 0) {
                    ClearFlagBits((unsigned char*)target, 1);
                }
            }
            if (flag == 0) {
                return;
            }
            if (_ZN8Object3D24MaybeSetRegularAnimationEPKci(ctx, &data_020f0486, 1) == 0) {
                func_020531f0(ctx);
                return;
            }
            {
                signed char cursor = ctx[0x17d];
                signed char count = ctx[0x17c];
                int next = cursor + 1;
                if (next < count) {
                    char* q = ctx + next;
                    q = q + 0x100;
                    func_0205308c(ctx, q[0x78]);
                }
            }
            ctx[0x17e] = 3;
            return;
        } else if (state == 3) {
            if (_ZNK8Object3D19HasAnimationStoppedEv(ctx) == 0) {
                return;
            }
            if (GetIndexedEntryField0x178((signed char*)ctx) == 0x19) {
                _Z32ForwardToTargetOrDefault0205eabcPvS_i((void*)&data_02108760, ctx + 0x198, 0);
            }
            ctx[0x17d] = ctx[0x17d] + 1;
            if (ctx[0x17d] < ctx[0x17c]) {
                ctx[0x17e] = 0;
                *(int*)(ctx + 0x190) = 0;
                return;
            }
            short cid = *(short*)(ctx + 4);
            if (cid == _Z19GetField0x397cValueP9GameState(bs)) {
                void* p = func_0205ec34();
                unsigned char req[0x34];
                for (int i = 0; i < 4; i++) {
                    char* q = ctx + i;
                    q = q + 0x100;
                    ((signed char*)(req + 0x2a))[i] = q[0x78];
                }
                if (_Z32SetTargetFieldAndForEach02064a9cPvS_(p, req) != 0) {
                    func_0206f81c(req);
                }
            }
            func_020531f0(ctx);
            return;
        }
    }
}
