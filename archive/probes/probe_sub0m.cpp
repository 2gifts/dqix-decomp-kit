#include <globaldefs.h>

struct Obj { int a; int b; int value; };

static inline int Zero() { return 0; }
static inline int BaseOf(Obj *) { return 0; }
template <class T> struct Base { static inline int of() { return 0; } };

#define BODY(EXPR)                          \
    int i = idx - (EXPR);                   \
    int flag = 1;                           \
    if (i != -1) {                          \
        if (obj->value != 0) flag = 0;      \
    }                                       \
    if (flag == 0) return obj->value + i;   \
    return dflt;

extern "C" ARM int za(Obj *obj, int idx, int dflt) { BODY(Zero()) }
extern "C" ARM int zb(Obj *obj, int idx, int dflt) { BODY(BaseOf(obj)) }
extern "C" ARM int zc(Obj *obj, int idx, int dflt) { BODY(Base<Obj>::of()) }
extern "C" ARM int zd(Obj *obj, int idx, int dflt) { BODY((char)0) }
extern "C" ARM int ze(Obj *obj, int idx, int dflt) { BODY((short)0) }
extern "C" ARM int zf(Obj *obj, int idx, int dflt) { BODY((bool)false) }
extern "C" ARM int zg(Obj *obj, int idx, int dflt) { BODY((long)0) }
extern "C" ARM int zh(Obj *obj, int idx, int dflt) { BODY((unsigned char)0) }
extern "C" ARM int zi(Obj *obj, int idx, int dflt) { BODY(!!0) }
extern "C" ARM int zj(Obj *obj, int idx, int dflt) { BODY((int)(float)0.0f) }
