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
extern "C" struct CombatantStruct* _Z25GetCombatantWithFlag0x100P9GameStatei(struct BattleStruct* battleStruct, int combatantId);
extern "C" struct BattleStruct* _ZN9GameState11GetInstanceEv();

extern "C" struct CombatantStruct* _ZN9GameState21GetPartyMemberByIndexEi(struct BattleStruct* battleStruct, int combatantId);
extern "C" struct CombatantStruct* _ZN9GameState31GetMaybeWanderingMonsterByIndexEi(struct BattleStruct* battleStruct, int combatantId);
extern "C" struct CombatantStruct* _ZN9GameState20GetGameObjectByIndexEi(struct BattleStruct* battleStruct, int combatantId);
extern "C" struct CombatantStruct* _ZN9GameState20GetUnknownGameObjectEv(struct BattleStruct* battleStruct);

extern "C" void* func_0205ec34(void);
int TestBitInByteArray(int unused, unsigned char* arr, int index);
extern "C" void* func_ov017_0218b5b0(void);
extern "C" int _ZNK9GameState16GetTrueDeltaTimeEv(struct BattleStruct* battleStruct);
int FindCombatantWithNegativeField2_021a20c0(int unused, int mult, int limit);
extern "C" void* func_0200f374(void* dst, int count);
extern "C" void* func_0202ae18(void);
extern "C" void VectorizedInvertedMemcpy(const void* src, void* dst, unsigned int length);
int CheckField0NonZero(int* obj);

struct SearchStruct;
int TestBitBySignedByteIndex(struct SearchStruct* obj, int value);

struct U16Field0x6_020375f8 {
    char unk[0x6];
    unsigned short field;
};
unsigned short GetU16At0x6(struct U16Field0x6_020375f8* obj);
int GetSignedByte0x1ca(void* obj);

struct Vec3 { int x; int y; int z; };
extern "C" struct Vec3 func_02034104(struct CombatantStruct* combatant);

struct ListEntry_02028430 { unsigned char id; unsigned char unk[0xF]; };
struct List_02028430 { unsigned char unk0[2]; unsigned char count; unsigned char unk3; struct ListEntry_02028430* entries; };
struct ListEntry_02028430* GetListEntryChecked(struct List_02028430* list, int index);

struct ListEntry_020283c0 { unsigned char id; unsigned char unk[0xF]; };
struct List_020283c0 { unsigned char unk0[2]; unsigned char count; unsigned char unk3; struct ListEntry_020283c0* entries; };
struct ListEntry_020283c0* FindListEntryById(struct List_020283c0* list, int id);

struct Vec3s32_020c3030 { int x; int y; int z; };
struct SearchCtx_02028460 { unsigned char pad0[2]; unsigned char count; unsigned char pad1[1]; void* nodes; };
extern "C" int func_02028460(struct SearchCtx_02028460* ctx, struct Vec3s32_020c3030* pos);

int CheckFlag0x6cBit0Clear(unsigned char* obj);

struct Point02031118 { int x; int y; int z; };
struct Bounds02031118 { int minx; int miny; int minz; int maxx; int maxy; int maxz; };
int CheckPointOutsideBounds02031118(struct Point02031118* p, struct Bounds02031118* b);

short GetTableEntryEven02030c68(int x);
short GetTableEntryOdd02030c9c(int x);

int CheckAnySlotMatchesId02073d58(int a, void* b, int c);

int Distance3D020c3030(struct Vec3s32_020c3030* a, struct Vec3s32_020c3030* b);

void SubtractVec3(struct Vec3* a, struct Vec3* b, struct Vec3* out);
void AddVec3(struct Vec3* a, struct Vec3* b, struct Vec3* out);

extern "C" void func_020c2f18(struct Vec3* v, struct Vec3* out);

struct FixedVec3 { int x; int y; int z; };
int DotFixedVec3(struct FixedVec3* a, struct FixedVec3* b);

extern "C" int rand(void);

void CopyVec3(int* dst, int* src);

struct Vec3Target02073d50 { int x; int y; int z; };
void SetVec3At0x002073d50(struct Vec3Target02073d50* obj, int x, int y, int z);

struct Vec3Fixed02030e2c { int x; int y; int z; };
ARM int HwDivideRounded020c2bf4(unsigned int numerHi, unsigned int denomLo);
int HardwareSqrt(int value);
int Vec3LengthRounded(int* v);
void ScaleVec3Fixed02030e2c(struct Vec3Fixed02030e2c* in, int scale, struct Vec3Fixed02030e2c* out);

struct Vec3;
int ClosestPointOnSegment02031468(struct Vec3* a, struct Vec3* b, struct Vec3* p, struct Vec3* out);

extern "C" int func_02073dfc(unsigned short* idPtr, struct Vec3s32_020c3030* distArg);

void* FindMatchingEntryByMask02073ec4(int mask, void* list);
int SelectWeightedEntry02073fdc(void* p);

extern "C" int func_020c338c(int x, int z);
extern "C" int func_ov017_021a2128(void* a, int b, int c, int d, void* e, int f, int g, int h);

struct Words4020733d8 { int a; int b; int c; int d; };
extern struct Words4020733d8 data_020e88a8;

struct ModeField020733d8 {
    unsigned short mode : 2;
    unsigned short rest : 14;
};

struct FlagC2Field020733d8 {
    unsigned char pad : 6;
    unsigned char bit6 : 1;
    unsigned char pad2 : 1;
};

struct LevelField020733d8 {
    unsigned int lowBits : 21;
    unsigned int level : 4;
    unsigned int highBits : 7;
};

struct ZoneNode020733d8 {
    unsigned char id;
    unsigned char mask;
    unsigned char count;
    unsigned char flags;
    short x;
    short y;
    short z;
    unsigned char pad[2];
    struct ZoneNode020733d8** children;
};

// SCRATCH-USA: func_020733d8
extern "C" ARM void func_020733d8(unsigned char* self, int mode) {
    if (self == 0 || self[0xc] == 0) {
        return;
    }

    void* dataBase = func_0205ec34();
    if (TestBitInByteArray((int)dataBase, (unsigned char*)dataBase + 0x8c, 0x79a) != 0 &&
        *(unsigned short*)self >= 0x190 &&
        *(unsigned short*)self <= 0x199) {
        return;
    }

    struct BattleStruct* battleStruct = _ZN9GameState11GetInstanceEv();
    void* globalB = func_ov017_0218b5b0();
    unsigned short savedId = *(unsigned short*)self;
    int newTimer = *(int*)(self + 8) + _ZNK9GameState16GetTrueDeltaTimeEv(battleStruct);
    *(int*)(self + 8) = newTimer;
    if (newTimer < 0x3e8) {
        return;
    }

    {
        int selfMode = ((struct ModeField020733d8*)(self + 2))->mode;
        if (FindCombatantWithNegativeField2_021a20c0((int)globalB, selfMode, 0xc) < 0) {
            return;
        }
    }

    struct Vec3 targetPos;
    func_0200f374(&targetPos, 0xc);

    struct Words4020733d8 weightsStruct = data_020e88a8;
    int* weights = (int*)&weightsStruct;

    void* matchEntry = 0;
    int resultCount = 0;
    int bestIdx = -1;
    int status = -1;

    for (int retry = 0; retry < 4; retry++) {
        matchEntry = 0;

        struct BattleStruct* bs = _ZN9GameState11GetInstanceEv();
        int* counter = (int*)func_0202ae18();
        int localWeights[4];
        VectorizedInvertedMemcpy(weights, localWeights, 0x10);

        int candCount = 0;
        int candIds[4];

        if (CheckField0NonZero(counter) == 0) {
            struct CombatantStruct* combatant0 = _ZN9GameState20GetUnknownGameObjectEv(bs);
            if (combatant0 != 0 && localWeights[0] == -1) {
                candIds[0] = *(signed short*)((char*)combatant0 + 4);
                candCount = 1;
            }
        } else {
            int excludeVal = -1;
            for (int j = 0; j < 4; j++) {
                if (!TestBitBySignedByteIndex((struct SearchStruct*)counter, j)) {
                    continue;
                }
                struct CombatantStruct* cand = _ZN9GameState21GetPartyMemberByIndexEi(bs, j);
                if (cand == 0) {
                    continue;
                }
                if (savedId != GetU16At0x6((struct U16Field0x6_020375f8*)cand)) {
                    continue;
                }

                int cmpVal;
                int sbyte1 = GetSignedByte0x1ca(cand);
                if (sbyte1 < 0) {
                    cmpVal = j;
                } else {
                    cmpVal = GetSignedByte0x1ca(cand);
                }

                int found = 0;
                for (int idx = 0; idx < 4; idx++) {
                    if (cmpVal == localWeights[idx]) {
                        found = 0;
                        found = found + 1;
                        break;
                    }
                }
                if (!found && cmpVal != excludeVal) {
                    candIds[candCount] = cmpVal;
                    candCount++;
                }
            }
        }

        if (candCount == 0) {
            return;
        }

        int counts[4];
        for (int k = 0; k < candCount; k++) {
            counts[k] = 0;
            struct CombatantStruct* candCombatant = _ZN9GameState31GetMaybeWanderingMonsterByIndexEi(bs, candIds[k]);
            struct Vec3 candBase = func_02034104(candCombatant);

            struct Bounds02031118 bounds;
            bounds.minx = candBase.x + 0xf000;
            bounds.miny = candBase.y + 0x1800;
            bounds.minz = candBase.z + 0xf000;
            SetVec3At0x002073d50((struct Vec3Target02073d50*)&bounds.maxx,
                                  candBase.x - 0xf000, candBase.y - 0x800, candBase.z - 0xf000);

            for (int m = 0; m < 0xc; m++) {
                int slotMode = ((struct ModeField020733d8*)(self + 2))->mode;
                int slotId = m + (slotMode * 0xc + 0x70);
                struct CombatantStruct* slotCombatant = _ZN9GameState20GetGameObjectByIndexEi(bs, slotId);
                if (slotCombatant == 0) {
                    continue;
                }
                if (!CheckFlag0x6cBit0Clear((unsigned char*)slotCombatant)) {
                    continue;
                }
                if (CheckPointOutsideBounds02031118((struct Point02031118*)((char*)slotCombatant + 0x44), &bounds)) {
                    counts[k]++;
                }
            }
        }

        int idsFiltered[4];
        int countsFiltered[4];
        int outCount = 0;
        for (int c = 0; c < candCount; c++) {
            int val = counts[c];
            if (val < 3) {
                countsFiltered[outCount] = val;
                idsFiltered[outCount] = candIds[c];
                outCount++;
            }
        }

        if (outCount == 0) {
            return;
        }

        bestIdx = -1;
        int minVal = 0xc;
        for (int d = 0; d < outCount; d++) {
            if (countsFiltered[d] < minVal) {
                bestIdx = idsFiltered[d];
                minVal = countsFiltered[d];
            }
        }

        if (bestIdx < 0) {
            return;
        }

        weights[resultCount] = bestIdx;
        resultCount++;

        struct CombatantStruct* combatant = _Z25GetCombatantWithFlag0x100P9GameStatei(_ZN9GameState11GetInstanceEv(), bestIdx);
        if (combatant != 0 && ((struct FlagC2Field020733d8*)((char*)combatant + 0xc2))->bit6) {
            status = 0;
        }

        if (status < 0) {
            continue;
        }

        unsigned short fieldB8 = *(unsigned short*)((char*)combatant + 0xb8);
        struct ListEntry_02028430* zone = GetListEntryChecked((struct List_02028430*)(self + 0x18), fieldB8);
        if (zone == 0) {
            struct Vec3 combPos = func_02034104(combatant);
            int closestIdx = func_02028460((struct SearchCtx_02028460*)(self + 0x18), (struct Vec3s32_020c3030*)&combPos);
            if (closestIdx < 0) {
                status = -1;
            } else {
                zone = GetListEntryChecked((struct List_02028430*)(self + 0x18), closestIdx);
                if (zone == 0) {
                    status = -1;
                } else {
                    *(unsigned short*)((char*)combatant + 0xb8) = (unsigned short)closestIdx;
                }
            }
        }

        if (status >= 0 && (zone == 0 || ((struct ZoneNode020733d8*)zone)->count == 0)) {
            status = -1;
        }

        if (status < 0) {
            continue;
        }

        struct ZoneNode020733d8* zoneNode = (struct ZoneNode020733d8*)zone;
        struct Vec3 selfPos = func_02034104(combatant);

        struct FixedVec3 facingDir;
        func_0200f374(&facingDir, 0xc);
        facingDir.x = GetTableEntryEven02030c68(*(int*)((char*)combatant + 0x54));
        facingDir.z = GetTableEntryOdd02030c9c(*(int*)((char*)combatant + 0x54));

        int matchCount = 0;
        void* storedArr[16];

        for (int p = 0; p < zoneNode->count; p++) {
            struct ZoneNode020733d8** children = zoneNode->children;
            unsigned char childId = *(unsigned char*)children[p];
            if (CheckAnySlotMatchesId02073d58(childId, self, status) != 0) {
                continue;
            }
            storedArr[matchCount] = children[p];
            matchCount++;
        }

        struct Vec3s32_020c3030 targetVec;
        targetVec.x = zoneNode->x << 12;
        targetVec.y = zoneNode->y << 12;
        targetVec.z = zoneNode->z << 12;
        int dist = Distance3D020c3030(&targetVec, (struct Vec3s32_020c3030*)&selfPos);
        if (dist > 0x5000) {
            unsigned char targetByte0 = zoneNode->id;
            int notFound = 1;
            for (int idx2 = 0; idx2 < matchCount; idx2++) {
                unsigned char elemByte = *(unsigned char*)storedArr[idx2];
                if (targetByte0 == elemByte) {
                    notFound = 0;
                    break;
                }
            }
            if (notFound) {
                if (CheckAnySlotMatchesId02073d58(targetByte0, self, -1) == 0) {
                    storedArr[matchCount] = zoneNode;
                    matchCount++;
                }
            }
        }

        status = -1;
        int bestDot = 0;
        for (int q = 0; q < matchCount; q++) {
            struct ZoneNode020733d8* candNode = (struct ZoneNode020733d8*)storedArr[q];
            struct Vec3 candPos;
            SetVec3At0x002073d50((struct Vec3Target02073d50*)&candPos, candNode->x << 12, candNode->y << 12, candNode->z << 12);
            struct Vec3 delta;
            SubtractVec3(&candPos, &selfPos, &delta);
            func_020c2f18(&delta, &delta);
            facingDir.y = 0;
            int dot = DotFixedVec3(&facingDir, (struct FixedVec3*)&delta);
            if (dot <= 0) {
                continue;
            }
            if (bestDot < dot) {
                bestDot = dot;
                status = 0;
                status = status + candNode->id;
            }
        }

        if (status < 0) {
            if (matchCount == 0) {
                status = -1;
                continue;
            }
            int randIdx = rand() % matchCount;
            status = *(unsigned char*)storedArr[randIdx];
        }

        struct ListEntry_020283c0* entry = FindListEntryById((struct List_020283c0*)(self + 0x18), status);
        struct ZoneNode020733d8* entryNode = (struct ZoneNode020733d8*)entry;
        if (entryNode == 0) {
            status = -1;
            continue;
        }

        struct Vec3s32_020c3030 entryPos;
        entryPos.x = entryNode->x << 12;
        entryPos.y = entryNode->y << 12;
        entryPos.z = entryNode->z << 12;
        CopyVec3((int*)&targetPos, (int*)&entryPos);

        if (zoneNode != entryNode) {
            int dist2 = Distance3D020c3030(&entryPos, (struct Vec3s32_020c3030*)&selfPos);
            if (dist2 > 0xa000) {
                struct Vec3 closest;
                int closestDist = ClosestPointOnSegment02031468((struct Vec3*)&entryPos, (struct Vec3*)&targetVec,
                                                                  &selfPos, &closest);
                if (closestDist > 0) {
                    if (closestDist >= 0xa000) {
                        status = -1;
                        continue;
                    }
                    int scaledSq = (int)(((long long)closestDist * closestDist + 0x800) >> 12);
                    unsigned int hw = HwDivideRounded020c2bf4(scaledSq, 0x64000);
                    int sqrtVal = HardwareSqrt(0x1000 - hw);
                    int threshold = (int)(((long long)sqrtVal * 0xa000 + 0x800) >> 12);
                    if (threshold > 0x5000) {
                        struct Vec3 diff;
                        SubtractVec3((struct Vec3*)&entryPos, &closest, &diff);
                        int lenDiff = Vec3LengthRounded((int*)&diff);
                        func_020c2f18(&diff, &diff);
                        if (lenDiff < threshold) {
                            ScaleVec3Fixed02030e2c((struct Vec3Fixed02030e2c*)&diff, lenDiff, (struct Vec3Fixed02030e2c*)&diff);
                        } else {
                            ScaleVec3Fixed02030e2c((struct Vec3Fixed02030e2c*)&diff, threshold, (struct Vec3Fixed02030e2c*)&diff);
                        }
                        AddVec3(&closest, &diff, &targetPos);
                    }
                }
            }
        }

        {
            int result1 = func_02073dfc((unsigned short*)self, (struct Vec3s32_020c3030*)&targetPos);
            if (result1 == 0) {
                status = -1;
            }
            if (self[0x1a] > 4) {
                status = -1;
            }
            if (status < 0) {
                continue;
            }

            int result2 = func_02073dfc((unsigned short*)self, (struct Vec3s32_020c3030*)&targetPos);
            if (result2 != 0) {
                continue;
            }

            struct BattleStruct* bs3 = _ZN9GameState11GetInstanceEv();
            struct Bounds02031118 boundsB;
            SetVec3At0x002073d50((struct Vec3Target02073d50*)&boundsB.minx,
                                  targetPos.x + 0x3000, targetPos.y + 0x5000, targetPos.z + 0x3000);
            SetVec3At0x002073d50((struct Vec3Target02073d50*)&boundsB.maxx,
                                  targetPos.x - 0x3000, targetPos.y - 0x5000, targetPos.z - 0x3000);

            int foundSlot = 0;
            for (int r = 0; r < 0xc; r++) {
                int slotMode = ((struct ModeField020733d8*)(self + 2))->mode;
                short slotId = slotMode * 0xc + 0x70 + r;
                struct CombatantStruct* slotCand = _ZN9GameState20GetGameObjectByIndexEi(bs3, slotId);
                if (slotCand == 0) {
                    continue;
                }
                if (!CheckFlag0x6cBit0Clear((unsigned char*)slotCand)) {
                    continue;
                }
                if (CheckPointOutsideBounds02031118((struct Point02031118*)((char*)slotCand + 0x44), &boundsB)) {
                    foundSlot = 1;
                    break;
                }
            }
            if (foundSlot) {
                continue;
            }

            struct ListEntry_020283c0* entry2 = FindListEntryById((struct List_020283c0*)(self + 0x18), status);
            if (entry2 == 0) {
                continue;
            }
            if (((struct ZoneNode020733d8*)entry2)->flags & 2) {
                continue;
            }
            unsigned char maskVal = ((struct ZoneNode020733d8*)entry2)->mask;
            matchEntry = FindMatchingEntryByMask02073ec4(maskVal, self + 0x60);
            if (matchEntry == 0) {
                continue;
            }

            int levelField = ((struct LevelField020733d8*)((char*)matchEntry + 4))->level;
            int invLevel = 6 - levelField;
            invLevel = invLevel + 1;
            int threshold2 = invLevel * 0x3e8;
            if (*(int*)(self + 8) >= threshold2) {
                break;
            }
        }
    }

    if (matchEntry == 0) {
        return;
    }
    int chosen = SelectWeightedEntry02073fdc(matchEntry);
    if (chosen == -1) {
        return;
    }

    struct CombatantStruct* combatant2 = _Z25GetCombatantWithFlag0x100P9GameStatei(battleStruct, bestIdx);
    int angle = 0;
    if (combatant2 != 0) {
        struct Vec3 posBuf3 = func_02034104(combatant2);
        struct Vec3 delta2;
        SubtractVec3(&posBuf3, &targetPos, &delta2);
        func_020c2f18(&delta2, &delta2);
        angle = func_020c338c(delta2.x, delta2.z);
    }

    int result3 = func_ov017_021a2128(globalB, savedId, chosen, *(unsigned short*)matchEntry,
                                       &targetPos, angle, (unsigned short)status, 0);
    if (result3 != 0) {
        *(int*)(self + 8) = 0;
    }
}
