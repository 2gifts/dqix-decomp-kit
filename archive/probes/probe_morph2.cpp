// Morph from a COMMITTED source that has 0xe7's ladder toward 0xe7 itself.
//
// CheckBitFlagsAcrossEntries_0202bd68 is byte-exact and emits `mov r7,r0` for the saved value and
// gives the pointer DERIVED from it the LOWER register r4 -- exactly what case 0xe7 needs and never
// gets. The one obvious structural difference is that its saved value is a PARAMETER, while 0xe7's
// is a CALL RESULT. Morph one into the other and read off the step where the ladder flips.
//
//   MWCC=2.0/sp2p2 python pad/probe_cc.py pad/probe_morph2.cpp --bytes

#pragma opt_propagation off

extern "C" void *GetSelf(void);
extern "C" int Test(void *, int);
extern "C" void *GetEntry(void *, int);
extern "C" unsigned short GetHalf(void);

// n0: the committed shape -- saved value is a parameter
extern "C" int n0(void *self, int bitIndex)
{
    signed char *arr = (signed char *)((char *)self + 0x38);
    int i = 0;
    arr += 0x1000;
    for (; i < 4; i++) {
        if (i > 0 && *(signed char *)((char *)self + i + 0x1038) == 0)
            continue;
        if (!Test(self, arr[i]))
            continue;
        void *entry = (i == GetHalf()) ? (void *)((char *)self + 0x7c0) : GetEntry(self, arr[i]);
        if (entry == 0)
            continue;
        if (((*(unsigned short *)((char *)entry + 2) << 0x1b) >> 0x1c) & (1 << bitIndex))
            return 0;
    }
    return 1;
}

// n1: the same body, but the saved value comes from a CALL
extern "C" int n1(int bitIndex)
{
    void *self = GetSelf();
    signed char *arr = (signed char *)((char *)self + 0x38);
    int i = 0;
    arr += 0x1000;
    for (; i < 4; i++) {
        if (i > 0 && *(signed char *)((char *)self + i + 0x1038) == 0)
            continue;
        if (!Test(self, arr[i]))
            continue;
        void *entry = (i == GetHalf()) ? (void *)((char *)self + 0x7c0) : GetEntry(self, arr[i]);
        if (entry == 0)
            continue;
        if (((*(unsigned short *)((char *)entry + 2) << 0x1b) >> 0x1c) & (1 << bitIndex))
            return 0;
    }
    return 1;
}

// n2: call result, no loop -- a guarded block, as 0xe7 has
extern "C" int n2(void)
{
    void *self = GetSelf();
    signed char *arr = (signed char *)((char *)self + 0x38);
    arr += 0x1000;
    if (arr[0] == 0) {
        void *e = GetEntry(self, arr[1]);
        Test(self, arr[2]);
        Test(e, arr[3]);
        arr[4] = 1;
        arr[5] = 2;
    }
    return 1;
}

// n4/n5: the flip is parameter-vs-call-result (n0 against n1). If an INLINED helper that takes the
// value as a parameter keeps the parameter colouring after inlining, that is the shape the ROM's
// source had. Needs -inline auto: with the project's -inline noauto mwcc leaves these out of line.
static int body_n4(void *self, int bitIndex)
{
    signed char *arr = (signed char *)((char *)self + 0x38);
    int i = 0;
    arr += 0x1000;
    for (; i < 4; i++) {
        if (i > 0 && *(signed char *)((char *)self + i + 0x1038) == 0)
            continue;
        if (!Test(self, arr[i]))
            continue;
        void *entry = (i == GetHalf()) ? (void *)((char *)self + 0x7c0) : GetEntry(self, arr[i]);
        if (entry == 0)
            continue;
        if (((*(unsigned short *)((char *)entry + 2) << 0x1b) >> 0x1c) & (1 << bitIndex))
            return 0;
    }
    return 1;
}

extern "C" int n4(int bitIndex)
{
    return body_n4(GetSelf(), bitIndex);
}

// n5/n6/n7: n1's shape, varying how many PARAMETERS are still live where the call result is saved.
// n0 vs n1 showed the pair flips on parameter-vs-call-result; if a live parameter at that point is
// what does it, that is something case 0xe7 can be given.
extern "C" int n5(void *keep)
{
    void *self = GetSelf();
    signed char *arr = (signed char *)((char *)self + 0x38);
    arr += 0x1000;
    if (arr[0] == 0) {
        void *e = GetEntry(self, arr[1]);
        arr[4] = arr[2];
        arr[5] = arr[3];
        Test(e, arr[6]);
    }
    return Test(keep, arr[7]);
}

extern "C" int n6(void *keep, void *keep2)
{
    void *self = GetSelf();
    signed char *arr = (signed char *)((char *)self + 0x38);
    arr += 0x1000;
    if (arr[0] == 0) {
        void *e = GetEntry(self, arr[1]);
        arr[4] = arr[2];
        arr[5] = arr[3];
        Test(e, arr[6]);
    }
    return Test(keep, arr[7]) + Test(keep2, arr[8]);
}

extern "C" int n7(void)
{
    void *self = GetSelf();
    signed char *arr = (signed char *)((char *)self + 0x38);
    arr += 0x1000;
    if (arr[0] == 0) {
        void *e = GetEntry(self, arr[1]);
        arr[4] = arr[2];
        arr[5] = arr[3];
        Test(e, arr[6]);
    }
    return Test(self, arr[7]);
}

// n8/n9: is "parameter-like" really "live from function entry"? Give the call result a variable
// that is already live when the function starts, and see whether it colours like n0's parameter.
extern "C" int n8(int flag)
{
    void *self = 0;
    signed char *arr;
    if (flag) {
        self = GetSelf();
        arr = (signed char *)((char *)self + 0x38);
        arr += 0x1000;
        arr[4] = arr[2];
        arr[5] = arr[3];
        Test(self, arr[6]);
        return Test(self, arr[7]);
    }
    return 0;
}

extern "C" int n9(int flag)
{
    void *self = GetSelf();
    signed char *arr = (signed char *)((char *)self + 0x38);
    arr += 0x1000;
    if (flag) {
        arr[4] = arr[2];
        arr[5] = arr[3];
        Test(self, arr[6]);
        return Test(self, arr[7]);
    }
    return 0;
}

// n3: n2 with the derived pointer used far more than the saved value, as 0xe7 has
extern "C" int n3(void)
{
    void *self = GetSelf();
    signed char *arr = (signed char *)((char *)self + 0x38);
    arr += 0x1000;
    if (arr[0] == 0) {
        void *e = GetEntry(self, 0);
        arr[4] = arr[1];
        arr[5] = arr[2];
        arr[6] = arr[3];
        arr[7] = arr[8];
        Test(e, arr[9]);
    }
    return 1;
}
