#include <globaldefs.h>

extern "C" int func_020017a4(int x);
extern "C" int _Z22GetSignedField020c39c8PVt(volatile unsigned short* p);
extern "C" void _Z25ResetBattlerState0201289cPh(unsigned char* obj);
struct Obj020128d8;
extern "C" void _Z34SaveAndDisableBattlerState020128d8P11Obj020128d8(struct Obj020128d8* obj);
extern "C" void func_02012938(unsigned char* obj);
struct StateA020129b4;
int UpdateTargetTrackingState(struct StateA020129b4* obj);
void SetCompareFlags0x50To0x53(unsigned char* obj);
extern "C" int _Z27TickByteFlagCounter02012ad0Ph(unsigned char* obj);
void SelectCoordsByFlag0x24(unsigned char* obj, int* out1, int* out2);

// USA: func_02012538
extern "C" ARM int func_02012538(unsigned char* obj) {
    if (obj[0x61] == 0) {
        _Z25ResetBattlerState0201289cPh(obj);
        return 0;
    }

    int dFar = _Z22GetSignedField020c39c8PVt((volatile unsigned short*)0x0400106c);
    int sum = func_020017a4(_Z22GetSignedField020c39c8PVt((volatile unsigned short*)0x0400006c)) + func_020017a4(dFar);

    if (sum >= 0x20) {
        _Z34SaveAndDisableBattlerState020128d8P11Obj020128d8((struct Obj020128d8*)obj);
    } else if (obj[0x60] != 0) {
        func_02012938(obj);
    }

    if ((*(unsigned short*)0x027fffa8 & 0x8000) >> 15) {
        return 0;
    }

    if (*(unsigned short*)(obj + 0x24) != 0) {
        obj[0x55] = 0;
        *(int*)(obj + 0x38) = *(unsigned short*)(obj + 0x20);
        *(int*)(obj + 0x3c) = *(unsigned short*)(obj + 0x22);
        if (!UpdateTargetTrackingState((struct StateA020129b4*)obj)) {
            return 0;
        }
        obj[0x5e] = obj[0x5d];
        int v;
        if (*(unsigned short*)(obj + 0x24) == 0) {
            obj[0x54] = 1;
            v = 0;
        } else {
            v = 1;
        }
        obj[0x5d] = v;
        SetCompareFlags0x50To0x53(obj);
        obj[0x5c] = _Z27TickByteFlagCounter02012ad0Ph(obj);
        return *(unsigned short*)(obj + 0x24) != 0;
    }

    *(int*)(obj + 0x38) = *(unsigned short*)(obj + 0x20);
    *(int*)(obj + 0x3c) = *(unsigned short*)(obj + 0x22);
    obj[0x5e] = obj[0x5d];
    obj[0x5d] = 1;
    obj[0x54] = 0;
    if (!UpdateTargetTrackingState((struct StateA020129b4*)obj)) {
        return 0;
    }
    SetCompareFlags0x50To0x53(obj);

    if (*(unsigned short*)(obj + 0x24) != 0) {
        obj[0x55] = 1;
        int f68 = *(int*)(obj + 0x68);
        if (f68 >= 0 || *(int*)(obj + 0x6c) >= 0) {
            *(int*)(obj + 0x30) = f68;
            *(int*)(obj + 0x34) = *(int*)(obj + 0x6c);
        } else {
            SelectCoordsByFlag0x24(obj, (int*)(obj + 0x30), (int*)(obj + 0x34));
        }
        obj[0x5d] = 1;
    } else {
        *(int*)(obj + 0x38) = *(unsigned short*)(obj + 0x20);
        *(int*)(obj + 0x3c) = *(unsigned short*)(obj + 0x22);
        obj[0x5d] = 0;
    }

    if (obj[0x62] == 0) {
        *(int*)(obj + 0x6c) = -1;
        *(int*)(obj + 0x68) = -1;
    }
    obj[0x63] = 0;
    obj[0x5c] = _Z27TickByteFlagCounter02012ad0Ph(obj);
    return *(unsigned short*)(obj + 0x24) != 0;
}
