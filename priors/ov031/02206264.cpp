#include <globaldefs.h>

struct QMeta02206264 {
    char pad[0x48];
    int maxCount;
};

struct QOwner02206264 {
    char pad[0x10c];
    QMeta02206264* meta;
};

struct Obj02206264 {
    char pad0[0x68];
    QOwner02206264* owner;
    char pad1[0x73 - 0x6c];
    signed char mode73;
};

extern "C" ARM int func_ov031_02206368(void* obj, int min, int max, int* out, int flags);
extern "C" ARM int func_ov031_02206410(void* obj, void* buf, int n, int val, int p3, int p4, int flags);

// USA: func_ov031_02206264
extern "C" ARM int func_ov031_02206264(Obj02206264* obj, void* buf, int count, int p3, int p4, int flags) {
    QMeta02206264* meta = obj->owner->meta;
    int total = 0;
    int out14;
    volatile int limit;
    if (obj->mode73 == 1) {
        if (count > meta->maxCount - 0x2a) {
            return total - 0x23;
        }
        limit = count;
    } else {
        limit = meta->maxCount - 0x36;
        if (count <= limit) {
            limit = count;
        }
    }
    if (count <= 0) {
        return total;
    }
    int flagBit = flags & 1;
    do {
        int n = func_ov031_02206368(obj, count, limit, &out14, flags);
        if (n > 0) {
            int r = func_ov031_02206410(obj, buf, n, out14, p3, p4, flags);
            if (r <= 0) {
                return -6;
            }
            buf = (char*)buf + n;
            count -= n;
            total += n;
        }
        if (!flagBit) {
            if (n <= 0) {
                return -6;
            }
            break;
        }
    } while (count > 0);
    return total;
}
