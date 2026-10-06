#include <globaldefs.h>

struct ScreenRect_020cdfd8 {
    short x;
    short y;
    short width;
    short height;
};

struct SourceRect_020cdfd8 {
    unsigned short x;
    unsigned short y;
    unsigned short width;
    unsigned short height;
};

struct AxisScale_020cdfd8 {
    int origin;
    int span;
    int scale;
};

extern int data_021117b0;
extern AxisScale_020cdfd8 data_021117cc[2];

// USA: func_020cdfd8
extern "C" ARM void func_020cdfd8(ScreenRect_020cdfd8* dst, const SourceRect_020cdfd8* src) {
    if (*(unsigned short*)((char*)&data_021117b0 + 0x34) == 0) {
        short x = src->x;
        short y = src->y;
        dst->x = x;
        dst->y = y;
        short w = src->width;
        short h = src->height;
        dst->width = w;
        dst->height = h;
        return;
    }
    AxisScale_020cdfd8* cal = data_021117cc;
    dst->width = src->width;
    dst->height = src->height;
    switch (src->width) {
    case 0:
        dst->x = 0;
        dst->y = 0;
        return;
    }
    dst->x = (short)(((long long)(src->x * 4) - cal[0].origin) * cal[0].scale >> 22);
    if (dst->x < 0) {
        dst->x = 0;
    } else if (dst->x > 0xff) {
        dst->x = 0xff;
    }
    dst->y = (short)(((long long)(src->y * 4) - cal[1].origin) * cal[1].scale >> 22);
    if (dst->y < 0) {
        dst->y = 0;
    } else if (dst->y > 0xbf) {
        dst->y = 0xbf;
    }
}
