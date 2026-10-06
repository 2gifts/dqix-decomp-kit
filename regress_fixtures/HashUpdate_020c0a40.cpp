#include <globaldefs.h>

struct HashCtx020c08ec {
    unsigned int unk0[5];
    unsigned int totalLo;
    unsigned int totalHi;
    unsigned int index;
    unsigned char buffer[0x40];
};

typedef void (*HashTransformFn020c08ec)(HashCtx020c08ec*, const void*, unsigned int);
extern HashTransformFn020c08ec data_020f2154;

extern "C" void func_020ca3ec(int value, void* dst, int size);

// USA: func_020c0a40
extern "C" ARM void func_020c0a40(HashCtx020c08ec* ctx, unsigned char* out) {
    unsigned int* words = (unsigned int*)ctx->buffer;
    int wordIdx = ctx->index >> 2;
    int index = ctx->index;
    if ((index & 3) == 0) {
        words[wordIdx] = 0;
    }

    unsigned char* bytes = ctx->buffer;
    bytes[index] = 0x80;
    index += 1;
    if (index & 3) {
        do {
            bytes[index] = 0;
            index += 1;
        } while (index & 3);
    }
    wordIdx += 1;

    if (ctx->index >= 0x38) {
        if (wordIdx < 0x10) {
            do {
                words[wordIdx] = 0;
                wordIdx += 1;
            } while (wordIdx < 0x10);
        }
        data_020f2154(ctx, words, 0x40);
        wordIdx = 0;
    }
    if (wordIdx < 0xe) {
        do {
            words[wordIdx] = 0;
            wordIdx += 1;
        } while (wordIdx < 0xe);
    }

    unsigned int lo = ctx->totalLo;
    bytes[0x3f] = (unsigned char)lo;
    bytes[0x3e] = (unsigned char)(lo >> 8);
    bytes[0x3d] = (unsigned char)(lo >> 16);
    bytes[0x3c] = (unsigned char)(lo >> 24);

    unsigned int hi = ctx->totalHi;
    bytes[0x3b] = (unsigned char)hi;
    bytes[0x3a] = (unsigned char)(hi >> 8);
    bytes[0x39] = (unsigned char)(hi >> 16);
    bytes[0x38] = (unsigned char)(hi >> 24);

    data_020f2154(ctx, words, 0x40);

    unsigned int h;
    h = ctx->unk0[0];
    out[0] = (unsigned char)(h >> 24);
    out[1] = (unsigned char)(h >> 16);
    out[2] = (unsigned char)(h >> 8);
    out[3] = (unsigned char)h;

    h = ctx->unk0[1];
    out[4] = (unsigned char)(h >> 24);
    out[5] = (unsigned char)(h >> 16);
    out[6] = (unsigned char)(h >> 8);
    out[7] = (unsigned char)h;

    h = ctx->unk0[2];
    out[8] = (unsigned char)(h >> 24);
    out[9] = (unsigned char)(h >> 16);
    out[10] = (unsigned char)(h >> 8);
    out[11] = (unsigned char)h;

    h = ctx->unk0[3];
    out[12] = (unsigned char)(h >> 24);
    out[13] = (unsigned char)(h >> 16);
    out[14] = (unsigned char)(h >> 8);
    out[15] = (unsigned char)h;

    h = ctx->unk0[4];
    out[16] = (unsigned char)(h >> 24);
    out[17] = (unsigned char)(h >> 16);
    out[18] = (unsigned char)(h >> 8);
    out[19] = (unsigned char)h;

    ctx->index = 0;
    func_020ca3ec(0, &ctx, sizeof(ctx));
}
