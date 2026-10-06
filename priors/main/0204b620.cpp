#include <globaldefs.h>

extern "C" void func_0204b620(void*, void*, int, int, int, int, int, int, int, int);

struct Obj0204bab4 {
    char pad0[0x14];
    void* field14;
};

struct Pointish0204bab4 {
    short x;
    short y;
    char pad4[8];
    void* field0xc;
};

// USA: func_0204bab4
extern "C" ARM void func_0204bab4(struct Obj0204bab4* obj, struct Pointish0204bab4* pt, int width, int height,
                        int x0, short y0, int countX, int countY, unsigned short a9) {
    short curY;
    short x, y;

    if (width <= 0 || height <= 0) return;
    if (pt->field0xc == NULL) return;

    x = pt->x;
    y = pt->y;
    curY = y0;
    if (x < width) width = x;
    if (y < height) height = y;

    for (unsigned short j = 0; j < countY; j++) {
        int curX = x0;
        for (unsigned short i = 0; i < countX; i++) {
            if (obj->field14 != NULL) {
                func_0204b620(obj, obj->field14, (int)pt, 0, 0, curX, curY, width, height, a9);
            }
            curX = (short)(curX + width);
        }
        curY = (short)(curY + height);
    }
}
