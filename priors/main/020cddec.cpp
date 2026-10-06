#include <globaldefs.h>

extern "C" unsigned int DisableIRQInterrupts(void);
extern "C" unsigned int SetIRQInterruptState(int mask);

struct AffineFit020cddec {
    short offsetX; // +0x0
    short offsetY; // +0x2
    short scaleX;  // +0x4
    short scaleY;  // +0x6
};

// SCRATCH-USA: func_020cddec
extern "C" ARM int ComputeAffineFit_020cddec(struct AffineFit020cddec* out,
    unsigned short x1, unsigned short y1, unsigned short w1, unsigned short h1,
    unsigned short x2, unsigned short y2, unsigned short w2, unsigned short h2)
{
    int yDiff8;
    if (!(x1 < 0x1000 && y1 < 0x1000 && x2 < 0x1000 && y2 < 0x1000))
        return 1;
    if (!(w1 < 0x100 && w2 < 0x100 && h1 < 0xc0 && h2 < 0xc0))
        return 1;
    if (w1 == w2 && h1 == h2 && x1 == x2 && y1 == y2)
        return 1;

    unsigned int mask = DisableIRQInterrupts();

    *(volatile unsigned short*)0x4000280 = 0;
    *(volatile unsigned int*)0x4000290 = (x1 - x2) << 8;
    *(volatile unsigned int*)0x4000298 = w1 - w2;
    *(volatile unsigned int*)0x400029c = 0;
    int hDiff = h1 - h2;
    yDiff8 = (y1 - y2) << 8;

    while (*(volatile unsigned short*)0x4000280 & 0x8000) {}
    int scaleX = *(volatile int*)0x40002a0;

    *(volatile unsigned short*)0x4000280 = 0;
    *(volatile unsigned int*)0x4000290 = yDiff8;
    *(volatile unsigned int*)0x4000298 = hDiff;
    *(volatile unsigned int*)0x400029c = 0;

    if (scaleX >= 0x8000 || scaleX < -0x8000) {
        SetIRQInterruptState(mask);
        return 1;
    }
    out->scaleX = (short)scaleX;
    int offsetX = (((x1 + x2) << 8) - out->scaleX * (w1 + w2)) << 9 >> 16;
    if (offsetX >= 0x8000 || offsetX < -0x8000) {
        SetIRQInterruptState(mask);
        return 1;
    }
    out->offsetX = (short)offsetX;

    while (*(volatile unsigned short*)0x4000280 & 0x8000) {}
    int scaleY = *(volatile int*)0x40002a0;
    SetIRQInterruptState(mask);

    if (scaleY >= 0x8000 || scaleY < -0x8000)
        return 1;
    out->scaleY = (short)scaleY;
    int offsetY = (((y1 + y2) << 8) - out->scaleY * (h1 + h2)) << 9 >> 16;
    if (offsetY >= 0x8000 || offsetY < -0x8000)
        return 1;
    out->offsetY = (short)offsetY;

    return 0;
}
