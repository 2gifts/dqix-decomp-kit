#include <globaldefs.h>

struct Template02002cb4 {
    unsigned char pad0[2];
    short maxCount;
};

struct DecimalDigits02002cb4 {
    signed char sign;
    unsigned char unused1;
    short exp;
    unsigned char length;
    signed char text[32];
};

extern "C" ARM void func_0200a300(Template02002cb4* tmpl, double value, DecimalDigits02002cb4* o);
extern "C" ARM void func_02002b90(DecimalDigits02002cb4* dec, int digits);

extern short data_020e69ac;
extern unsigned char data_020eeef8;
extern unsigned char data_020eef00;
extern unsigned char data_020eef08;
extern unsigned char data_020eef0c;
extern unsigned char data_020eef10;
extern unsigned char data_020eef18;
extern unsigned char data_020eef20;
extern unsigned char data_020eef24;

struct FormatFlags02002cb4 {
    unsigned char pad0;
    signed char fmtChar;
    unsigned char pad2;
    unsigned char alt;
    unsigned char pad4;
    signed char plus;
    unsigned char pad6;
    unsigned char pad7;
};

// USA: func_02002cb4
extern "C" ARM char* func_02002cb4(double value, char* buf, FormatFlags02002cb4 flagsStruct, int reserved, int precision) {
    signed char plus = flagsStruct.plus;
    unsigned char alt = flagsStruct.alt;
    signed char fmtChar = flagsStruct.fmtChar;
    char* cursor;
    (void)reserved;

    if (precision > 509) {
        return 0;
    }

    Template02002cb4 tmpl;
    DecimalDigits02002cb4 dec;
    tmpl.pad0[0] = 0;
    tmpl.maxCount = 0x20;
    func_0200a300(&tmpl, value, &dec);

    signed char* p = dec.text + dec.length;
    if (dec.length > 1) {
        while (dec.length > 1 && *--p == '0') {
            dec.length--;
            dec.exp++;
        }
    }

    if (dec.text[0] == '0') {
        dec.exp = 0;
        goto numeric;
    }
    if (dec.text[0] == 'I') {
        int upper = 0;
        if (fmtChar >= 0 && fmtChar < 0x80) {
            upper = ((unsigned short*)&data_020e69ac)[(unsigned char)fmtChar] & 0x200;
        }
        if (value >= 0.0) {
            char* w = buf - 4;
            if (upper != 0) {
                char* s = (char*)&data_020eef08;
                w[0] = s[0]; w[1] = s[1]; w[2] = s[2]; w[3] = s[3];
            } else {
                char* s = (char*)&data_020eef0c;
                w[0] = s[0]; w[1] = s[1]; w[2] = s[2]; w[3] = s[3];
            }
            return w;
        } else {
            char* w = buf - 5;
            if (upper != 0) {
                char* s = (char*)&data_020eeef8;
                w[0] = s[0]; w[1] = s[1]; w[2] = s[2]; w[3] = s[3]; w[4] = s[4];
            } else {
                char* s = (char*)&data_020eef00;
                w[0] = s[0]; w[1] = s[1]; w[2] = s[2]; w[3] = s[3]; w[4] = s[4];
            }
            return w;
        }
    }
    if (dec.text[0] == 'N') {
        int upper = 0;
        if (fmtChar >= 0 && fmtChar < 0x80) {
            upper = ((unsigned short*)&data_020e69ac)[(unsigned char)fmtChar] & 0x200;
        }
        if (dec.sign != 0) {
            char* w = buf - 5;
            if (upper != 0) {
                char* s = (char*)&data_020eef10;
                w[0] = s[0]; w[1] = s[1]; w[2] = s[2]; w[3] = s[3]; w[4] = s[4];
            } else {
                char* s = (char*)&data_020eef18;
                w[0] = s[0]; w[1] = s[1]; w[2] = s[2]; w[3] = s[3]; w[4] = s[4];
            }
            return w;
        } else {
            char* w = buf - 4;
            if (upper != 0) {
                char* s = (char*)&data_020eef20;
                w[0] = s[0]; w[1] = s[1]; w[2] = s[2]; w[3] = s[3];
            } else {
                char* s = (char*)&data_020eef24;
                w[0] = s[0]; w[1] = s[1]; w[2] = s[2]; w[3] = s[3];
            }
            return w;
        }
    }

numeric:
    dec.exp = dec.exp + (dec.length - 1);
    cursor = buf - 1;
    *cursor = 0;

    if (fmtChar == 'g' || fmtChar == 'G') {
        if (dec.length > precision) {
            func_02002b90(&dec, precision);
        }
        if (dec.exp < -4 || dec.exp >= precision) {
            if (alt == 0) {
                precision = dec.length - 1;
            } else {
                precision = precision - 1;
            }
            fmtChar = (fmtChar == 'g') ? 'e' : 'E';
            goto sci;
        } else {
            if (alt != 0) {
                precision = precision - (dec.exp + 1);
            } else {
                precision = dec.length - (dec.exp + 1);
                if (precision < 0) precision = 0;
            }
            goto fixed;
        }
    } else if (fmtChar == 'e' || fmtChar == 'E') {
        goto sci;
    } else if (fmtChar == 'f' || fmtChar == 'F') {
        goto fixed;
    } else {
        goto end;
    }

sci:
    {
        if (dec.length > precision + 1) {
            func_02002b90(&dec, precision + 1);
        }
        int e = dec.exp;
        char expSign = '+';
        int digitCount = 0;
        if (e < 0) {
            e = -e;
            expSign = '-';
        }
        do {
            int q = e / 10;
            int rem = e - q * 10;
            *--cursor = (char)(rem + '0');
            e = q;
            digitCount++;
        } while (e != 0 || digitCount < 2);
        cursor[-1] = expSign;
        cursor -= 2;
        *cursor = fmtChar;

        if (precision + (buf - cursor) > 509) {
            return 0;
        }

        if (dec.length < precision + 1) {
            int pad = (precision + 2) - dec.length - 1;
            while (pad != 0) {
                *--cursor = '0';
                pad--;
            }
        }

        {
            signed char* q = dec.text + dec.length;
            int n = dec.length - 1;
            while (n != 0) {
                *--cursor = *--q;
                n--;
            }
        }

        if (precision != 0 || alt != 0) {
            *--cursor = '.';
        }
        *--cursor = dec.text[0];

        if (dec.sign != 0) {
            *--cursor = '-';
            goto end;
        }
        if (plus == 1) {
            *--cursor = '+';
            goto end;
        }
        if (plus == 2) {
            *--cursor = ' ';
        }
        goto end;
    }

fixed:
    {
        int fracDigits = dec.length - dec.exp - 1;
        if (fracDigits < 0) fracDigits = 0;
        if (fracDigits > precision) {
            func_02002b90(&dec, dec.length - (fracDigits - precision));
        }
        int intDigits = dec.exp + 1;
        fracDigits = dec.length - dec.exp - 1;
        if (fracDigits < 0) fracDigits = 0;
        if (intDigits < 0) intDigits = 0;
        int totalDigits = intDigits + fracDigits;
        if (totalDigits > 509) {
            return 0;
        }

        signed char* srcEnd = dec.text + dec.length;
        int zeroPad = precision - fracDigits;
        int i;
        if (zeroPad > 0) {
            for (i = 0; i < zeroPad; ) {
                i++;
                *--cursor = '0';
            }
        }
        i = 0;
        while (i < fracDigits && i < dec.length) {
            *--cursor = *--srcEnd;
            i++;
        }
        while (i < fracDigits) {
            *--cursor = '0';
            i++;
        }

        if (precision != 0 || alt != 0) {
            *--cursor = '.';
        }

        if (intDigits != 0) {
            int k = 0;
            int j = intDigits - dec.length;
            if (j > 0) {
                do {
                    *--cursor = '0';
                    k++;
                    j = intDigits - dec.length;
                } while (k < j);
            }
            while (k < intDigits) {
                *--cursor = *--srcEnd;
                k++;
            }
        } else {
            *--cursor = '0';
        }

        if (dec.sign != 0) {
            *--cursor = '-';
            goto end;
        }
        if (plus == 1) {
            *--cursor = '+';
            goto end;
        }
        if (plus == 2) {
            *--cursor = ' ';
        }
        goto end;
    }

end:
    return cursor;
}
