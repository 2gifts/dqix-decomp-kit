#include <globaldefs.h>

struct Grid02188e70 {
    char reserved[0xc04];
    int originY;   /* +0xc04 */
    int width;     /* +0xc08 */
    int height;    /* +0xc0c */
    int stepX;     /* +0xc10 */
    int stepY;     /* +0xc14 */
    int columns;   /* +0xc18 */
};

// USA: func_ov009_02188e70
extern "C" ARM void func_ov009_02188e70(void* objRaw, int idx) {
    char* obj = (char*)objRaw;
    struct Grid02188e70* grid = (struct Grid02188e70*)(obj + 0x2c);
    int columns = grid->columns;
    int originY = grid->originY;
    int width = grid->width;
    int height = grid->height;
    int stepY = grid->stepY;

    *(short*)(obj + 0xc4c) = (short)(grid->stepX * (idx % columns) + *(int*)(obj + 0xc2c));
    *(short*)(obj + 0xc4e) = (short)(stepY * (idx / columns) + originY);
    *(short*)(obj + 0xc50) = (short)width;
    *(short*)(obj + 0xc52) = (short)height;
    *(short*)(obj + 0xc54) = *(short*)(obj + 0xc4c);
    *(short*)(obj + 0xc56) = *(short*)(obj + 0xc4e);
}
