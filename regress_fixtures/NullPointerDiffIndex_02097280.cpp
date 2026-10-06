#include <globaldefs.h>

struct Struct02097280 {
    char pad[8];
    int value;
};

// USA: func_02097280
extern "C" ARM int func_02097280(Struct02097280 *obj, int idx, int dflt) {
    int flag = 1;
    if (idx != -1) {
        if (obj->value != 0) {
            flag = 0;
        }
    }
    if (flag == 0) {
        return obj->value + idx;
    }
    return dflt;
}
