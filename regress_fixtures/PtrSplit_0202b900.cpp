#include <globaldefs.h>
#include "Combat/Main/BattleList.h"
struct BattleStruct;
extern "C" struct BattleStruct* _ZN9GameState11GetInstanceEv();

extern char data_020feffc[];

int GetGlobalField0x10(void);
int AdvanceStateIfState2(void);
void SleepCurrentContext(unsigned int);
extern "C" int _Z12Init0202dc40iPv(int mode, void* src);
unsigned long long GetCurrentTimestamp(void);
extern "C" int func_02001aec(void* a, void* b, int n);

// USA: func_0202b900
extern "C" ARM int func_0202b900(int* obj, unsigned char* key) {
    char* entries;
    int i;
    int result;
    int count;
    int j;
    int stride;
    struct BattleStruct* battleStruct;
    unsigned long long ts;

    i = 0;
    while (GetGlobalField0x10() == 2) {
        AdvanceStateIfState2();
        i++;
        if (i > 0x1388) {
            return -1;
        }
        SleepCurrentContext(1);
    }

    i = 0;
    while (GetGlobalField0x10() != 1) {
        i++;
        if (i > 0x1388) {
            return -1;
        }
        SleepCurrentContext(1);
    }

    count = obj[3];
    result = -1;
    j = 0;
    stride = 0xc0;
    entries = (char*)obj + 0x10;

    for (; j < count; j++) {
        if (func_02001aec(key, entries + j * stride + 4, 6) == 0) {
            _Z12Init0202dc40iPv(5, data_020feffc + j * stride);
            *obj = 6;
            battleStruct = _ZN9GameState11GetInstanceEv();
            ts = GetCurrentTimestamp();
            *(unsigned int*)((char*)battleStruct + 0x3f0) = (unsigned int)ts;
            result = j;
            *(unsigned int*)((char*)battleStruct + 0x3f4) = (unsigned int)(ts >> 32);
            break;
        }
    }
    return result;
}
