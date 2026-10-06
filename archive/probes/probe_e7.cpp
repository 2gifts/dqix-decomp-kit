// Isolate 02061c04 case 0xe7's callee-saved ladder.
//
//   ROM:  battle=r5  rec=r4  snap=r6  fields=r7
//   ours: battle=r4  rec=r5  snap=r6  fields=r7
//
// `rec` is derived from `battle`, so `battle` is always DEFINED first -- and the measured rule is
// that callee-saved registers follow definition order. The ROM violates it, so something else
// decides. Each fN() is one hypothesis.
//
//   MWCC=2.0/sp2p2 python pad/probe_cc.py pad/probe_e7.cpp --bytes

#pragma opt_propagation off

struct Snap { unsigned int w[8]; };
extern "C" void *GetBattle(void);
extern "C" void *GetFields(void *);
extern "C" void *GetComb(void *);
extern "C" void Copy(void *, void *);
extern "C" int Pct(void *);

static void body(Snap *snap, char *rec, void *fields, unsigned short level)
{
    Copy(snap, rec + 0x3c);
    Copy((char *)snap + 4, rec + 0x40);
    snap->w[2] = Pct(rec + 0x3c);
    snap->w[3] = Pct(rec + 0x40);
    snap->w[4] = *(unsigned int *)(rec + 0x44) + level;
    snap->w[5] = *(unsigned int *)((char *)fields + 0xf6c);
    snap->w[6] |= 0x1000000;
}

extern "C" int f0(void)                         // the form the big function has
{
    void *battle = GetBattle();
    char *e7base = (char *)battle + 0x104;
    char *rec = e7base + 0x7400;
    int snapOff = 0xcc;
    Snap *snap = (Snap *)(rec + snapOff);
    if (!(snap->w[6] & 0x1000000)) {
        void *fields = GetFields(battle);
        unsigned short level = 0;
        void *comb = GetComb(battle);
        if (comb)
            level = *(unsigned short *)((char *)comb + 0x30);
        body(snap, rec, fields, level);
    }
    return 1;
}

extern "C" int f1(void)                         // battle re-derived from rec (defines rec first)
{
    int snapOff = 0xcc;
    char *rec = (char *)GetBattle() + 0x104 + 0x7400;
    void *battle = rec - 0x7504;
    Snap *snap = (Snap *)(rec + snapOff);
    if (!(snap->w[6] & 0x1000000)) {
        void *fields = GetFields(battle);
        unsigned short level = 0;
        void *comb = GetComb(battle);
        if (comb)
            level = *(unsigned short *)((char *)comb + 0x30);
        body(snap, rec, fields, level);
    }
    return 1;
}

extern "C" int f2(void)                         // snap defined before rec
{
    void *battle = GetBattle();
    int snapOff = 0xcc;
    Snap *snap = (Snap *)((char *)battle + 0x104 + 0x7400 + snapOff);
    char *rec = (char *)snap - snapOff;
    if (!(snap->w[6] & 0x1000000)) {
        void *fields = GetFields(battle);
        unsigned short level = 0;
        void *comb = GetComb(battle);
        if (comb)
            level = *(unsigned short *)((char *)comb + 0x30);
        body(snap, rec, fields, level);
    }
    return 1;
}

extern "C" int f3(void)                         // battle's last use moved earlier
{
    void *battle = GetBattle();
    char *rec = (char *)battle + 0x104 + 0x7400;
    int snapOff = 0xcc;
    Snap *snap = (Snap *)(rec + snapOff);
    void *fields = GetFields(battle);
    void *comb = GetComb(battle);
    if (!(snap->w[6] & 0x1000000)) {
        unsigned short level = 0;
        if (comb)
            level = *(unsigned short *)((char *)comb + 0x30);
        body(snap, rec, fields, level);
    }
    return 1;
}

extern "C" int f5(void)                         // a COPY of the call result, made after rec
{
    void *b0 = GetBattle();
    char *rec = (char *)b0 + 0x104 + 0x7400;
    void *battle = b0;
    int snapOff = 0xcc;
    Snap *snap = (Snap *)(rec + snapOff);
    if (!(snap->w[6] & 0x1000000)) {
        void *fields = GetFields(battle);
        unsigned short level = 0;
        void *comb = GetComb(battle);
        if (comb)
            level = *(unsigned short *)((char *)comb + 0x30);
        body(snap, rec, fields, level);
    }
    return 1;
}

extern "C" int f6(void)                         // the copy made inside the guarded block
{
    void *b0 = GetBattle();
    char *rec = (char *)b0 + 0x104 + 0x7400;
    int snapOff = 0xcc;
    Snap *snap = (Snap *)(rec + snapOff);
    if (!(snap->w[6] & 0x1000000)) {
        void *battle = b0;
        void *fields = GetFields(battle);
        unsigned short level = 0;
        void *comb = GetComb(battle);
        if (comb)
            level = *(unsigned short *)((char *)comb + 0x30);
        body(snap, rec, fields, level);
    }
    return 1;
}

extern "C" int f7(void)                         // early return instead of a guarded block
{
    void *battle = GetBattle();
    char *rec = (char *)battle + 0x104 + 0x7400;
    int snapOff = 0xcc;
    Snap *snap = (Snap *)(rec + snapOff);
    if (snap->w[6] & 0x1000000)
        return 1;
    void *fields = GetFields(battle);
    unsigned short level = 0;
    void *comb = GetComb(battle);
    if (comb)
        level = *(unsigned short *)((char *)comb + 0x30);
    body(snap, rec, fields, level);
    return 1;
}

extern "C" int f8(void)                         // level declared before fields
{
    void *battle = GetBattle();
    char *rec = (char *)battle + 0x104 + 0x7400;
    int snapOff = 0xcc;
    Snap *snap = (Snap *)(rec + snapOff);
    if (!(snap->w[6] & 0x1000000)) {
        unsigned short level = 0;
        void *fields = GetFields(battle);
        void *comb = GetComb(battle);
        if (comb)
            level = *(unsigned short *)((char *)comb + 0x30);
        body(snap, rec, fields, level);
    }
    return 1;
}

extern "C" int f9(void)                         // comb not a variable
{
    void *battle = GetBattle();
    char *rec = (char *)battle + 0x104 + 0x7400;
    int snapOff = 0xcc;
    Snap *snap = (Snap *)(rec + snapOff);
    if (!(snap->w[6] & 0x1000000)) {
        void *fields = GetFields(battle);
        unsigned short level = 0;
        if (GetComb(battle))
            level = 7;
        body(snap, rec, fields, level);
    }
    return 1;
}

extern "C" int f10(void)                        // rec also read in the guard
{
    void *battle = GetBattle();
    char *rec = (char *)battle + 0x104 + 0x7400;
    int snapOff = 0xcc;
    Snap *snap = (Snap *)(rec + snapOff);
    if (!(snap->w[6] & 0x1000000) && *(unsigned int *)(rec + 0x44)) {
        void *fields = GetFields(battle);
        unsigned short level = 0;
        void *comb = GetComb(battle);
        if (comb)
            level = *(unsigned short *)((char *)comb + 0x30);
        body(snap, rec, fields, level);
    }
    return 1;
}

extern "C" int f4(void)                         // fields taken from rec, so battle dies sooner
{
    void *battle = GetBattle();
    char *rec = (char *)battle + 0x104 + 0x7400;
    int snapOff = 0xcc;
    Snap *snap = (Snap *)(rec + snapOff);
    if (!(snap->w[6] & 0x1000000)) {
        void *fields = GetFields(battle);
        unsigned short level = 0;
        void *comb = GetComb(fields);
        if (comb)
            level = *(unsigned short *)((char *)comb + 0x30);
        body(snap, rec, fields, level);
    }
    return 1;
}
