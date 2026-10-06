#include <globaldefs.h>

struct DecimalString_02002b90 {
    unsigned char sign;
    unsigned char unused;
    short exp;
    unsigned char length;
    signed char text[1];
};

// USA: func_02002b90
extern "C" ARM void func_02002b90(struct DecimalString_02002b90* dec, int digits) {
    unsigned char len;
    signed char* text;
    signed char* p;
    signed char* q;
    int v;
    int carry;

    if (digits < 0) {
    setToZero:
        dec->exp = 0;
        dec->length = 1;
        dec->text[0] = '0';
        return;
    }
    len = dec->length;
    if (digits >= len) {
        return;
    }
    text = dec->text;
    p = text + digits + 1;
    v = (signed char)(*--p - '0');
    if (v == 5) {
        q = text + len;
        do {
            q--;
        } while (q > p && *q == '0');
        carry = (q == p) ? (p[-1] & 1) : 1;
    } else {
        carry = v > 5;
    }
    while (digits != 0) {
        v = (signed char)(*--p - '0' + carry);
        carry = v > 9;
        if (carry != 0 || v == 0) {
            digits--;
        } else {
            *p = (signed char)(v + '0');
            break;
        }
    }
    if (carry != 0) {
        dec->exp = dec->exp + 1;
        dec->length = 1;
        dec->text[0] = '1';
        return;
    }
    if (digits == 0) {
        goto setToZero;
    }
    dec->length = digits;
}
