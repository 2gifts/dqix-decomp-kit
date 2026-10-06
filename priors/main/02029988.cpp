#include <globaldefs.h>
#include "std_library_functions.h"

extern "C" void func_02029634(void);

struct IndexNode_0202a9ac { char unk[0x34]; struct IndexNode_0202a9ac* next; };
struct IndexList_0202a9ac { char unk[0x44]; struct IndexNode_0202a9ac* head; char unk2[0x12]; unsigned short count; };
extern "C" struct IndexNode_0202a9ac* GetNodeAtIndex(struct IndexList_0202a9ac* list, int index);

extern "C" void SelectCoordsByFlag0x24(unsigned char* obj, int* out1, int* out2);
extern "C" int _s32_div_f(int a, int b);

struct Callback_0202a91c;
extern "C" void SetState0x6cAndInvoke(struct Callback_0202a91c* obj);
extern "C" void Deactivate0202a8cc(char* obj, int flag);
extern "C" void func_0202a6c4(void* obj);
extern "C" void FormatAndDispatch0202ad98(int a, int b, int c, const char* fmt, ...);
extern "C" void FormatAndDispatch020290e8(int a, int b, const char* fmt, ...);
extern "C" int TestFlag0SetAndFlag1Clear(unsigned short* obj, int mask);
extern "C" int TestFlagMask(unsigned short* obj, int mask);
struct Obj0201248c;
extern "C" int func_0201248c(struct Obj0201248c* obj, int mask);
struct WrapField_0202a678 { char unk[0x40]; int value; int lower; int upper; unsigned short flags; };
extern "C" void AdjustValueWrapOrClamp(struct WrapField_0202a678* obj, int delta);
extern "C" void* func_0200f374(void* dst, int count);
extern "C" int SetupViewport02029384(int a0, int a1, int a2, int a3);
extern "C" void PackRegister02029470(int a, int b, int c, int d, int e, int f);
extern "C" void func_020292ec(int a0, int a1, int a2, int a3, int a4);

extern unsigned char data_02114e54;
struct DivModeState_020ef74c { int f0; int f4; int f8; int mode; };
extern struct DivModeState_020ef74c data_020ef74c;
struct FbToggle_020fe9a4 { void* buf; int flag; };
extern struct FbToggle_020fe9a4 data_020fe9a4;
extern unsigned short data_02114e30;
extern const char data_020ef75c;   // "%s"
extern const char data_020ef75f;   // "<>>"
extern const char data_020ef763;   // "   "
extern const char data_020ef767;   // "[*]"
extern const char data_020ef76b;   // "[-]"
extern const char data_020ef76f;
extern const char data_020ef772;
extern const char data_020ef775;
extern const char data_020ef778;   // "[%x]"
extern const char data_020ef77d;   // "[%c]"
extern const char data_020ef782;   // "[%d]"

#define S16(p,o) (*(short*)((char*)(p)+(o)))
#define U16(p,o) (*(unsigned short*)((char*)(p)+(o)))
#define S8(p,o)  (*(unsigned char*)((char*)(p)+(o)))
#define S32(p,o) (*(int*)((char*)(p)+(o)))

// SCRATCH-USA: func_02029988
extern "C" ARM int UpdateAndRenderDebugList_02029988(char* obj, int p1) {
    struct IndexNode_0202a9ac* node0;
    struct IndexNode_0202a9ac* node;
    struct IndexNode_0202a9ac* node5;
    int d1, d2, d3, d4, fixA, fpVal;
    int local2c;
    float t;
    int row, col;
    int v4c, v48, v44, v40, v3c, v38;
    unsigned char* g;
    char buf160[0x80];
    char bufE0[0x80];
    char buf60[0x80];
    char buf50[0x10];

    func_02029634();
    if (S8(obj, 0x6d) & 8) {
        return 0;
    }
    if (S8(obj, 0x6c) != 0) {
        goto L_0b00;
    }

    node0 = GetNodeAtIndex((struct IndexList_0202a9ac*)obj, S16(obj, 0x60));
    d1 = S16(obj, 0x5e) - S16(obj, 0x5c);
    d2 = U16(obj, 0x5a) - d1;
    d3 = U16(obj, 0x62) - d2;
    t = (float)S16(obj, 0x56) / (float)d1;
    fixA = (int)(t * (float)d3);
    local2c = S16(obj, 0x50) + S16(obj, 0x54) - 9;
    d4 = U16(obj, 0x64) - S16(obj, 0x5c);
    fpVal = (int)((float)S16(obj, 0x52) + t * (float)d4);

    if (S8(obj, 0x72) == 0) {
        goto L_0414;
    }

    g = ((__typeof__(&data_02114e54))0x02114E54);
    if (g[0x55] != 0) {
        obj[0x71] = 1;
        SelectCoordsByFlag0x24(g, &v4c, &v48);
        if (S16(obj, 0x50) < v4c && v4c < S16(obj, 0x50) + S16(obj, 0x54) - 0xc &&
            S16(obj, 0x52) < v48 && v48 < S16(obj, 0x52) + S16(obj, 0x56)) {
            obj[0x73] = 1;
        } else if (S16(obj, 0x50) + S16(obj, 0x54) - 0xc < v4c && v4c < S16(obj, 0x50) + S16(obj, 0x54) + 4 &&
                   S16(obj, 0x52) < v48 && v48 < S16(obj, 0x52) + S16(obj, 0x56)) {
            obj[0x74] = 1;
        }
        goto L_0414;
    }

    if (obj[0x71] != 0 && obj[0x73] != 0 && g[0x5f] != 0 && S16(g, 0x24) != 0) {
        int q, r1v;
        SelectCoordsByFlag0x24(g, &v44, &v40);
        if (S16(obj, 0x50) < v44 && v44 < S16(obj, 0x50) + S16(obj, 0x54) - 0xc &&
            S16(obj, 0x52) < v40 && v40 < S16(obj, 0x52) + S16(obj, 0x56)) {
            q = _s32_div_f(v40 - S16(obj, 0x52), data_020ef74c.f4);
            r1v = q - 1;
            if (S16(obj, 0x5c) <= r1v && r1v <= S16(obj, 0x5c) + d3) {
                int newv = U16(obj, 0x64) + (r1v - S16(obj, 0x5c));
                if (S16(obj, 0x5c) <= newv && newv <= S16(obj, 0x5e)) {
                    S16(obj, 0x60) = (short)newv;
                }
            }
        }
        goto L_0414;
    }

    if (obj[0x71] != 0 && obj[0x74] != 0 && g[0x5f] != 0 && S16(g, 0x24) != 0) {
        int fpv, half;
        SelectCoordsByFlag0x24(g, &v3c, &v38);
        half = fixA / 2;
        fpv = v38 - half;
        if (fpv < S16(obj, 0x52)) {
            fpv = S16(obj, 0x52);
        } else if (S16(obj, 0x52) + S16(obj, 0x54) < fpv + fixA) {
            fpv = S16(obj, 0x52) + S16(obj, 0x56) - fixA;
        }
        U16(obj, 0x64) = (unsigned int)((float)S16(obj, 0x5c) + (float)(fpv - S16(obj, 0x52)) / t);
        U16(obj, 0x66) = U16(obj, 0x64) + d3;
        if (S16(obj, 0x60) < (short)U16(obj, 0x64)) S16(obj, 0x60) = (short)U16(obj, 0x64);
        if ((short)U16(obj, 0x66) < S16(obj, 0x60)) S16(obj, 0x60) = (short)U16(obj, 0x66);
        goto L_0414;
    }

    if (obj[0x71] != 0 && g[0x54] != 0) {
        int gb1 = S32(g, 0x38);
        int gb0 = S32(g, 0x3c);
        if (obj[0x73] != 0 &&
            S16(obj, 0x50) < gb1 && gb1 < S16(obj, 0x50) + S16(obj, 0x54) - 0xc &&
            S16(obj, 0x52) < gb0 && gb0 < S16(obj, 0x52) + S16(obj, 0x56)) {
            int q = _s32_div_f(gb0 - S16(obj, 0x52), data_020ef74c.f4);
            int r1v = q - 1;
            if (S16(obj, 0x5c) <= r1v && r1v <= S16(obj, 0x5c) + d3) {
                int sum = U16(obj, 0x64) + (r1v - S16(obj, 0x5c));
                if (S16(obj, 0x5c) <= sum && sum <= S16(obj, 0x5e)) {
                    S16(obj, 0x60) = (short)sum;
                    if (S16(obj, 0x60) >= 0 && S32(node0, 0) == 1) {
                        Deactivate0202a8cc((char*)node0, 1);
                        obj[0x6c] = 4;
                        goto L_END;
                    }
                    if (!(S8(obj, 0x6d) & 2)) {
                        SetState0x6cAndInvoke((struct Callback_0202a91c*)obj);
                        obj[0x6e] = 1;
                    }
                    goto L_END;
                }
            }
        }
    }
    obj[0x71] = 0;
    obj[0x73] = 0;
    obj[0x74] = 0;

L_0414:
    obj[0x71] = 0;
    obj[0x73] = 0;
    obj[0x74] = 0;
    if (U16(obj, 0x66) < (unsigned short)S16(obj, 0x60)) {
        U16(obj, 0x64) = (unsigned short)(S16(obj, 0x60) - d3);
        U16(obj, 0x66) = (unsigned short)S16(obj, 0x60);
    } else if ((unsigned short)S16(obj, 0x60) < U16(obj, 0x64)) {
        U16(obj, 0x64) = (unsigned short)S16(obj, 0x60);
        U16(obj, 0x66) = (unsigned short)(S16(obj, 0x60) + d3);
    }

    if (data_020ef74c.mode == 0) {
        FormatAndDispatch0202ad98(S16(obj, 0x50) / 8 + 1, S16(obj, 0x52) / 8 + 2, 7, (const char*)(obj + 4));
    } else if (data_020ef74c.mode >= 3 && data_020ef74c.mode <= 6) {
        data_020fe9a4.flag = 0;
        FormatAndDispatch020290e8(S16(obj, 0x50) + data_020ef74c.f8, S16(obj, 0x52) + data_020ef74c.f4, (const char*)(obj + 4));
    }

    node = GetNodeAtIndex((struct IndexList_0202a9ac*)obj, 0);
    for (row = 0; row < S16(obj, 0x5c); row++) {
        if (node == 0) break;
        sprintf(buf160, &data_020ef75c, (char*)node + 4);
        data_020fe9a4.flag = (S32(node, 0x38) != 0) ? 1 : 0;
        if (data_020ef74c.mode == 0) {
            FormatAndDispatch0202ad98(S16(obj, 0x50) / 8 + 1, S16(obj, 0x52) / 8 + 2 + row, 7, buf160);
        } else if (data_020ef74c.mode >= 3 && data_020ef74c.mode <= 6) {
            FormatAndDispatch020290e8(S16(obj, 0x50) + data_020ef74c.f8, data_020ef74c.f4 * (row + 2) + S16(obj, 0x52), buf160);
        }
        node = *(struct IndexNode_0202a9ac**)((char*)node + 0x34);
    }

    node = GetNodeAtIndex((struct IndexList_0202a9ac*)obj, S16(obj, 0x5e) + 1);
    for (row = S16(obj, 0x5e) + 1; row < U16(obj, 0x5a); row++) {
        if (node == 0) break;
        sprintf(bufE0, &data_020ef75c, (char*)node + 4);
        data_020fe9a4.flag = (S32(node, 0x38) != 0) ? 1 : 0;
        if (data_020ef74c.mode == 0) {
            FormatAndDispatch0202ad98(S16(obj, 0x50) / 8 + 1, S16(obj, 0x52) / 8 + 2 + row, 7, bufE0);
        } else if (data_020ef74c.mode >= 3 && data_020ef74c.mode <= 6) {
            FormatAndDispatch020290e8(S16(obj, 0x50) + data_020ef74c.f8, data_020ef74c.f4 * (row + 2) + S16(obj, 0x52), bufE0);
        }
        node = *(struct IndexNode_0202a9ac**)((char*)node + 0x34);
    }

    node = GetNodeAtIndex((struct IndexList_0202a9ac*)obj, U16(obj, 0x64));
    for (row = U16(obj, 0x64); row <= (unsigned short)U16(obj, 0x66); row++) {
        col = 0;
        if (node == 0) goto L_0b00;
        data_020fe9a4.flag = (S32(node, 0x38) != 0) ? 1 : 0;
        if (S16(obj, 0x5c) <= row && row <= S16(obj, 0x5e)) {
            int mode24 = 0;
            sprintf(buf60, (S16(obj, 0x60) == row) ? &data_020ef75f : &data_020ef763);
            col += 3;

            if (S32(node, 0) == 2) {
                if (S16(obj, 0x60) == row && S32(node, 0x38) == 0 &&
                    TestFlag0SetAndFlag1Clear(&data_02114e30, 0x30)) {
                    S32(node, 0x40) = (S32(node, 0x40) == 0) ? 1 : 0;
                }
                sprintf(buf60 + col, (S32(node, 0x40) != 0) ? &data_020ef767 : &data_020ef76b);
                col += 3;
            }

            func_0200f374(buf50, 0x10);

            if (S32(node, 0) == 3) {
                int adjusted = 0;
                int origVal = S32(node, 0x40);
                U16(node, 0x4c) &= ~2;
                U16(node, 0x4c) &= ~0xc;
                if (S16(obj, 0x60) == row && S32(node, 0x38) == 0) {
                    int sl = TestFlagMask(&data_02114e30, 8) ? 100 : 1;
                    if (func_0201248c((struct Obj0201248c*)&data_02114e30, 0x10)) {
                        AdjustValueWrapOrClamp((struct WrapField_0202a678*)node, sl);
                        adjusted = 1;
                    }
                    if (func_0201248c((struct Obj0201248c*)&data_02114e30, 0x20)) {
                        AdjustValueWrapOrClamp((struct WrapField_0202a678*)node, -sl);
                        adjusted = 1;
                    }
                    if (func_0201248c((struct Obj0201248c*)&data_02114e30, 0x100)) {
                        AdjustValueWrapOrClamp((struct WrapField_0202a678*)node, sl * 10);
                        adjusted = 1;
                    }
                    if (func_0201248c((struct Obj0201248c*)&data_02114e30, 0x200)) {
                        AdjustValueWrapOrClamp((struct WrapField_0202a678*)node, sl * -10);
                        adjusted = 1;
                    }
                }
                if (adjusted) {
                    unsigned short saveA = U16(obj, 0x64);
                    unsigned short saveB = U16(obj, 0x66);
                    func_0202a6c4(obj);
                    U16(obj, 0x64) = saveA;
                    U16(obj, 0x66) = saveB;
                    U16(node, 0x4c) |= 2;
                    if (origVal < S32(node, 0x40)) U16(node, 0x4c) |= 4;
                    if (origVal > S32(node, 0x40)) U16(node, 0x4c) |= 8;
                }

                if (!(U16(node, 0x4c) & 0x40)) {
                    if (U16(node, 0x4c) & 0x20) {
                        if (U16(node, 0x4c) & 1) {
                            sprintf(buf50, &data_020ef778, S32(node, 0x40));
                        } else if (U16(node, 0x4c) & 0x80) {
                            sprintf(buf50, &data_020ef772, S32(node, 0x40) + 0x40);
                        } else {
                            sprintf(buf50, &data_020ef775, S32(node, 0x40));
                        }
                    } else {
                        if (U16(node, 0x4c) & 1) {
                            sprintf(buf50, &data_020ef77d, S32(node, 0x40));
                        } else if (U16(node, 0x4c) & 0x80) {
                            sprintf(buf50, &data_020ef772, S32(node, 0x40) + 0x40);
                        } else {
                            sprintf(buf50, &data_020ef782, S32(node, 0x40));
                            mode24 = 1;
                            if (U16(node, 0x4c) & 0x10) mode24 = 2;
                        }
                    }
                }
            }

            if (mode24 == 1) {
                sprintf(buf60 + col, &data_020ef76f, buf50);
                col += strlen(buf50);
                sprintf(buf60 + col, &data_020ef772, (char*)node + 4);
                col += strlen((char*)node + 4);
            } else if (mode24 == 2) {
                sprintf(buf60 + col, &data_020ef775, buf50);
                col += strlen(buf50);
            }

            if (((__typeof__(&data_020ef74c))0x020EF74C)->mode == 0) {
                FormatAndDispatch0202ad98(S16(obj, 0x50) / 8 + 1, S16(obj, 0x52) / 8 + 2 + (row - U16(obj, 0x64)), 7, buf60);
            } else if (data_020ef74c.mode >= 3 && data_020ef74c.mode <= 6) {
                FormatAndDispatch020290e8(S16(obj, 0x50) + data_020ef74c.f8, data_020ef74c.f4 * (2 + (row - U16(obj, 0x64))) + S16(obj, 0x52), buf60);
            }
        }
        node = *(struct IndexNode_0202a9ac**)((char*)node + 0x34);
    }

L_0b00:
    if (S8(obj, 0x6c) == 4) {
        node5 = GetNodeAtIndex((struct IndexList_0202a9ac*)obj, S16(obj, 0x60));
        if (node5 != 0 && S32(node5, 0) == 1) {
            UpdateAndRenderDebugList_02029988((char*)node5, p1);
            if (S8(node5, 0x6c) == 1) {
                Deactivate0202a8cc(obj, 0);
                switch (S8(obj, 0x6c)) {
                    case 0: goto L_0b78;
                    case 1: goto L_END;
                    case 2: goto L_0c5c;
                    case 3: goto L_0c84;
                    default: goto L_END;
                }
            }
        }
    }
    goto L_END;

L_0b78:
    if (SetupViewport02029384(S16(obj, 0x50), S16(obj, 0x52), S16(obj, 0x54), S16(obj, 0x56))) {
        if ((unsigned short)(U16(obj, 0x66) - U16(obj, 0x64) + 1) < U16(obj, 0x5a)) {
            int a1b, a3b;
            PackRegister02029470(0, 1, 3, 0, 0, 0);
            func_020292ec(local2c, S16(obj, 0x52) + 1, 8, S16(obj, 0x56) - 2, 0x7fff);
            PackRegister02029470(0, 1, 3, 0, 0x1f, 0);
            if (fixA >= 0 && fixA < 0x100) {
                a1b = fpVal;
                a3b = fixA;
            } else {
                a1b = S16(obj, 0x52) + 1;
                a3b = S16(obj, 0x56) - 2;
            }
            func_020292ec(local2c, a1b, 8, a3b, 0x7fff);
        }
    }
    goto L_END;

L_0c5c:
    S16(obj, 0x58) = S16(obj, 0x56);
    obj[0x6c] = 0;
    SetupViewport02029384(S16(obj, 0x50), S16(obj, 0x52), S16(obj, 0x54), S16(obj, 0x58));
    goto L_END;

L_0c84:
    S16(obj, 0x58) = 0;
    obj[0x6c] = 1;
    SetupViewport02029384(S16(obj, 0x50), S16(obj, 0x52), S16(obj, 0x54), S16(obj, 0x58));

L_END:
    return 0;
}
