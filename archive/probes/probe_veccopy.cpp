#include <globaldefs.h>

extern "C" void *memcpy(void *d, const void *s, unsigned int n);

struct Vec3 { int x, y, z; };

extern "C" void SinkVec(struct Vec3 *p);

extern "C" ARM void ProbeAssign(struct Vec3 *src) {
    struct Vec3 a;
    struct Vec3 b;
    a = *src;
    b = a;
    SinkVec(&b);
}

extern "C" ARM void ProbeCopyInit(struct Vec3 *src) {
    struct Vec3 a = *src;
    struct Vec3 b = a;
    SinkVec(&b);
}

extern "C" ARM void ProbeMemcpy(struct Vec3 *src) {
    struct Vec3 a;
    struct Vec3 b;
    memcpy(&a, src, sizeof a);
    memcpy(&b, &a, sizeof b);
    SinkVec(&b);
}

inline void *operator new(unsigned long, void *p) { return p; }

extern "C" ARM void ProbePlacement(struct Vec3 *src) {
    struct Vec3 a;
    struct Vec3 b;
    new (&a) Vec3(*src);
    new (&b) Vec3(a);
    SinkVec(&b);
}

// Does a copy CHAIN survive if the intermediate is also used? The target emits three ldm/stm pairs
// for diff->half->halfOut plus mid->midOut; ours collapses the chain to one.
extern "C" ARM void ProbeChainBothUsed(struct Vec3 *src) {
    struct Vec3 a = *src;
    struct Vec3 b = a;
    SinkVec(&a);
    SinkVec(&b);
}

extern "C" ARM void ProbeChainVolatile(struct Vec3 *src) {
    volatile struct Vec3 a = *src;
    struct Vec3 b = (struct Vec3 &)a;
    SinkVec(&b);
}
