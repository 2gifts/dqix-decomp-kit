#include <globaldefs.h>

struct Mtx44_020c223c { int v[16]; };
struct Quad020c223c { int q[4]; };
struct QuadMtx020c223c { struct Quad020c223c row[4]; };

// USA: func_020c223c
extern "C" ARM void func_020c223c(struct Mtx44_020c223c *p1, struct Mtx44_020c223c *p2, struct Mtx44_020c223c *pDst) {
    struct Mtx44_020c223c tmpMtx;
    struct Mtx44_020c223c *p = (pDst == p2) ? &tmpMtx : pDst;

    int b0z = p2->v[2], b1z = p2->v[6], b2z = p2->v[10], b3z = p2->v[14];
    p->v[0] = (int)(((long long)p1->v[0] * p2->v[0] + (long long)p1->v[1] * p2->v[4] + (long long)p1->v[2] * p2->v[8] + (long long)p1->v[3] * p2->v[12]) >> 12);
    p->v[1] = (int)(((long long)p1->v[0] * p2->v[1] + (long long)p1->v[1] * p2->v[5] + (long long)p1->v[2] * p2->v[9] + (long long)p1->v[3] * p2->v[13]) >> 12);
    int b0w = p2->v[3], b1w = p2->v[7], b2w = p2->v[11], b3w = p2->v[15];
    p->v[3] = (int)(((long long)p1->v[0] * b0w + (long long)p1->v[1] * b1w + (long long)p1->v[2] * b2w + (long long)p1->v[3] * b3w) >> 12);
    p->v[2] = (int)(((long long)p1->v[0] * b0z + (long long)p1->v[1] * b1z + (long long)p1->v[2] * b2z + (long long)p1->v[3] * b3z) >> 12);

    p->v[6] = (int)(((long long)p1->v[4] * b0z + (long long)p1->v[5] * b1z + (long long)p1->v[6] * b2z + (long long)p1->v[7] * b3z) >> 12);
    int b0y = p2->v[1], b1y = p2->v[5], b2y = p2->v[9], b3y = p2->v[13];
    p->v[5] = (int)(((long long)p1->v[4] * b0y + (long long)p1->v[5] * b1y + (long long)p1->v[6] * b2y + (long long)p1->v[7] * b3y) >> 12);
    p->v[7] = (int)(((long long)p1->v[4] * b0w + (long long)p1->v[5] * b1w + (long long)p1->v[6] * b2w + (long long)p1->v[7] * b3w) >> 12);
    int b0x = p2->v[0], b1x = p2->v[4], b2x = p2->v[8], b3x = p2->v[12];
    p->v[4] = (int)(((long long)p1->v[4] * b0x + (long long)p1->v[5] * b1x + (long long)p1->v[6] * b2x + (long long)p1->v[7] * b3x) >> 12);

    p->v[8] = (int)(((long long)p1->v[8] * b0x + (long long)p1->v[9] * b1x + (long long)p1->v[10] * b2x + (long long)p1->v[11] * b3x) >> 12);
    p->v[9] = (int)(((long long)p1->v[8] * b0y + (long long)p1->v[9] * b1y + (long long)p1->v[10] * b2y + (long long)p1->v[11] * b3y) >> 12);
    p->v[11] = (int)(((long long)p1->v[8] * p2->v[3] + (long long)p1->v[9] * p2->v[7] + (long long)p1->v[10] * p2->v[11] + (long long)p1->v[11] * p2->v[15]) >> 12);
    int b0z2 = p2->v[2], b1z2 = p2->v[6], b2z2 = p2->v[10], b3z2 = p2->v[14];
    p->v[10] = (int)(((long long)p1->v[8] * b0z2 + (long long)p1->v[9] * b1z2 + (long long)p1->v[10] * b2z2 + (long long)p1->v[11] * b3z2) >> 12);

    p->v[14] = (int)(((long long)p1->v[12] * b0z2 + (long long)p1->v[13] * b1z2 + (long long)p1->v[14] * b2z2 + (long long)p1->v[15] * b3z2) >> 12);
    p->v[13] = (int)(((long long)p1->v[12] * p2->v[1] + (long long)p1->v[13] * p2->v[5] + (long long)p1->v[14] * p2->v[9] + (long long)p1->v[15] * p2->v[13]) >> 12);
    p->v[12] = (int)(((long long)p1->v[12] * p2->v[0] + (long long)p1->v[13] * p2->v[4] + (long long)p1->v[14] * p2->v[8] + (long long)p1->v[15] * p2->v[12]) >> 12);
    p->v[15] = (int)(((long long)p1->v[12] * p2->v[3] + (long long)p1->v[13] * p2->v[7] + (long long)p1->v[14] * p2->v[11] + (long long)p1->v[15] * p2->v[15]) >> 12);

    if (p == &tmpMtx) {
        struct QuadMtx020c223c *qd = (struct QuadMtx020c223c *)pDst;
        struct QuadMtx020c223c *qs = (struct QuadMtx020c223c *)&tmpMtx;
        qd->row[0] = qs->row[0];
        qd->row[1] = qs->row[1];
        qd->row[2] = qs->row[2];
        qd->row[3] = qs->row[3];
    }
}
