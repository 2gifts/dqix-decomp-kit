#include <globaldefs.h>
#include "Combat/Main/BattleList.h"
struct BattleStruct;
struct CombatantStruct;
extern "C" struct CombatantStruct* _Z25GetCombatantWithFlag0x100P9GameStatei(struct BattleStruct* battleStruct, int combatantId);
extern "C" struct BattleStruct* _ZN9GameState11GetInstanceEv();
extern "C" struct CombatantStruct* _ZN9GameState19GetCombatantByIndexEi(struct BattleStruct* battleStruct, int id);

struct Struct021ecb90 {
    int a;
    unsigned char idx : 6;
    unsigned char mode : 2;
    unsigned char c;
    unsigned char d;
    unsigned char pad07;
    unsigned int e;
    unsigned int f;
    unsigned int g;
    unsigned int ringBase;
    unsigned int ringStride;
    int j;
    short k;
    short l;
    unsigned int m;
    int n;
    unsigned short o;
    short p;
    short q;
    short r;
    short s;
    short t;
    unsigned char u;
    unsigned char v;
    unsigned char w;
    unsigned char pad3b;
    unsigned short x;
    unsigned short pad3e;
};

struct Ctx021ee438;

struct Container021ebb90 {
    struct Struct021ecb90 entries[12];
    unsigned char f300;
    unsigned char f301;
    unsigned short f302;
    short f304;
    unsigned char f306;
    unsigned char pad307;
    int f308;
    struct Ctx021ee438* f30c;
    unsigned char f310;
    unsigned char f311;
};

struct List021600f8 { short id; char pad02[7]; unsigned char count; };
struct ListNode021600f8 {
    char pad00[0xe];
    short vals[3];
    unsigned char codes[3];
    unsigned char codeCount;
    unsigned char counts[3];
};
struct List02160094;
struct ListNode02160094 {
    char pad00[0x20];
    unsigned short id;
    char pad22[4];
    unsigned char f26, f27, f28, f29, f2a, f2b;
};
struct Row02079e2c { char pad[0x14]; unsigned int flags; };

struct Vec3 { int x; int y; int z; };
struct Mtx33_021ebb90 { int m[9]; };

struct Flags55Bits {
    unsigned char bit0 : 1;
    unsigned char bit1 : 1;
    unsigned char bit2 : 1;
    unsigned char bit3 : 1;
    unsigned char bit4 : 1;
    unsigned char bit5 : 1;
    unsigned char bit6 : 1;
    unsigned char bit7 : 1;
};

struct SpawnParams021ebb90 {
    unsigned char f00;
    char pad01[0xf];
    unsigned char f10;
    struct Flags55Bits flags;
    short f12;
    short targetId;
    short f16;
    short f18;
    short f1a;
    short f1c;
    short pad1e;
    int f20[3];
    struct Vec3 pos;
    int f38[3];
    struct Vec3 scale;
};

struct Slot021ebb90 { unsigned char idx; unsigned char cat; };

struct Bytes02033b88;
struct Obj0203a588;

void* GetActiveCombatWork(void);
extern "C" void* func_ov017_0218b5b0(void);
unsigned char GetByte_021dcc64_021dcc64(void* obj);
void SetByte_021dcc58(void* obj, unsigned char v);
extern "C" int _ZNK9GameState21GetEffectiveDeltaTimeEv(struct BattleStruct* battleStruct);
int CheckCombatantStatusOrTable_02162954(void* self);
int Check021ed2f4(void* obj);
extern "C" void func_ov025_021ebb24(struct Struct021ecb90* obj);
struct ListNode021600f8* GetNodeAtIndex021600f8(struct List021600f8* list, int index);
struct ListNode02160094* GetNodeAtIndex02160094(struct List02160094* list, int index);
void* GetData02108e10(void);
struct Row02079e2c* SearchBothTables02079e2c(char* table, int key);
void AppendUniqueShort0216346c(char* obj, int val);
void AppendUniqueShort021634c8(char* obj, int val);
extern "C" short func_ov000_0215ffa0(void* obj);
extern "C" int func_ov025_021d8c30(void* sl_, void* sb_, int kind, int category, int idx, int inner, int flag);
extern "C" struct CombatantStruct* _ZN9GameState20GetGameObjectByIndexEi(struct BattleStruct* battleStruct, int combatantId);
extern "C" void _ZN8Object3D11MakeVisibleEv(unsigned char* obj);
void ClearSubstructFlag0x4(unsigned char* obj);
int GetSubstructByte0x56(unsigned char* obj);
int SetByte0xbeShiftPrev(struct Bytes02033b88* p, int val);
void* GetGlobalField0x1c020421a0(void);
extern "C" void func_02043124(void* self);
struct Obj0203a588* GetData02104b6c(void);
void ResetLowFlagEntries0203a588(struct Obj0203a588* obj);
void ZeroFieldsAt0x6e8(void* obj);
extern "C" void _Z17SetMainBrightnessP13GameResourcesii(void* self, int a, int b);
int TestBit0At0x2f4(unsigned char* obj);
int TestBit1At0x2f4(unsigned char* obj);
extern "C" void* func_02057924(void);
extern "C" int _ZNK8Object3D9GetHeightEv(void* obj);
extern "C" void _ZN8Vector3iaSERKS_(struct Vec3* dst, struct Vec3* src);
extern "C" void Vector3fix_Subtract(struct Vec3* a, struct Vec3* b, struct Vec3* out);
extern "C" void Vector3fix_Normalize(struct Vec3* in, struct Vec3* out);
extern "C" void _Z24Vector3fixMultiplyScalarPK8Vector3iiPS_(struct Vec3* in, int scale, struct Vec3* out);
extern "C" int func_020c32f8(int);
extern "C" int func_020c3260(int);
extern "C" float func_0200c700(int);
extern "C" void Mat3x3_WriteRotationY(struct Mtx33_021ebb90* out, int sinv, int cosv);
extern "C" void Mat3x3_ApplyToVector(struct Vec3* v, struct Mtx33_021ebb90* m, struct Vec3* out);
int ClampScaledStat_0216352c(int id, float a, float b);
extern "C" int func_ov000_021635e4(int id, unsigned char mode);
int IsCombatantStateSpecial02163c0c(void* unused, int id);
void FindNodeAndProcess02057fb4(void* list, int id, int a3);
void DispatchIfField0xc4NonNeg_0205ebfc(void* obj, int a, int b);
int IsIdUnique021ee438(struct Ctx021ee438* obj, int id);
extern "C" void func_ov025_021dcf14(void* combatWork);
extern "C" void* func_ov000_02160f14(void* combatWork);
extern "C" void func_ov000_0216df00(void* handle, int a, int b, int c, float scale);
extern "C" void func_ov000_021626a0(void* combatWork, int a, int b);
void CopyStructFields_021ecb90(struct Struct021ecb90* dst, struct Struct021ecb90* src);

extern int data_02108760;
extern "C" void __clear(void* dst, unsigned int n);

static inline int IsPartyId021ebb90(unsigned int id) { return id <= 3; }

#define FX_MUL_021ebb90(a, b) ((int)((((long long)(a) * (long long)(b)) + 0x800) >> 12))

// USA: func_ov025_021ebb90
static inline struct Struct021ecb90* EntryAt021ebb90(struct Container021ebb90* o, int n) { return (struct Struct021ecb90*)o + n; }

extern "C" ARM void func_ov025_021ebb90(struct Container021ebb90* obj) {
    struct SpawnParams021ebb90 params;
    struct Vec3 scale;
    struct Vec3 dir;
    struct Vec3 targetPos;
    struct Vec3 selfPos;
    struct Vec3 toTarget;
    struct Vec3 targetRot;
    struct Mtx33_021ebb90 rot;
    int counts[6];
    struct Slot021ebb90 slots[32];

    unsigned char* codes;

    struct BattleStruct* battle = _ZN9GameState11GetInstanceEv();
    unsigned char* cw = (unsigned char*)GetActiveCombatWork();
    void* p54 = func_ov017_0218b5b0();

    if (GetByte_021dcc64_021dcc64(cw) != 0 && GetByte_021dcc64_021dcc64(cw) < 3) return;
    if (GetByte_021dcc64_021dcc64(cw) > 5 && GetByte_021dcc64_021dcc64(cw) < 7) return;

    unsigned int fpv = _ZNK9GameState21GetEffectiveDeltaTimeEv(battle);

    if (obj->f301 != 0 && CheckCombatantStatusOrTable_02162954(cw) != 0) {
        for (int j = 0; j < 12; j++) {
            if (obj->entries[j].a != 0) {
                if (fpv < obj->entries[j].f) obj->entries[j].f -= fpv;
                else obj->entries[j].f = 0;
                return;
            }
        }
        return;
    }

    int sp50 = 0;
    for (int i = 0; i < 12; i++) {
        if (obj->entries[i].a == 0) continue;
        struct Struct021ecb90* e = EntryAt021ebb90(obj, i);

        if ((e->d & 0xf) == 0xf) {
            if (e->f <= e->e || obj->f300 == 1) {
                if (e->d & 0x80) {
                    if (cw[0x6efe] != 4) return;
                    if (Check021ed2f4(obj->f30c) == 0) return;
                }
                func_ov025_021ebb24(&obj->entries[i]);
                if (obj->f300 != 0) obj->f300--;
                continue;
            }
            obj->entries[i].e += fpv;
            return;
        }

        struct ListNode021600f8* node1 = 0;
        struct ListNode02160094* node2 = 0;
        if (e->mode == 1) {
            struct ListNode021600f8* n = GetNodeAtIndex021600f8((struct List021600f8*)obj->entries[i].a, e->idx);
            if (n != 0) node1 = n;
        } else if (e->mode == 0) {
            struct ListNode02160094* n = GetNodeAtIndex02160094((struct List02160094*)obj->entries[i].a, 0);
            if (n != 0) node2 = n;
        } else if (e->mode == 2 || e->mode == 3) {
            struct ListNode021600f8* n = GetNodeAtIndex021600f8((struct List021600f8*)obj->entries[i].a, e->idx);
            if (n != 0) node1 = n;
        }

        unsigned int sp14 = obj->entries[i].e;
        unsigned int sp4c = fpv + obj->entries[i].e;

        if (!(obj->entries[i].d & 0x10)) {
            struct Row02079e2c* row = SearchBothTables02079e2c((char*)GetData02108e10(),
                                                               ((struct List021600f8*)obj->entries[i].a)->id);
            for (int spRow = 0; spRow < ((struct List021600f8*)obj->entries[i].a)->count; spRow++) {
                struct ListNode021600f8* node = GetNodeAtIndex021600f8((struct List021600f8*)obj->entries[i].a, spRow);
                if (node != 0) {
                    unsigned char found = 0;
                    unsigned char blocked = 0;
                    for (int j = 0; j < node->codeCount; j++) {
                        codes = node->codes;
                        if (codes[j] == 7 || codes[j] == 8 || codes[j] == 6) {
                            if (row != 0 && (row->flags >> 28 == 3 || row->flags >> 28 == 4)) {
                                blocked = 1;
                            } else {
                                AppendUniqueShort0216346c((char*)cw, node->vals[j]);
                                found = 1;
                            }
                        } else if (codes[j] == 1 || codes[j] == 2) {
                            if (found == 0) {
                                if (cw[0x6efe] == 0) cw[0x6efe] = 1;
                                if (blocked != 0 && obj->entries[i].f != 0) obj->entries[i].d |= 0x80;
                                AppendUniqueShort021634c8((char*)cw, node->vals[j - 1]);
                                *(short*)(cw + 0x6efc) = func_ov000_0215ffa0(node);
                            }
                            obj->f306 = 1;
                        }
                    }
                }
            }

            if (row == 0 || (row->flags >> 28 != 3 && row->flags >> 28 != 4)) {
                struct ListNode02160094* n0 = GetNodeAtIndex02160094((struct List02160094*)obj->entries[i].a, 0);
                int partyBlocked = 0;
                if (n0 != 0 && n0 != 0 && IsPartyId021ebb90(n0->id)) {
                    struct CombatantStruct* member = _Z25GetCombatantWithFlag0x100P9GameStatei(battle, n0->id);
                    if (member != 0 &&
                        (TestBit0At0x2f4(*(unsigned char**)((char*)member + 0x150)) != 0 ||
                         TestBit1At0x2f4(*(unsigned char**)((char*)member + 0x150)) != 0)) {
                        if (row != 0 && row->flags >> 28 != 2) partyBlocked = 1;
                    }
                }
                if (node1 != 0 && partyBlocked == 0) {
                    for (int j = 1; j < node1->codeCount; j++) {
                        unsigned char* codes = node1->codes;
                        if ((codes[j] == 7 || codes[j] == 8 || codes[j] == 6) && GetByte_021dcc64_021dcc64(cw) == 0) {
                            SetByte_021dcc58(cw, 1);
                            struct Struct021ecb90 copyA = obj->entries[i];
                            CopyStructFields_021ecb90((struct Struct021ecb90*)(cw + 0x6f78), &copyA);
                            obj->entries[i].ringBase = 0;
                            obj->entries[i].d |= 0x18;
                            if (obj->f306 != 0) obj->entries[i].d |= 6;
                            obj->f306 = 0;
                            sp50 = 1;
                        }
                    }
                }
            }
        }

        if (obj->f306 != 0) {
            struct Struct021ecb90 copyB = obj->entries[i];
            CopyStructFields_021ecb90((struct Struct021ecb90*)(cw + 0x6f78), &copyB);
            obj->entries[i].ringBase = 0;
            obj->entries[i].d |= 0x10;
            obj->entries[i].d |= 0x08;
            obj->entries[i].d |= 0x04;
            obj->entries[i].d |= 0x02;
        }

        if (!(obj->entries[i].d & 8)) {
            if (obj->f302 == 1) {
                if (obj->entries[i].a != 0) {
                    int idx = obj->entries[i].idx;
                    int mode = obj->entries[i].mode;
                    int targetId2 = -1;
                    if (mode == 1) {
                        struct ListNode021600f8* n = GetNodeAtIndex021600f8((struct List021600f8*)obj->entries[i].a, idx);
                        if (n != 0) targetId2 = func_ov000_0215ffa0(n);
                    } else if (mode == 0) {
                        struct ListNode02160094* n = GetNodeAtIndex02160094((struct List02160094*)obj->entries[i].a, idx);
                        if (n != 0) targetId2 = n->id;
                    }
                    if (targetId2 >= 0) {
                        func_ov000_0216df00(func_ov000_02160f14(cw), targetId2, 0, 0, 1.8f);
                        func_ov000_021626a0(cw, 4, 0);
                        struct CombatantStruct* c2 = _ZN9GameState19GetCombatantByIndexEi(battle, targetId2);
                        if (c2 != 0) {
                            _ZN8Object3D11MakeVisibleEv((unsigned char*)c2);
                            ClearSubstructFlag0x4((unsigned char*)c2);
                            if (GetSubstructByte0x56((unsigned char*)c2) != 0) {
                                SetByte0xbeShiftPrev((struct Bytes02033b88*)c2, 0);
                            }
                        }
                        func_02043124(GetGlobalField0x1c020421a0());
                        ResetLowFlagEntries0203a588(GetData02104b6c());
                        ZeroFieldsAt0x6e8(cw + 0x6ffc);
                    }
                }
                obj->entries[i].d |= 8;
            } else {
                obj->entries[i].d |= 8;
            }
        }

        if (!(obj->entries[i].d & 2)) {
            int ownerId = -1;
            struct ListNode02160094* n1 = GetNodeAtIndex02160094((struct List02160094*)obj->entries[i].a, 0);
            if (n1 != 0) ownerId = n1->id;

            if (sp14 <= obj->entries[i].g && obj->entries[i].g < sp4c) {
                int targetId3 = 0;
                if (node1 != 0) targetId3 = func_ov000_0215ffa0(node1);
                if (sp50 != 0) targetId3 = node1->vals[0];

                struct CombatantStruct* target = _ZN9GameState20GetGameObjectByIndexEi(battle, targetId3);
                if (target != 0) {
                    void* spawnObj = func_02057924();
                    params.targetId = targetId3;
                    params.f00 = 0;
                    params.f12 = 0;
                    params.f20[0] = 0;
                    params.f20[1] = 0;
                    params.f20[2] = 0;
                    params.pos.x = 0;
                    params.pos.y = 0;
                    params.pos.z = 0;
                    params.f38[0] = 0;
                    params.f38[1] = 0;
                    params.f38[2] = 0;
                    params.f1c = -0x1000;
                    params.f10 = 1;
                    params.f16 = -1;
                    params.f18 = -1;
                    params.f1a = -1;
                    params.scale.x = 0x1000;
                    params.scale.y = 0x1000;
                    params.flags.bit0 = 0;
                    params.flags.bit1 = 0;
                    params.flags.bit2 = 1;
                    params.flags.bit3 = 0;
                    params.flags.bit4 = 0;
                    params.flags.bit5 = 0;
                    params.flags.bit6 = 0;
                    params.flags.bit7 = 0;
                    params.scale.z = 0x1000;

                    if (obj->entries[i].n & 1) params.pos.y += _ZNK8Object3D9GetHeightEv(target) / 2;
                    if (params.pos.y < obj->f304) params.pos.y = obj->f304;
                    params.flags.bit2 = 0;

                    int inRange = (targetId3 >= 0xc0 && targetId3 <= 0xc7) ? 1 : 0;
                    if (inRange) {
                        if (e->o & 1) {
                            int sv = 0x1000;
                            switch (e->u) {
                            case 0:
                                sv = ClampScaledStat_0216352c(targetId3, e->p / 4096.0f, e->q / 4096.0f);
                                break;
                            case 1:
                                sv = func_ov000_021635e4(targetId3, e->p);
                                if (e->q <= sv) sv = e->q;
                                break;
                            }
                            scale.x = sv;
                            scale.y = sv;
                            scale.z = sv;
                            _ZN8Vector3iaSERKS_(&params.scale, &scale);
                        }

                        if (e->o & 2) {
                            int v = e->v;
                            int sf = 0x1000;
                            switch (v) {
                            case 0:
                                dir.x = e->r;
                                dir.y = e->s;
                                dir.z = e->t;
                                break;
                            case 1:
                            case 4: {
                                dir.z = e->r;
                                float speed = e->s / 4096.0f;
                                sf = (short)(ClampScaledStat_0216352c(targetId3, speed, 1.0f) - (int)speed);
                                if (sf <= 0) sf = 0;
                                dir.y = 0;
                                dir.x = 0;
                                if (e->t != 0) {
                                    struct CombatantStruct* owner = _ZN9GameState20GetGameObjectByIndexEi(battle, ownerId);
                                    struct CombatantStruct* victim = _ZN9GameState20GetGameObjectByIndexEi(battle, targetId3);
                                    if (owner != 0 && victim != 0) {
                                        _ZN8Vector3iaSERKS_(&selfPos, (struct Vec3*)((char*)owner + 0x44));
                                        _ZN8Vector3iaSERKS_(&targetPos, (struct Vec3*)((char*)victim + 0x44));
                                        Vector3fix_Subtract(&selfPos, &targetPos, &toTarget);
                                        Vector3fix_Normalize(&toTarget, &toTarget);
                                        _Z24Vector3fixMultiplyScalarPK8Vector3iiPS_(&toTarget, dir.z, &dir);
                                        _ZN8Vector3iaSERKS_(&targetRot, (struct Vec3*)((char*)victim + 0x50));
                                        int yaw = -targetRot.y;
                                        int cosv = (int)(4096.0f * (func_0200c700(func_020c32f8(yaw)) / 4294967296.0f));
                                        int sinv = (int)(4096.0f * (func_0200c700(func_020c3260(yaw)) / 4294967296.0f));
                                        Mat3x3_WriteRotationY(&rot, sinv, cosv);
                                        Mat3x3_ApplyToVector(&dir, &rot, &dir);
                                    }
                                }
                                break;
                            }
                            case 2:
                                sf = ClampScaledStat_0216352c(targetId3, 1.0f, 0.5f);
                                dir.x = e->r;
                                dir.y = e->s;
                                dir.z = e->t;
                                break;
                            case 3:
                                sf = e->r;
                                dir.x = 0;
                                dir.y = 0;
                                dir.z = func_ov000_021635e4(targetId3, 3);
                                break;
                            }
                            dir.x = FX_MUL_021ebb90(dir.x, sf);
                            dir.y = FX_MUL_021ebb90(dir.y, sf);
                            dir.z = FX_MUL_021ebb90(dir.z, sf);
                            if (v == 4) {
                                params.pos.x += dir.x;
                                params.pos.y += dir.y;
                                params.pos.z += dir.z;
                            } else {
                                _ZN8Vector3iaSERKS_(&params.pos, &dir);
                            }
                        }
                    }

                    if (sp50 != 0) params.pos.z += 0x2000;
                    if (cw[0x6ff9] != 0 && IsCombatantStateSpecial02163c0c(cw, targetId3) != 0) {
                        params.pos.y += 0xe66;
                    }
                    FindNodeAndProcess02057fb4(spawnObj, obj->entries[i].k, (int)&params);
                }

                if (obj->entries[i].d & 0x20) _Z17SetMainBrightnessP13GameResourcesii(p54, 0x10, 0xf);
                obj->entries[i].d |= 2;
            }
        }

        if (!(obj->entries[i].d & 4) && sp14 <= obj->entries[i].m && obj->entries[i].m < sp4c) {
            DispatchIfField0xc4NonNeg_0205ebfc(&data_02108760, obj->entries[i].l, 0);
            obj->entries[i].d |= 4;
        }

        unsigned int ringStart = obj->entries[i].x;
        unsigned int ringEnd = fpv + ringStart;
        __clear(counts, sizeof(counts));
        int* cnt = counts;
        int stalled = 0;
        if (node1 != 0) {
            cnt[0] = node1->counts[0];
            cnt[1] = node1->counts[1];
            cnt[2] = node1->counts[2];
        } else if (node2 != 0) {
            cnt[0] = node2->f26;
            cnt[1] = node2->f28;
            cnt[2] = node2->f29;
            cnt[3] = node2->f2b;
            cnt[4] = node2->f2a;
            cnt[5] = node2->f27;
        }
        if (obj->entries[i].c != 6) {
            for (int z = 0; z < 6; z++) {
                if (z != obj->entries[i].c) cnt[z] = 0;
            }
        }

        int numSlots = 0;
        for (int cat = 0; cat < 6; cat++) {
            for (int k = 0; k < cnt[cat]; k++) {
                slots[numSlots].idx = k;
                slots[numSlots].cat = cat;
                numSlots++;
            }
        }

        int waitUnique = 1;
        if (e->mode == 1) waitUnique = obj->f311 != 0 ? 0 : waitUnique;

        if (numSlots > 0) {
            for (int cur = obj->entries[i].w; cur < numSlots; cur++) {
                unsigned int slotTime = obj->entries[i].ringBase + cur * obj->entries[i].ringStride;
                if (!(ringStart <= slotTime && slotTime < ringEnd)) continue;
                if (IsIdUnique021ee438(obj->f30c, obj->f308) != 0 || waitUnique == 0) {
                    obj->f308 = func_ov025_021d8c30(cw, (void*)obj->entries[i].a, obj->entries[i].mode,
                                                    slots[cur].cat, obj->entries[i].idx, slots[cur].idx, 0);
                    obj->entries[i].w++;
                    if (obj->entries[i].d & 0x20) {
                        _Z17SetMainBrightnessP13GameResourcesii(p54, 0, 0xf);
                        obj->entries[i].d &= ~0x20;
                    }
                    if (IsIdUnique021ee438(obj->f30c, obj->f308) == 0 && waitUnique != 0) {
                        stalled = 1;
                        break;
                    }
                    obj->f308 = 0;
                    continue;
                }
                stalled = 1;
                break;
            }
            if (numSlots == obj->entries[i].w) {
                if (IsIdUnique021ee438(obj->f30c, obj->f308) != 0 || (obj->entries[i].d & 0x40) != 0 ||
                    waitUnique == 0) {
                    obj->entries[i].d |= 1;
                    obj->f308 = 0;
                    if (obj->f300 <= 1) obj->f310 = 1;
                }
            }
        } else {
            obj->entries[i].d |= 1;
        }

        if (sp50 != 0 && GetByte_021dcc64_021dcc64(cw) == 1) func_ov025_021dcf14(cw);

        obj->entries[i].e += fpv;
        if (stalled == 0) obj->entries[i].x += fpv;
        return;
    }
}
