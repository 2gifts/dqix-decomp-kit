#include <globaldefs.h>

struct Md5FullCtx020c04e8 {
    unsigned int a, b, c, d;
    unsigned int countLow;
    unsigned int countHigh;
    unsigned char buffer[64];
};

extern unsigned int data_020f2050[64];
extern unsigned int data_020f1f90[48];

#define MD5_F(x, y, z) ((~(x) & (z)) | ((x) & (y)))
#define MD5_G(x, y, z) (((y) & ~(z)) | ((x) & (z)))
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z) ((y) ^ ((x) | ~(z)))
#define MD5_ROTL(v, s) (((v) << (s)) | ((v) >> (32 - (s))))

// SCRATCH-USA: func_020c04e8  (semantic: TransformMd5Block)
extern "C" ARM void func_020c04e8(void* obj) {
    Md5FullCtx020c04e8* ctx = (Md5FullCtx020c04e8*)obj;
    unsigned int a = ctx->a;
    unsigned int b = ctx->b;
    unsigned int c = ctx->c;
    unsigned int d = ctx->d;
    unsigned int* x = (unsigned int*)ctx->buffer;
    unsigned int* k = data_020f2050;
    int i;

    unsigned int* p = x;
    i = 0;
    do {
        a = b + MD5_ROTL(a + MD5_F(b, c, d) + p[0] + k[0], 7);
        d = a + MD5_ROTL(d + MD5_F(a, b, c) + p[1] + k[1], 12);
        c = d + MD5_ROTL(c + MD5_F(d, a, b) + p[2] + k[2], 17);
        b = c + MD5_ROTL(b + MD5_F(c, d, a) + k[3] + p[3], 22);
        p += 4;
        k += 4;
        i++;
    } while (i < 4);

    p = data_020f1f90;
    i = 0;
    do {
        a = b + MD5_ROTL(a + MD5_G(b, c, d) + x[p[0]] + k[0], 5);
        d = a + MD5_ROTL(d + MD5_G(a, b, c) + x[p[1]] + k[1], 9);
        c = d + MD5_ROTL(c + MD5_G(d, a, b) + x[p[2]] + k[2], 14);
        b = c + MD5_ROTL(b + MD5_G(c, d, a) + k[3] + x[p[3]], 20);
        p += 4;
        k += 4;
        i++;
    } while (i < 4);

    i = 0;
    do {
        a = b + MD5_ROTL(a + MD5_H(b, c, d) + x[p[0]] + k[0], 4);
        d = a + MD5_ROTL(d + MD5_H(a, b, c) + x[p[1]] + k[1], 11);
        c = d + MD5_ROTL(c + MD5_H(d, a, b) + x[p[2]] + k[2], 16);
        b = c + MD5_ROTL(b + MD5_H(c, d, a) + x[p[3]] + k[3], 23);
        p += 4;
        k += 4;
        i++;
    } while (i < 4);

    i = 0;
    do {
        a = b + MD5_ROTL(a + MD5_I(b, c, d) + x[p[0]] + k[0], 6);
        d = a + MD5_ROTL(d + MD5_I(a, b, c) + x[p[1]] + k[1], 10);
        c = d + MD5_ROTL(c + MD5_I(d, a, b) + x[p[2]] + k[2], 15);
        b = c + MD5_ROTL(b + MD5_I(c, d, a) + x[p[3]] + k[3], 21);
        p += 4;
        k += 4;
        i++;
    } while (i < 4);

    ctx->a += a;
    ctx->b += b;
    ctx->c += c;
    ctx->d += d;
}
