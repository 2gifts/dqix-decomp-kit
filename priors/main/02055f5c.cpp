#include <globaldefs.h>
#include "Memory/SignedAllocator.h"

extern "C" int rand(void);

extern "C" void* func_02055ed0(void* self);
extern "C" void func_02058d40(void* obj, void* p1, void* p2, int p3);
extern "C" void* func_0200f374(void* dst, int count);
extern "C" int func_02030f30(int angle);
extern "C" void func_020c1180(void* dst);
extern "C" void func_020ca528(const void* pSrc, void* pDest);
extern "C" void func_020c1d60(void* dst, void* a, void* b);
extern "C" void func_020c1264(void* dst, int s);
extern "C" void func_020c1280(void* dst, int s, int c);
extern "C" void func_020c129c(void* dst, int s, int c);
extern "C" void func_020c15a4(void* dst, void* a, void* b);
extern "C" void func_02059720(void* obj);

struct S02055080;
void* GetField0x4Field0x0OrNull(struct S02055080* obj);
void* GetSubField0x8(struct S02055080* p);

struct Vec3i_020374f0 { int x; int y; int z; };
struct Vec3i_020374f0 GetVec3FromShortsAt0x5c(unsigned char* src);
void StoreVec3AsShortsAt0x5c(unsigned char* dst, int* src);

void CopyVec3(int* dst, int* src);

struct Struct02058c88;
void InitStruct02058c88(struct Struct02058c88* obj);

struct Mtx43_02030d84 { unsigned int v[12]; };
void BuildRotationMatrixY(struct Mtx43_02030d84* dst, int angle);

struct Mtx43_02030d30 { unsigned int v[12]; };
void BuildRotationMatrixX(struct Mtx43_02030d30* dst, int angle);

struct MtxFx43_02030dd8 { unsigned int v[12]; };
void BuildTransformMatrix02030dd8(struct MtxFx43_02030dd8* dst, void* src);

struct FixedVec3_2034 { int x; int y; int z; };
struct FixedMtx3T_2034 { struct FixedVec3_2034 row0; struct FixedVec3_2034 row1; struct FixedVec3_2034 row2; struct FixedVec3_2034 trans; };
void MulVec3MtxTranslate020c2034(struct FixedVec3_2034* v, struct FixedMtx3T_2034* m, struct FixedVec3_2034* out);

struct FixedVec3_17c4 { int x; int y; int z; };
struct FixedMtx3_17c4 { struct FixedVec3_17c4 row0; struct FixedVec3_17c4 row1; struct FixedVec3_17c4 row2; };
void MulVec3Mtx020c17c4(struct FixedVec3_17c4* v, struct FixedMtx3_17c4* m, struct FixedVec3_17c4* out);

short GetTableEntryEven02030c68(int x);
short GetTableEntryOdd02030c9c(int x);

#define ToBamAngle(degF) ((int)(4096.0f * (3.14159274f * ((degF) / 180.0f))))

struct Vec3iTriple { int x; int y; int z; };

struct PosData0x9c {
    int unused0;
    float x, y, z;
    float x2, y2, z2;
};

struct CountHolder0xc {
    char pad[0xc];
    unsigned short count;
};

struct Particle02055f5c {
    char pad00[0x10];
    int posX, posY, posZ;          // 0x10,0x14,0x18
    char pad1c[0x2c - 0x1c];
    unsigned char active2c;         // 0x2c
    unsigned char flag2d;           // 0x2d
    unsigned char flag2e;           // 0x2e
    char pad2f;
    char pad30[0x6c - 0x30];
    float velX, velY, velZ;         // 0x6c,0x70,0x74
    float speed78;                  // 0x78
};

struct Spawner02055f5c {
    char pad00[0x4];
    int field4;                     // 0x4
    char pad08[0x2c - 0x8];
    unsigned char* field2c;         // 0x2c
    void* ptr30;                    // 0x30
    char pad34[0x3a - 0x34];
    unsigned char field3a;          // 0x3a
    unsigned char field3b;          // 0x3b
    unsigned short field3c;         // 0x3c
    char pad3e[0x40 - 0x3e];
    int field40, field44, field48;  // 0x40,0x44,0x48
    char pad4c[0x70 - 0x4c];
    float field70, field74, field78, field7c, field80, field84; // 0x70-0x84
    char pad88[0xa8 - 0x88];
    float fieldA8;                  // 0xa8
    char padAC[0xb0 - 0xac];
    float fieldB0, fieldB4, fieldB8; // 0xb0,0xb4,0xb8
    char padBC[0xe8 - 0xbc];
    unsigned int fieldE8[9];         // 0xe8, 36 bytes
    int field10c, field110, field114; // 0x10c,0x110,0x114
    float field118, field11c, field120; // 0x118,0x11c,0x120
    char pad124[4];
    float field128, field12c, field130; // 0x128,0x12c,0x130
    float field134, field138, field13c; // 0x134,0x138,0x13c
    float field140, field144, field148; // 0x140,0x144,0x148
    float field14c;                  // 0x14c
    int field150;                    // 0x150
    char pad154[0x15c - 0x154];
    float field15c;                  // 0x15c
    float field160;                  // 0x160
    float field164, field168, field16c; // 0x164,0x168,0x16c
};

// SCRATCH-USA: func_02055f5c
extern "C" ARM void func_02055f5c(struct Spawner02055f5c* self, int param, int waveCount) {
    struct CountHolder0xc* ch = (struct CountHolder0xc*)GetField0x4Field0x0OrNull((struct S02055080*)self->ptr30);

    if (ch->count != 0) {
        if (self->field3c >= ch->count) return;

        struct SignedAllocatorHeader* elem = ((struct SignedAllocatorList*)((char*)self->ptr30 + 0x11c))->GetNthElement(self->field3c);

        struct Particle02055f5c* obj = (struct Particle02055f5c*)func_02055ed0(self);
        if (obj == 0) return;

        if (self->field2c != 0 && self->field40 == 0 && self->field44 == 0 && self->field48 == 0) {
            const struct Vec3i_020374f0& tmp = GetVec3FromShortsAt0x5c(self->field2c);
            CopyVec3(&self->field40, (int*)&tmp);
        }

        InitStruct02058c88((struct Struct02058c88*)obj);

        int tmpv[3];
        tmpv[0] = (int)((*(struct PosData0x9c**)elem)->x * 4096.0f);
        tmpv[1] = (int)((*(struct PosData0x9c**)elem)->y * 4096.0f);
        tmpv[2] = (int)((*(struct PosData0x9c**)elem)->z * 4096.0f);
        CopyVec3(&obj->posX, tmpv);

        struct PosData0x9c* p1 = *(struct PosData0x9c**)elem;
        int a = (int)(p1->x2 * 4096.0f);
        int b = (int)(p1->y2 * 4096.0f);
        int c = (int)(p1->z2 * 4096.0f);

        struct Vec3iTriple baseVec = *(struct Vec3iTriple*)&self->field40;
        int outv[3];
        outv[0] = (int)(((long long)a * baseVec.x + 0x800) >> 12);
        outv[1] = (int)(((long long)b * baseVec.y + 0x800) >> 12);
        outv[2] = (int)(((long long)c * baseVec.z + 0x800) >> 12);
        StoreVec3AsShortsAt0x5c(self->field2c, outv);

        func_02058d40(obj, self->ptr30, self->field2c, self->field4);
        obj->active2c = 1;

        self->field3c = self->field3c + 1;
        if (self->field3c >= ch->count) return;

        struct SignedAllocatorHeader* peekElem = ((struct SignedAllocatorList*)((char*)self->ptr30 + 0x11c))->GetNthElement(self->field3c);
        self->field150 = **(int**)peekElem;
        if (self->field150 != 0) return;

        func_02055f5c(self, param, waveCount);
        return;
    }

    unsigned char* sub = (unsigned char*)GetSubField0x8((struct S02055080*)self->ptr30);
    int byteCount = sub[5];
    int rem0 = rand() % (byteCount * 2);
    float ratio0 = (float)(rem0 - byteCount) / 100.0f;
    int scaled0 = (int)(self->field14c * ratio0);
    int innerCount = (int)(self->field14c + (float)scaled0);

    for (int wave = 0; wave < waveCount; wave++) {
        int remaining = waveCount - wave;

        for (int j = 0; j < innerCount; j++) {
            struct Particle02055f5c* obj = (struct Particle02055f5c*)func_02055ed0(self);
            if (obj == 0) continue;

            InitStruct02058c88((struct Struct02058c88*)obj);

            int rA = rand() % 90;
            float ratioA = (float)(rA + 10) / 100.0f;
            int rB = rand() % 90;
            float ratioB = (float)(rB + 10) / 100.0f;
            int angleDeg = rand() % 360;

            struct FixedVec3_2034 posv;
            func_0200f374(&posv, 0xc);
            posv.x = (int)(4096.0f * ratioA);
            int angle2 = ToBamAngle((float)angleDeg);

            struct Mtx43_02030d84 mtxY;
            BuildRotationMatrixY(&mtxY, angle2);
            struct Mtx43_02030d84 mtxYCopy = mtxY;
            MulVec3MtxTranslate020c2034(&posv, (struct FixedMtx3T_2034*)&mtxYCopy, &posv);

            float A = self->field118 * self->field128;
            float B = self->field11c * self->field12c;
            float C = self->field120 * self->field130;

            float px = (float)posv.x;
            float ratioP = px / 4096.0f;
            float D = ratioP * A;
            float E = self->field140 + D;
            float F = self->field7c * (float)param;
            float G = F / (float)remaining;
            float H = E + G;

            float I = ratioB * B;
            float J = self->field144 + I;
            float K = self->field80 * (float)param;
            float L = K / (float)remaining;
            float M = J + L;

            float pz = (float)posv.z;
            float ratioPZ = pz / 4096.0f;
            float N = ratioPZ * C;
            float O = self->field148 + N;
            float P = self->field84 * (float)param;
            float Q = P / (float)remaining;
            float R = O + Q;

            float S = B / 2.0f;
            float T = M - S;

            int posArr[3];
            posArr[0] = (int)(4096.0f * H) + self->field10c;
            posArr[1] = (int)(4096.0f * T) + self->field110;
            posArr[2] = (int)(4096.0f * R) + self->field114;
            CopyVec3(&obj->posX, posArr);

            float velXdeg = self->field134 + (self->field70 * (float)param) / (float)remaining;
            if (velXdeg < 0.0f) velXdeg = 360.0f + velXdeg;
            float velYdeg = self->field138 + (self->field74 * (float)param) / (float)remaining;
            if (velYdeg < 0.0f) velYdeg = 360.0f + velYdeg;
            float velZdeg = self->field13c + (self->field78 * (float)param) / (float)remaining;
            if (velZdeg < 0.0f) velZdeg = 360.0f + velZdeg;

            int angleX = ToBamAngle(velXdeg);
            struct Mtx43_02030d30 rotX;
            BuildRotationMatrixX(&rotX, angleX);
            struct Mtx43_02030d30 accumX = rotX;

            int angleY = ToBamAngle(velYdeg);
            struct Mtx43_02030d84 rotY;
            BuildRotationMatrixY(&rotY, angleY);
            struct Mtx43_02030d84 accumY = rotY;

            int angleZ = ToBamAngle(velZdeg);
            struct MtxFx43_02030dd8 transform;
            BuildTransformMatrix02030dd8(&transform, (void*)angleZ);
            struct MtxFx43_02030dd8 accumT = transform;

            func_020c1d60(&accumX, &accumY, &accumX);
            func_020c1d60(&accumX, &accumT, &accumX);

            int velVx = (int)(4096.0f * (self->field164 + (self->fieldB0 * (float)param) / (float)remaining));
            int velVy = (int)(4096.0f * (self->field168 + (self->fieldB4 * (float)param) / (float)remaining));
            int velVz = (int)(4096.0f * (self->field16c + (self->fieldB8 * (float)param) / (float)remaining));
            struct FixedVec3_17c4 velVec;
            velVec.x = velVx;
            velVec.y = velVy;
            velVec.z = velVz;

            MulVec3MtxTranslate020c2034((struct FixedVec3_2034*)&velVec, (struct FixedMtx3T_2034*)&accumX, (struct FixedVec3_2034*)&velVec);

            struct FixedMtx3_17c4 orientMtx;
            func_020c1180(&orientMtx);
            func_020ca528(self->fieldE8, &orientMtx);
            MulVec3Mtx020c17c4(&velVec, &orientMtx, &velVec);

            struct FixedMtx3_17c4 jitterAccum;
            func_020c1180(&jitterAccum);

            int jitterCount = (int)self->field160;
            if ((float)jitterCount >= 1.0f) {
                int rX = rand() % jitterCount;
                int angJ = ToBamAngle((float)rX);
                if (rand() % 2) {
                    angJ -= ToBamAngle((float)jitterCount);
                }
                angJ = func_02030f30(angJ);
                short cOdd = GetTableEntryOdd02030c9c(angJ);
                short sEven = GetTableEntryEven02030c68(angJ);
                struct FixedMtx3_17c4 m1;
                func_020c1264(&m1, sEven);
                func_020c15a4(&jitterAccum, &m1, &jitterAccum);

                int rY = rand() % jitterCount;
                int angJ2 = ToBamAngle((float)rY);
                if (rand() % 2) {
                    angJ2 -= ToBamAngle((float)jitterCount);
                }
                angJ2 = func_02030f30(angJ2);
                short cOdd2 = GetTableEntryOdd02030c9c(angJ2);
                short sEven2 = GetTableEntryEven02030c68(angJ2);
                struct FixedMtx3_17c4 m2;
                func_020c1280(&m2, sEven2, cOdd2);
                func_020c15a4(&jitterAccum, &m2, &jitterAccum);

                int rZ = rand() % jitterCount;
                int angJ3 = ToBamAngle((float)rZ);
                if (rand() % 2) {
                    angJ3 -= ToBamAngle((float)jitterCount);
                }
                angJ3 = func_02030f30(angJ3);
                short cOdd3 = GetTableEntryOdd02030c9c(angJ3);
                short sEven3 = GetTableEntryEven02030c68(angJ3);
                struct FixedMtx3_17c4 m3;
                func_020c129c(&m3, sEven3, cOdd3);
                func_020c15a4(&jitterAccum, &m3, &jitterAccum);

                MulVec3Mtx020c17c4(&velVec, &jitterAccum, &velVec);
            }

            float fz = (float)velVec.z / 4096.0f;
            float fy = (float)velVec.y / 4096.0f;
            float fx = (float)velVec.x / 4096.0f;
            obj->velX = fx;
            obj->velY = fy;
            obj->velZ = fz;

            obj->speed78 = self->field15c + (self->fieldA8 * (float)param) / (float)remaining;

            if (self->field3a != 0) {
                obj->flag2d = 1;
            } else if (self->field3b != 0) {
                obj->flag2e = 1;
            }

            func_02058d40(obj, self->ptr30, self->field2c, self->field4);
            obj->active2c = 1;

            for (int t = 0; t < wave; t++) {
                func_02059720(obj);
            }
        }
    }
}
