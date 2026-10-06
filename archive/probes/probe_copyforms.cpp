// Copy 12 bytes into an EXISTING object and emit `ldm`/`stm` with no extra instruction.
// copy-init does it but only for a fresh declaration; assignment emits an out-of-line helper;
// placement new does it but guards the pointer with `cmp rX,#0`. These are the remaining forms.
#include <globaldefs.h>

struct Vec3 { int x, y, z; };
struct Wrap { int a[3]; };
union UVec { struct Vec3 v; int a[3]; };

extern "C" void SinkVec(struct Vec3 *p);

static struct Vec3 gDst;

extern "C" ARM void FormUnion(struct Vec3 *src) {
    union UVec *d = (union UVec *)&gDst;
    union UVec *s = (union UVec *)src;
    *d = *s;
    SinkVec(&gDst);
}

extern "C" ARM void FormWrap(struct Vec3 *src) {
    *(struct Wrap *)&gDst = *(struct Wrap *)src;
    SinkVec(&gDst);
}

extern "C" ARM void FormLongLong(struct Vec3 *src) {
    struct Pair { long long a; int b; };
    *(struct Pair *)&gDst = *(struct Pair *)src;
    SinkVec(&gDst);
}
