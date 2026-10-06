#include <globaldefs.h>
#include "Memory/SafeAllocator.h"
#include "Memory/SignedAllocator.h"

extern "C" int rand(void);
extern "C" void func_02034bc4(void* obj);
extern "C" void func_02036d88(void* srcObj, void* dstObj);
extern "C" void SetFlag0x10000InField0x6c(unsigned char* obj);

struct S02055080;
extern "C" void* GetSubField0x14(struct S02055080* p);
extern "C" void* GetSubField0x18(struct S02055080* p);
extern "C" void* GetSubField0xC(struct S02055080* p);
extern "C" void* GetField0x4Field0x1cOrNull(struct S02055080* obj);
extern "C" void* GetField0x4Field0x20OrNull(struct S02055080* obj);

struct Obj020370a0;
extern "C" int func_020370a0(struct Obj020370a0* obj, int id, int flags);

struct S02055080 {
    SafeAllocator* alloc;          // 0x0
    void* fieldSub;                // 0x4
    SignedAllocatorList listB[3];  // 0x8
    SignedAllocatorList listC[3];  // 0x2c
    SignedAllocatorList listD[3];  // 0x50
    SignedAllocatorList listE;     // 0x74
    SignedAllocatorList listF[3];  // 0x80
    SignedAllocatorList listG;     // 0xa4
    SignedAllocatorList listH[2];  // 0xb0
    SignedAllocatorList listI[2];  // 0xc8
    SignedAllocatorList listJ;     // 0xe0
    SignedAllocatorList listK[4];  // 0xec
    SignedAllocatorList listA;     // 0x11c
};

struct WeatherFlags14 {
    unsigned char f0;
    unsigned char f1;
    unsigned char f2;
    unsigned char f3;
    unsigned char f4;
    unsigned char f5;
    unsigned char f6;
};

struct RArrCount2_02058d40 {
    void* arr[2];
    unsigned char count[2];
};

struct Cfg0xC_02058d40 {
    char pad0[0x17];
    unsigned char f17;
};

struct Cfg0x20_02058d40 {
    short base;
    unsigned char jitterRange;
    char pad3;
    void* arr;
    unsigned char count;
};

struct Pair0xb0_02058d40 {
    unsigned int a;
    unsigned int b;
};
struct NodePair0xb0_02058d40 {
    Pair0xb0_02058d40* p;
};

struct PointRGA_02058d40 {
    unsigned int x;
    float r;
    float g;
    float amp;
};
struct NodeRGA_02058d40 {
    PointRGA_02058d40* p;
};

struct PointHue_02058d40 {
    int x;
    float val;
    float amp;
};
struct NodeHue_02058d40 {
    PointHue_02058d40* p;
};

struct Obj02058d40 {
    char pad0[0x4];
    int field4;
    int field8;
    unsigned char fieldC;
    char pad1[0x28 - 0xd];
    S02055080* weatherList;        // 0x28
    char pad2[0x30 - 0x2c];
    float field30;
    float field34;
    float field38;
    float field3c;
    float field40;
    float field44;
    float field48;
    float field4c;
    float field50;
    float field54;
    float field58;
    float field5c;
    float field60;
    unsigned short field64;
    unsigned short field66;
    unsigned short field68;
    unsigned short field6a;
    char pad4[0x78 - 0x6c];
    float field78;
    char sub7c[0x12c - 0x7c];
    float field12c;
    float field130;
    float field134;
    float field138;
    float field13c;
    float field140;
    float field144;
};

// SCRATCH-USA: func_02058d40
extern "C" ARM void func_02058d40(struct Obj02058d40* self, struct S02055080* weatherList, struct Obj020370a0* templateObj, unsigned int timeCursor) {
    self->weatherList = weatherList;
    func_02034bc4(self->sub7c);
    func_02036d88(templateObj, self->sub7c);
    SetFlag0x10000InField0x6c((unsigned char*)self->sub7c);

    struct WeatherFlags14* flags = (struct WeatherFlags14*)GetSubField0x14(self->weatherList);
    if (flags->f3 != 0) {
        self->fieldC = 1;
    } else {
        unsigned char b0 = flags->f0;
        int randVal = rand();
        float b0f = (float)b0;
        unsigned char b1 = flags->f1;
        int rem = randVal % b1;
        float fracF = (float)(-rem) / 100.0f;
        self->field8 = b0 + (int)(b0f * fracF);
        self->field4 = 0;
        self->field30 = (float)(unsigned int)self->field8 / (float)(unsigned int)flags->f0;
    }

    struct RArrCount2_02058d40* h0cfg = (struct RArrCount2_02058d40*)GetSubField0x18(self->weatherList);
    struct NodePair0xb0_02058d40* h0node1 = (struct NodePair0xb0_02058d40*)self->weatherList->listH[0].ElementAfter(NULL);
    self->field12c = (float)(unsigned int)(h0node1->p->b & 0x1f);
    self->field130 = (float)(unsigned int)((h0node1->p->b & 0x3e0) >> 5);
    self->field134 = (float)(unsigned int)((h0node1->p->b & 0x7c00) >> 0xa);

    if (h0cfg->count[0] > 1) {
        self->field64 = 1;
        struct NodePair0xb0_02058d40* h0node2 = (struct NodePair0xb0_02058d40*)self->weatherList->listH[0].GetNthElement(1);
        if (h0node2 != NULL) {
            unsigned int packed2 = h0node2->p->b;
            unsigned int g2raw = (packed2 & 0x3e0) >> 5;
            unsigned int b2raw = (packed2 & 0x7c00) >> 0xa;
            unsigned int r2raw = packed2 & 0x1f;
            self->field34 = ((float)r2raw - self->field12c) /
                             (float)((int)h0node2->p->a - (int)h0node1->p->a);
            self->field38 = ((float)g2raw - self->field130) /
                             (float)((int)h0node2->p->a - (int)h0node1->p->a);
            self->field3c = ((float)b2raw - self->field134) /
                             (float)((int)h0node2->p->a - (int)h0node1->p->a);
        }
    }

    struct NodePair0xb0_02058d40* h1node1 = (struct NodePair0xb0_02058d40*)self->weatherList->listH[1].ElementAfter(NULL);
    self->field138 = (float)(unsigned int)h1node1->p->b;

    if (h0cfg->count[1] > 1) {
        self->field66 = 1;
        struct NodePair0xb0_02058d40* h1node2 = (struct NodePair0xb0_02058d40*)self->weatherList->listH[1].GetNthElement(1);
        if (h1node2 != NULL) {
            float x1f = (float)(unsigned int)h1node1->p->a * self->field30;
            float x2f = (float)(unsigned int)h1node2->p->a * self->field30;
            float diffF = (float)((int)h1node2->p->b - (int)h1node1->p->b);
            self->field40 = diffF / (x2f - x1f);
        }
    }

    struct RArrCount2_02058d40* i0cfg = (struct RArrCount2_02058d40*)GetField0x4Field0x1cOrNull(self->weatherList);
    struct NodeRGA_02058d40* i0cur = (struct NodeRGA_02058d40*)self->weatherList->listI[0].ElementAfter(NULL);
    if (i0cur != NULL) {
        struct NodeRGA_02058d40* i0prev = NULL;
        unsigned int i0idx = 0;
        unsigned int i0count = 0;
        goto i0_check;
        for (;;) {
            if (i0cur->p->x >= timeCursor) break;
            i0idx = i0idx + 1;
            i0prev = i0cur;
            i0cur = (struct NodeRGA_02058d40*)self->weatherList->listI[0].GetNthElement(i0idx);
            if (i0cur == NULL) break;
        i0_check:
            i0count = i0count + 1;
            if (i0count >= i0cfg->count[0]) break;
        }

        if (i0cur == NULL) {
            struct NodeRGA_02058d40* prevW = (struct NodeRGA_02058d40*)i0prev;
            int ampInt = (int)prevW->p->amp;
            int jr = (rand() & (ampInt << 1)) - (int)prevW->p->amp;
            float jitter = (float)jr / 100.0f;
            self->field50 = prevW->p->r + prevW->p->r * jitter;
            self->field54 = prevW->p->g + prevW->p->g * jitter;
        } else if (i0prev == NULL) {
            struct NodeRGA_02058d40* curW = (struct NodeRGA_02058d40*)i0cur;
            float rVal = curW->p->r;
            float gVal = curW->p->g;
            int ampInt = (int)curW->p->amp;
            int jr = (rand() & (ampInt << 1)) - (int)curW->p->amp;
            float jitter = (float)jr / 100.0f;
            self->field50 = rVal + rVal * jitter;
            self->field54 = gVal + gVal * jitter;
        } else {
            struct NodeRGA_02058d40* prevW = (struct NodeRGA_02058d40*)i0prev;
            struct NodeRGA_02058d40* curW = (struct NodeRGA_02058d40*)i0cur;
            float prevR = prevW->p->r;
            int prevAmpFix2 = (int)prevW->p->amp;
            float prevG = prevW->p->g;
            int prevAmpFix1 = (int)prevW->p->amp;
            int rand1 = rand();
            int prevJr = (rand1 & (prevAmpFix1 << 1)) - prevAmpFix2;
            float prevJitter = (float)prevJr / 100.0f;

            float curR = curW->p->r;
            float curG = curW->p->g;
            int curAmpFix1 = (int)curW->p->amp;
            int rand2 = rand();
            int curAmpFix2 = (int)curW->p->amp;
            int curJr = (rand2 & (curAmpFix1 << 1)) - curAmpFix2;
            float curJitter = (float)curJr / 100.0f;

            int prevX = (int)prevW->p->x;
            int curX = (int)curW->p->x;
            int span = curX - prevX;
            int diffTime = (int)(timeCursor - (unsigned int)prevX);

            float jitterSlope = (curJitter - prevJitter) / (float)span;
            float jitterAtTime = prevJitter + (float)diffTime * jitterSlope;

            float rSlope = (curR - prevR) / (float)span;
            float baseR = prevR + (float)diffTime * rSlope;
            self->field50 = baseR;

            float gSlope = (curG - prevG) / (float)span;
            float baseG = prevG + (float)diffTime * gSlope;
            self->field54 = baseG;

            self->field50 = self->field50 + self->field50 * jitterAtTime;
            self->field54 = self->field54 + self->field54 * jitterAtTime;
        }
    }

    struct NodeRGA_02058d40* i1node1 = (struct NodeRGA_02058d40*)self->weatherList->listI[1].ElementAfter(NULL);
    if (i1node1 != NULL) {
        float amp2 = (int)(i1node1->p->amp * 2.0f);
        int jr1 = ((int)amp2 & rand()) - (int)i1node1->p->amp;
        float jitter1 = (float)jr1 / 100.0f;
        self->field13c = i1node1->p->r + i1node1->p->r * jitter1;
        self->field140 = i1node1->p->g + i1node1->p->g * jitter1;

        if (i0cfg->count[1] > 1) {
            self->field68 = 1;
            struct NodeRGA_02058d40* i1node2 = (struct NodeRGA_02058d40*)self->weatherList->listI[1].GetNthElement(1);
            if (i1node2 != NULL) {
                int ampFix1 = (int)i1node2->p->amp;
                int rand3 = rand();
                int ampFix2 = (int)i1node2->p->amp;
                int jr2 = (rand3 & (ampFix1 << 1)) - ampFix2;
                float jitter2 = (float)jr2 / 100.0f;
                self->field58 = i1node2->p->r + i1node2->p->r * jitter2;
                self->field5c = i1node2->p->g + i1node2->p->g * jitter2;

                int span2 = (int)i1node2->p->x - (int)i1node1->p->x;
                self->field44 = (self->field58 - self->field13c) / (float)span2;
                self->field48 = (self->field5c - self->field140) / (float)span2;
            }
        }
    }

    struct Cfg0x20_02058d40* hueCfg = (struct Cfg0x20_02058d40*)GetField0x4Field0x20OrNull(self->weatherList);
    struct SignedAllocatorHeader* j0node1 = self->weatherList->listJ.ElementAfter(NULL);
    {
        int jr = (rand() & (hueCfg->jitterRange << 1)) - hueCfg->jitterRange;
        float jitter = (float)jr / 100.0f;
        float baseHue = (float)hueCfg->base;
        baseHue = (float)hueCfg->base;
        self->field144 = baseHue + baseHue * jitter;
    }
    if (self->field144 < 0.0f) self->field144 = self->field144 + 360.0f;
    if (self->field144 > 360.0f) self->field144 = self->field144 - 360.0f;

    if (hueCfg->count > 1) {
        self->field6a = 1;
        struct SignedAllocatorHeader* j0node2 = self->weatherList->listJ.GetNthElement(1);
        if (j0node2 != NULL) {
            struct NodeHue_02058d40* n2 = (struct NodeHue_02058d40*)j0node2;
            int ampFix1 = (int)n2->p->amp;
            int randVal2 = rand();
            int ampFix2 = (int)n2->p->amp;
            int jr2 = (randVal2 & (ampFix1 << 1)) - ampFix2;
            float jitter2 = (float)jr2 / 100.0f;
            self->field60 = n2->p->val + n2->p->val * jitter2;

            struct NodeHue_02058d40* n1 = (struct NodeHue_02058d40*)j0node1;
            int span3 = n2->p->x - n1->p->x;
            self->field4c = self->field60 / (float)span3;

            if (self->field4c < 0.0f) self->field4c = self->field4c + 360.0f;
            if (self->field4c > 360.0f) self->field4c = self->field4c - 360.0f;
        }
    }

    struct WeatherFlags14* flags2 = (struct WeatherFlags14*)GetSubField0x14(self->weatherList);
    if (flags2->f4 != 0) {
        func_020370a0((struct Obj020370a0*)self->sub7c, 0, 0);
    } else {
        func_020370a0((struct Obj020370a0*)self->sub7c, 0, 1);
    }

    if (flags2->f6 != 0) {
        if (rand() % 2 != 0) {
            self->field4c = 0.0f - self->field4c;
        }
    }

    struct Cfg0xC_02058d40* cCfg = (struct Cfg0xC_02058d40*)GetSubField0xC(self->weatherList);
    unsigned char f17 = cCfg->f17;
    int rem2 = rand() % f17;
    float scale = (float)(100 - rem2) * 0.01f;
    self->field78 = self->field78 * scale;
}
