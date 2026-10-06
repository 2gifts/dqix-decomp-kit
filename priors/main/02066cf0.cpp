#include <globaldefs.h>

// mangled C++ callees -- parameter types must match the existing definitions exactly
// so the symbol mangles identically (see e.g. IsField440Empty.cpp, GetField440XY.cpp).
struct CE28Obj;
void LoadStateBytePair0206b804(char* obj, int* out1, int* out2);
int IsField440Empty(struct CE28Obj* obj);
struct Field440Obj0202f6a4;
void SetupField440Type1(struct Field440Obj0202f6a4* obj);
void UpdateTimedState0202f380(void* obj, int amount);
struct Field440XYObj;
void GetField440XY(struct Field440XYObj* obj, short* outX, short* outY);
struct Holder02042c24;
void SetSubstructXY02042c24(struct Holder02042c24* holder, int x, int y);
void EmitScaledField0x440(void* obj, int a, int b);
void PackWordWithFlag0x40(unsigned int* out, unsigned char a, unsigned char b, unsigned char c, int d);
bool IsCommandInRange02067a9c(void* obj, void* src);
void MeasureTextWidth0206b6b8(unsigned char* obj);
void Forward02044830();
int GetWordFromTable02042648(int idx);
bool IsHighByteFF02044494(void* obj, void* src);
int LookupKeyValue020425e4(int a, int b, int tableIdx);
int EncodeStreamValue020dc0b0(int value);
int GetGlobalField0x1c020421a0();
int EncodeStreamFields020dc0e0(int a, int b, int c, int d, unsigned char e, unsigned char f);
int EncodeStreamValue020dc0c8(int value);
struct Holder020416cc;
int GetField0OfSubstructAt0x18(struct Holder020416cc* holder);
void UpdateObj020e15f8Entry(char* obj, int idx, int a2, int a3, int a4, unsigned char a5, unsigned char a6);
struct SetIntIntShortAt0xcStruct;
void SetIntIntShortAt0xc(struct SetIntIntShortAt0xcStruct* s, int a, int b, short c);
void CallFunc0205c96cIfMode6AndFlag(char* obj, int flag);

// plain (unmangled) callees
extern "C" void func_02066a6c(void* obj, int flag);
extern "C" void func_020e2110(void* target);
extern "C" void func_020675fc(void* obj);
extern "C" void func_0200f374(void* dst, int size);
extern "C" void func_020654b8(void* obj, int cfg, void* glyph, short y, short x, int flags);
extern "C" void func_02045864(int x, int y, void* fp, int streamHandle, int p5, int p6, int p7, unsigned char p8);
extern "C" void func_020417b0(void* glyph, unsigned short val, void* p38, int cfgVal);
extern "C" void func_0204459c(void* obj, int flag);
extern "C" void func_020653e4(int a, int b);
extern "C" unsigned int _fflt(int);
extern "C" unsigned int _fmul(unsigned int, unsigned int);
extern "C" int _ffix(unsigned int);
extern "C" unsigned int _fsub(unsigned int, unsigned int);

extern short data_020e7e54[];
extern short data_020e7e3c[];
extern int data_021077fc;

struct Glyph02066cf0 {
    char pad[0x17];
    unsigned char widthA;
    void* field18;
};

#define P1000(o, off) (*(unsigned char*)((char*)(o) + 0x1000 + (off)))
#define I1000(o, off) (*(int*)((char*)(o) + 0x1000 + (off)))

// SCRATCH-USA: func_02066cf0
extern "C" ARM void func_02066cf0(unsigned char* obj, int flag) {
    if (P1000(obj, 0x9cc) != 0) return;

    int out1, out2;
    LoadStateBytePair0206b804((char*)obj, &out1, &out2);

    int cond9b1 = (*(int*)(obj + 0x998) != 0) && (P1000(obj, 0x9b1) != 0);
    if (cond9b1 || P1000(obj, 0x9cb) != 0) {
        if (P1000(obj, 0x9cb) != 0) {
            if (IsField440Empty((struct CE28Obj*)((char*)obj + 0x9e0 + 0x1000)) != 0) {
                SetupField440Type1((struct Field440Obj0202f6a4*)((char*)obj + 0x9e0 + 0x1000));
                UpdateTimedState0202f380((char*)obj + 0x9e0 + 0x1000, 1);
            }
        }

        if ((*(int*)(obj + 0x998) != 0) && (P1000(obj, 0x9b1) != 0)) {
            func_02066a6c(obj, flag);
        }

        short outX, outY;
        GetField440XY((struct Field440XYObj*)((char*)obj + 0x9e0 + 0x1000), &outX, &outY);
        SetSubstructXY02042c24((struct Holder02042c24*)((char*)obj + 0x9e0 + 0x1000),
                                (short)(outX + out1), (short)(outY + out2));

        if (flag == 0) {
            if (*(int*)(obj + 0x38) != 0) {
                func_020e2110(*(void**)(obj + 0x38));
            } else {
                EmitScaledField0x440((char*)obj + 0x9e0 + 0x1000, 1, 1);
            }
        }

        SetSubstructXY02042c24((struct Holder02042c24*)((char*)obj + 0x9e0 + 0x1000), outX, outY);
    }

    if (*(int*)(obj + 0x998) == 0) return;
    func_020675fc(obj);

    void* p38 = 0;
    if (P1000(obj, 0x9b9) == 1) {
        p38 = obj + 0x48 + 0x1800;
    }

    if (*(int*)(obj + 0x54) == 0 || *(int*)(obj + 0x58) == 0) return;
    if (*(int*)(obj + 0x6c) == 0) return;

    CallFunc0205c96cIfMode6AndFlag((char*)obj, flag);

    int arr94[16];
    int arr54[16];
    int logIdx = 0;
    func_0200f374(arr94, 0x40);
    func_0200f374(arr54, 0x40);

    int cfgVal = 0x1f;
    if (P1000(obj, 0x95b) & 4) {
        int v = *(int*)(obj + 0x9b0) >> 16;
        if (flag != 0) {
            v &= 0x1f;
            if (v != 0) {
                v >>= 1;
                cfgVal = 0xf - v;
            }
            PackWordWithFlag0x40((unsigned int*)0x04000050, 8, 1, (unsigned char)v, cfgVal);
        }
    }

    if (P1000(obj, 0x9c5) != 0) {
        unsigned short chPeek = (*(unsigned short**)(obj + 0x4c))[*(int*)(obj + 0x78)];
        if (!IsCommandInRange02067a9c(obj, &chPeek)) return;
    }

    if (P1000(obj, 0x95b) & 0x40) {
        MeasureTextWidth0206b6b8(obj);
    }

    int i;
    if (P1000(obj, 0x9b8) != 0) {
        Forward02044830();
        for (i = 0; i < 0x10; i++) {
            arr54[i] = I1000(obj, 0x85c);
        }
    } else {
        for (i = 0; i < 0x10; i++) {
            arr94[i] = I1000(obj, 0x858) + 0xc;
            arr54[i] = I1000(obj, 0x85c);
        }
    }

    int endIdx = 0;
    endIdx = endIdx + *(int*)(obj + 0x78);
    int r7 = *(int*)(obj + 0x74);
    int minIdx = *(int*)(obj + 0x88);
    if (r7 < minIdx) r7 = minIdx;

    int lastGlyphWidth = 0xff;
    int tableVal = GetWordFromTable02042648(P1000(obj, 0x9dc));
    int spacingAdj = tableVal & 0xff;
    int field868 = I1000(obj, 0x868);
    int cursorX = arr54[0] - field868;
    int cursorY = arr94[0];

    for (; r7 < endIdx; r7++) {
        unsigned short chBuf = (*(unsigned short**)(obj + 0x4c))[r7];

        if (IsHighByteFF02044494(obj, &chBuf)) {
            if (IsCommandInRange02067a9c(obj, &chBuf)) {
                break;
            }
            if (chBuf == 0xff18) {
                int fld864 = I1000(obj, 0x864);
                logIdx = logIdx + 1;
                cursorY = arr94[logIdx];
                cursorX += fld864;
                lastGlyphWidth = 0xff;
            } else if (chBuf == 0xff19) {
                cursorY += spacingAdj + 1;
                lastGlyphWidth = 0xff;
            }
            continue;
        }

        struct Glyph02066cf0* glyph = (struct Glyph02066cf0*)((char*)obj + 0x1b8 + 0x800) + chBuf;
        cursorY += LookupKeyValue020425e4(lastGlyphWidth, glyph->widthA, P1000(obj, 0x9dc));
        lastGlyphWidth = glyph->widthA;

        if (flag != 0 && *(int*)(obj + 0x40) <= r7) {
            func_020654b8(obj, *(int*)(obj + 0x34), glyph, (short)cursorY, (short)(cursorX + 2), 0xf);
        }

        if (P1000(obj, 0x9ce) != 0) {
            if (*(int*)(obj + 0x44) != *(int*)(obj + 0x78)) {
                int streamHandle = EncodeStreamValue020dc0b0(0x3000);
                if (streamHandle != 0) {
                    void* fielde28 = *(void**)((char*)obj + 0x1000 + 0xe28);
                    unsigned char field9dc2 = P1000(obj, 0x9dc);
                    int field50 = *(int*)((char*)fielde28 + 0x50);
                    void* fp = glyph->field18;
                    int sp18 = (int)(((unsigned int)field50 << 16) >> 13);

                    if (fp != 0) {
                        unsigned char gFlags = *(unsigned char*)((char*)(void*)(int)GetGlobalField0x1c020421a0() + 0x1000 + 0x95b);
                        if (gFlags & 0x80) {
                            int yBase = cursorY;
                            int xBase = cursorX - 0x74;
                            for (int j = 0; j < 4; j = (j + 1) & 0xff) {
                                int xArg = data_020e7e54[j] + (short)yBase;
                                int yArg = data_020e7e3c[j] + (short)xBase;
                                func_02045864(xArg, yArg, fp, streamHandle, 0x100, 0x60, 0xf, field9dc2);
                            }
                            func_02045864((short)yBase + 1, (short)xBase + 1, fp, streamHandle, 0x100, 0x60, 0xf, field9dc2);
                        } else if (gFlags & 2) {
                            int xArg = (short)cursorY + 1;
                            int yArg = (short)(cursorX - 0x74) + 1;
                            func_02045864(xArg, yArg, fp, streamHandle, 0x100, 0x60, 0xf, field9dc2);
                            func_02045864(xArg, yArg, fp, streamHandle, 0x100, 0x60, 0xf, field9dc2);
                        } else {
                            func_02045864((short)cursorY, (short)(cursorX - 0x74), fp, streamHandle, 0x100, 0x60, 0xf, field9dc2);
                        }
                    }

                    EncodeStreamFields020dc0e0(1, streamHandle, sp18, 0x3000, 1, 0);
                    EncodeStreamValue020dc0c8(streamHandle);
                }
            }
        } else {
            if (*(int*)(obj + 0x38) != 0) {
                int a4 = GetField0OfSubstructAt0x18((struct Holder020416cc*)glyph);
                UpdateObj020e15f8Entry(*(char**)(obj + 0x38), 0, cursorY, cursorX - 0x74, a4, 0xf, P1000(obj, 0x9dc));
            } else {
                SetIntIntShortAt0xc((struct SetIntIntShortAt0xcStruct*)glyph, cursorY, cursorX, 0);
            }
            func_020417b0(glyph, *(unsigned short*)(obj + 0x1800 + 0x7c), p38, cfgVal);
        }

        if (glyph->field18 != 0) {
            int adv = *(signed char*)((char*)glyph->field18 + 4);
            cursorY += adv + 1;
        }
    }

    func_0204459c(obj, flag);
    *(int*)(obj + 0x40) = endIdx;

    if (P1000(obj, 0x9ce) == 0) return;
    void* fielde28 = *(void**)((char*)obj + 0x1000 + 0xe28);
    if (fielde28 == 0) return;

    int field50 = *(int*)((char*)fielde28 + 0x50);

    *(volatile unsigned int*)0x04000444 = *(volatile unsigned int*)0x04000470 = 0;
    *(volatile unsigned int*)0x04000470 = 0x74000;
    *(volatile unsigned int*)0x04000470 = 0x400000;
    *(volatile unsigned int*)0x0400046c = 0x1000;
    *(volatile unsigned int*)0x0400046c = 0x1000;
    *(volatile unsigned int*)0x0400046c = 0x1000;

    *(volatile unsigned int*)0x04000444 = 0;
    *(volatile unsigned int*)0x04000470 = 0;
    *(volatile unsigned int*)0x04000470 = 0x80000;
    *(volatile unsigned int*)0x04000470 = 0;
    *(volatile unsigned int*)0x0400046c = 0x1000;
    *(volatile unsigned int*)0x0400046c = (unsigned int)(0x80000 - 0x81000);
    *(volatile unsigned int*)0x0400046c = 0x1000;

    *(volatile unsigned int*)0x04000444 = 0;
    *(volatile unsigned int*)0x040004c0 = (unsigned int)(0 - 0x80000001);
    *(volatile unsigned int*)0x040004c4 = 0x4210;
    *(volatile unsigned int*)0x040004a8 = 0x6e500000u | (unsigned short)field50;
    *(volatile unsigned int*)0x040004ac = (unsigned int)data_021077fc >> 4;
    *(volatile unsigned int*)0x040004a4 = 0x3e1f00c0;

    *(volatile unsigned int*)0x0400046c = 0x100000;
    *(volatile unsigned int*)0x0400046c = 0x80000;
    *(volatile unsigned int*)0x0400046c = 0x1000;

    int vtxType = 1;
    *(volatile unsigned int*)0x04000500 = vtxType;
    *(volatile unsigned int*)0x04000480 = 0x8000 - vtxType;

    unsigned int t = 0;
    int scaledT = _ffix(_fmul(_fflt(0x80), t));
    scaledT = scaledT << 12;
    func_020653e4(0, scaledT);

    unsigned int comp = _fsub(0x3f800000u, t);

    unsigned int packed1 = ((unsigned int)(unsigned short)_ffix(_fmul(0x45800000u, comp))) << 16;
    *(volatile unsigned int*)0x0400048c = packed1;
    *(volatile unsigned int*)0x0400048c = 0;
    func_020653e4(0x100000, scaledT);

    unsigned int packed2 = ((unsigned int)(unsigned short)_ffix(_fmul(0x45800000u, comp))) << 16;
    packed2 |= 0x1000;
    *(volatile unsigned int*)0x0400048c = packed2;
    *(volatile unsigned int*)0x0400048c = 0;
    func_020653e4(0x100000, 0x80000);

    *(volatile unsigned int*)0x0400048c = 0x1000;
    *(volatile unsigned int*)0x0400048c = 0;
    func_020653e4(0, 0x80000);

    *(volatile unsigned int*)0x0400048c = 0;
    *(volatile unsigned int*)0x0400048c = 0;
    *(volatile unsigned int*)0x04000504 = 0;
    *(volatile unsigned int*)0x04000448 = 1;
    *(volatile unsigned int*)0x04000448 = 1;
    *(volatile unsigned int*)0x04000448 = 1;
}
