#include <globaldefs.h>

// MSL_C __strtold: decimal, hex-float ("0x1.8p3"), INFINITY and NAN(...) parsing.
// The library was built for speed, so the aggregate initialisers, the literal
// copies and the short loops appear here in the unrolled form the ROM carries.

extern "C" float _fsub(float a, float b);
extern "C" double func_0200c578(float x);            // _f2d
extern "C" double func_02001710(const char* tag);    // nan
extern "C" double func_0200b608(double a, double b); // _dsub
extern "C" double func_0200a3a8(void* d);            // __dec2num

extern const unsigned short data_020e69ac[]; // __ctype_map
extern const unsigned char data_020e692c[];  // __upper_map
extern const char data_020e6aac[];           // "NAN("
extern const char data_020e6ab1[];           // "INFINITY"
extern float data_020eecc4;                  // __float_huge
extern double data_020eeccc;                 // __double_huge

#define CTYPE(map, c, m)  (((c) < 0 || (c) >= 0x80) ? 0 : (int)((map)[c] & (m)))
#define UPPER(map, c)     (((c) < 0 || (c) >= 0x80) ? (c) : (int)(map)[c])
#define isspace(c)  CTYPE(data_020e69ac, c, 0x100)
#define isdigit(c)  CTYPE(data_020e69ac, c, 0x8)
#define isxdigit(c) CTYPE(data_020e69ac, c, 0x400)
#define toupper(c)  UPPER(data_020e692c, c)

#define INFINITY (data_020eecc4)
#define HUGE_VAL (data_020eeccc)
#define EOF (-1)
#define SHRT_MIN (-0x8000)
#define SHRT_MAX 0x7fff
#define LDBL_MIN 2.2250738585072014e-308
#define LDBL_MAX 1.7976931348623157e+308
#define LDBL_MANT_DIG 53

struct decimal {
    char sign;
    char unused;
    short exp;
    struct {
        unsigned char length;
        unsigned char text[32];
        unsigned char unused;
    } sig;
};

enum scan_states {
    start              = 0x0001,
    sig_start          = 0x0002,
    leading_sig_zeroes = 0x0004,
    int_digit_loop     = 0x0008,
    frac_start         = 0x0010,
    frac_digit_loop    = 0x0020,
    sig_end            = 0x0040,
    exp_start          = 0x0080,
    leading_exp_digit  = 0x0100,
    leading_exp_zeroes = 0x0200,
    exp_digit_loop     = 0x0400,
    finished           = 0x0800,
    failure            = 0x1000,
    nan_state          = 0x2000,
    infin_state        = 0x4000,
    hex_state          = 0x8000
};

enum hex_scan_states {
    not_hex                = 0x0000,
    hex_start              = 0x0001,
    hex_leading_sig_zeroes = 0x0002,
    hex_int_digit_loop     = 0x0004,
    hex_frac_digit_loop    = 0x0008,
    hex_sig_end            = 0x0010,
    hex_exp_start          = 0x0020,
    hex_leading_exp_digit  = 0x0040,
    hex_leading_exp_zeroes = 0x0080,
    hex_exp_digit_loop     = 0x0100
};

#define final_state(scan_state) ((scan_state) & (finished | failure))
#define success(scan_state) ((scan_state) & (leading_sig_zeroes | int_digit_loop | frac_digit_loop | \
                                             leading_exp_zeroes | exp_digit_loop | finished))
#define hex_success(count, scan_state) ((count) - 1 > 2 && ((scan_state) & (hex_leading_sig_zeroes | \
                                        hex_int_digit_loop | hex_frac_digit_loop | \
                                        hex_leading_exp_zeroes | hex_exp_digit_loop)))

#define fetch()    (count++, (*ReadProc)(ReadProcArg, 0, 0))
#define unfetch(c) (*ReadProc)(ReadProcArg, c, 1)

// SCRATCH-USA: func_020042a8
extern "C" ARM long double func_020042a8(int max_width, int (*ReadProc)(void*, int, int), void* ReadProcArg,
                                         int* chars_scanned, int* overflow) {
    int scan_state = start;
    int hex_scan_state = not_hex;
    int count = 0;
    int spaces = 0;
    int c;
    decimal d;
    {
        short* p = (short*)&d;
        int n = 4;
        do {
            p[0] = 0;
            p[1] = 0;
            p[2] = 0;
            p[3] = 0;
            p += 4;
        } while (--n != 0);
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
    }
    int sig_negative = 0;
    int exp_negative = 0;
    long exp_value = 0;
    int exp_adjust = 0;
    long double result;
    int sign_detected = 0;
    unsigned char* chptr;
    long hex_exp = 0;
    unsigned char uch, uch1;
    int ui;
    int NibbleIndex;
    int mantissa_digits;
    int expsign = 0;
    unsigned intdigits = 0;
    unsigned char mantissa[8];
    unsigned char infinity_str[9];
    unsigned char nan_str[5];

    *overflow = 0;
    c = 0;
    c = c + fetch();

    {
        const unsigned char* s = (const unsigned char*)data_020e6ab1;
        unsigned char* p = infinity_str;
        int n = 4;
        do {
            p[0] = s[0];
            p[1] = s[1];
            s += 2;
            p += 2;
        } while (--n != 0);
        p[0] = s[0];
    }
    {
        const unsigned char* s = (const unsigned char*)data_020e6aac;
        nan_str[0] = s[0];
        nan_str[1] = s[1];
        nan_str[2] = s[2];
        nan_str[3] = s[3];
        nan_str[4] = s[4];
    }

    while (count <= max_width && c != EOF && !final_state(scan_state)) {
        switch (scan_state) {
        case start:
            if (isspace(c)) {
                c = fetch();
                count--;
                spaces++;
                break;
            }

            switch (toupper(c)) {
            case '-':
                sig_negative = 1;
            case '+':
                c = fetch();
                sign_detected = 1;
                break;
            case 'I':
                c = fetch();
                scan_state = infin_state;
                break;
            case 'N':
                c = fetch();
                scan_state = nan_state;
                break;
            default:
                scan_state = sig_start;
                break;
            }
            break;

        case infin_state: {
            int i = 1;
            char model[9];
            char* m;
            const unsigned char* upper;

            {
                const unsigned char* s = infinity_str;
                unsigned char* p = (unsigned char*)model;
                int n = 4;
                do {
                    p[0] = s[0];
                    p[1] = s[1];
                    s += 2;
                    p += 2;
                } while (--n != 0);
                p[0] = s[0];
            }

            m = model + 1;
            upper = data_020e692c;
            while (i < 8 && UPPER(upper, c) == *m) {
                m++;
                i++;
                c = fetch();
            }

            if (i == 3 || i == 8) {
                if (sig_negative)
                    result = func_0200c578(_fsub(0.0f, INFINITY));
                else
                    result = func_0200c578(INFINITY);

                *chars_scanned = spaces + i + sign_detected;
                return result;
            } else
                scan_state = failure;
            break;
        }

        case nan_state: {
            int i = 1, j = 0;
            char model[5];
            char nan_arg[32];
            char* m;

            {
                const unsigned char* s = nan_str;
                unsigned char* p = (unsigned char*)model;
                p[0] = s[0];
                p[1] = s[1];
                p[2] = s[2];
                p[3] = s[3];
                p[4] = s[4];
            }
            {
                char* p = nan_arg;
                int n = 8;
                do {
                    p[0] = 0;
                    p[1] = 0;
                    p[2] = 0;
                    p[3] = 0;
                    p += 4;
                } while (--n != 0);
            }

            m = model + 1;
            while (i < 4 && toupper(c) == *m) {
                m++;
                i++;
                c = fetch();
            }

            if (i == 3 || i == 4) {
                if (i == 4) {
                    const unsigned short* ctype = data_020e69ac;

                    while (j < 32 && (CTYPE(ctype, c, 0x8) || CTYPE(ctype, c, 0x1) || c == '.')) {
                        nan_arg[j++] = c;
                        c = fetch();
                    }

                    if (c != ')') {
                        scan_state = failure;
                        break;
                    } else
                        j++;
                }
                nan_arg[j] = '\0';

                if (sig_negative)
                    result = func_0200b608(0.0, func_02001710(nan_arg));
                else
                    result = func_02001710(nan_arg);

                *chars_scanned = spaces + i + j + sign_detected;
                return result;
            } else
                scan_state = failure;
            break;
        }

        case sig_start:
            if (c == '.') {
                scan_state = frac_start;
                c = fetch();
                break;
            }

            if (!isdigit(c)) {
                scan_state = failure;
                break;
            }

            if (c == '0') {
                c = fetch();
                if (toupper(c) == 'X') {
                    scan_state = hex_state;
                    hex_scan_state = hex_start;
                } else
                    scan_state = leading_sig_zeroes;
                break;
            }

            scan_state = int_digit_loop;
            break;

        case leading_sig_zeroes:
            if (c == '0') {
                c = fetch();
                break;
            }

            scan_state = int_digit_loop;
            break;

        case int_digit_loop:
            if (!isdigit(c)) {
                if (c == '.') {
                    scan_state = frac_digit_loop;
                    c = fetch();
                } else
                    scan_state = sig_end;
                break;
            }

            if (d.sig.length < 20) {
                int n = d.sig.length;
                d.sig.length = n + 1;
                ((unsigned char*)&d + n)[5] = c;
            } else
                exp_adjust++;

            c = fetch();
            break;

        case frac_start:
            if (!isdigit(c)) {
                scan_state = failure;
                break;
            }

            scan_state = frac_digit_loop;
            break;

        case frac_digit_loop:
            if (!isdigit(c)) {
                scan_state = sig_end;
                break;
            }

            if (d.sig.length < 20) {
                if (c != '0' || d.sig.length)
                    ((unsigned char*)&d + d.sig.length++)[5] = c;

                exp_adjust--;
            }

            c = fetch();
            break;

        case sig_end:
            if (toupper(c) == 'E') {
                scan_state = exp_start;
                c = fetch();
                break;
            }

            scan_state = finished;
            break;

        case exp_start:
            if (c == '+')
                c = fetch();
            else if (c == '-') {
                c = fetch();
                exp_negative = 1;
            }

            scan_state = leading_exp_digit;
            break;

        case leading_exp_digit:
            if (!isdigit(c)) {
                scan_state = failure;
                break;
            }

            if (c == '0') {
                scan_state = leading_exp_zeroes;
                c = fetch();
                break;
            }

            scan_state = exp_digit_loop;
            break;

        case leading_exp_zeroes:
            if (c == '0') {
                c = fetch();
                break;
            }

            scan_state = exp_digit_loop;
            break;

        case exp_digit_loop:
            if (!isdigit(c)) {
                scan_state = finished;
                break;
            }

            exp_value = exp_value * 10 + (c - '0');

            if (exp_value > SHRT_MAX)
                *overflow = 1;

            c = fetch();
            break;

        case hex_state:
            switch (hex_scan_state) {
            case hex_start:
                chptr = mantissa;
                chptr[0] = 0;
                chptr[1] = 0;
                chptr[2] = 0;
                chptr[3] = 0;
                chptr[4] = 0;
                chptr[5] = 0;
                chptr[6] = 0;
                chptr[7] = 0;
                mantissa_digits = (LDBL_MANT_DIG + 3) / 4;
                intdigits = 0;
                NibbleIndex = 0;
                hex_scan_state = hex_leading_sig_zeroes;
                c = fetch();
                break;

            case hex_leading_sig_zeroes:
                if (c == '0') {
                    c = fetch();
                    break;
                }

                hex_scan_state = hex_int_digit_loop;
                break;

            case hex_int_digit_loop:
                if (!isxdigit(c)) {
                    if (c == '.') {
                        hex_scan_state = hex_frac_digit_loop;
                        c = fetch();
                    } else
                        hex_scan_state = hex_sig_end;
                    break;
                }

                if (intdigits < mantissa_digits) {
                    intdigits++;
                    uch = *(chptr + NibbleIndex / 2);
                    ui = toupper(c);

                    if (ui >= 'A')
                        ui = ui - 'A' + 10;
                    else
                        ui -= '0';

                    uch1 = ui;

                    if ((NibbleIndex % 2) != 0)
                        uch |= uch1;
                    else
                        uch |= (unsigned char)(ui << 4);

                    *(chptr + NibbleIndex++ / 2) = uch;
                    c = fetch();
                } else
                    c = fetch();
                break;

            case hex_frac_digit_loop:
                if (!isxdigit(c)) {
                    hex_scan_state = hex_sig_end;
                    break;
                }

                if (intdigits < mantissa_digits) {
                    uch = *(chptr + NibbleIndex / 2);
                    ui = toupper(c);

                    if (ui >= 'A')
                        ui = ui - 'A' + 10;
                    else
                        ui -= '0';

                    uch1 = ui;

                    if ((NibbleIndex % 2) != 0)
                        uch |= uch1;
                    else
                        uch |= (unsigned char)(ui << 4);

                    *(chptr + NibbleIndex++ / 2) = uch;
                    c = fetch();
                } else
                    c = fetch();
                break;

            case hex_sig_end:
                if (toupper(c) == 'P') {
                    hex_scan_state = hex_exp_start;
                    c = fetch();
                } else
                    scan_state = finished;
                break;

            case hex_exp_start:
                if (c == '-')
                    expsign = 1;
                else if (c != '+') {
                    unfetch(c);
                    count--;
                }

                hex_scan_state = hex_leading_exp_digit;
                c = fetch();
                break;

            case hex_leading_exp_digit:
                if (!isdigit(c)) {
                    scan_state = failure;
                    break;
                }

                if (c == '0') {
                    hex_scan_state = hex_leading_exp_zeroes;
                    c = fetch();
                    break;
                }

                hex_scan_state = hex_exp_digit_loop;
                break;

            case hex_leading_exp_zeroes:
                if (c == '0') {
                    c = fetch();
                    break;
                }

                hex_scan_state = hex_exp_digit_loop;
                break;

            case hex_exp_digit_loop:
                if (!isdigit(c)) {
                    scan_state = finished;
                    break;
                }

                hex_exp = hex_exp * 10 + (c - '0');

                if (exp_value > SHRT_MAX)
                    *overflow = 1;

                c = fetch();
                break;
            }
            break;
        }
    }

    if (scan_state != hex_state ? !success(scan_state) : !hex_success(count, hex_scan_state)) {
        count = 0;
        *chars_scanned = 0;
    } else {
        count--;
        *chars_scanned = count + spaces;
    }

    unfetch(c);

    if (hex_scan_state == not_hex) {
        if (exp_negative)
            exp_value = -exp_value;

        {
            int n = d.sig.length;
            unsigned char* p = &d.sig.text[n];

            while (n-- && *--p == '0')
                exp_adjust++;

            d.sig.length = n + 1;

            if (d.sig.length == 0)
                d.sig.text[d.sig.length++] = '0';
        }

        exp_value += exp_adjust;

        if (exp_value < SHRT_MIN || exp_value > SHRT_MAX)
            *overflow = 1;

        if (*overflow) {
            if (exp_negative)
                return 0.0;
            else
                return sig_negative ? func_0200b608(0.0, HUGE_VAL) : HUGE_VAL;
        }

        d.exp = exp_value;

        result = func_0200a3a8(&d);

        if (result != 0.0 && result < LDBL_MIN) {
            *overflow = 1;
        } else if (result > LDBL_MAX) {
            *overflow = 1;
            result = HUGE_VAL;
        }

        if (sig_negative && success(scan_state))
            result = func_0200b608(0.0, result);

        return result;
    } else {
        long double dbl;
        unsigned char* dbl_bits = (unsigned char*)&dbl;
        unsigned char* lo;
        unsigned i;
        int top;
        int shift;
        int ff;
        unsigned mantissa_bit, dbl_bit;
        unsigned biased;
        int k, j;

        if (expsign)
            hex_exp = -hex_exp;

        uch = mantissa[0];
        hex_exp += intdigits * 4;

        top = 0x80;
        for (i = 0; i < 4 && !(uch & (top >> i)); i++)
            hex_exp--;

        shift = i + 1;
        if (shift != 0) {
            lo = mantissa;
            chptr = mantissa + 7;
            uch1 = 0;
            if (chptr >= lo) {
                int rshift = 8 - shift;
                do {
                    uch = *chptr;
                    *chptr-- = (uch << shift) | uch1;
                    uch1 = uch >> rshift;
                } while (chptr >= lo);
            }
        }

        dbl_bits[0] = 0;
        dbl_bits[1] = 0;
        dbl_bits[2] = 0;
        dbl_bits[3] = 0;
        dbl_bits[4] = 0;
        dbl_bits[5] = 0;
        dbl_bits[6] = 0;
        dbl_bits[7] = 0;

        k = 0;
        j = 1;
        mantissa_bit = 0;
        dbl_bit = 12;
        ff = 0xff;
        lo = mantissa;
        do {
            uch = lo[k];
            if (mantissa_bit + 8 > 52)
                uch &= ff << (52 - mantissa_bit);
            dbl_bits[j] |= (unsigned char)(uch >> (dbl_bit % 8));
            j++;
            dbl_bits[j] |= (unsigned char)(uch << (8 - dbl_bit % 8));
            mantissa_bit += 8;
            dbl_bit += 8;
            k++;
        } while (mantissa_bit < 52);

        biased = hex_exp + 0x3fe;

        if (biased & ~0x7ff) {
            *overflow = 1;
            return 0.0;
        }

        dbl_bits[0] |= (biased << 21) >> 25;
        dbl_bits[1] |= (biased << 21) >> 17;

        if (sig_negative)
            dbl_bits[0] |= 0x80;

        k = 0;
        do {
            uch = dbl_bits[k];
            dbl_bits[k] = dbl_bits[7 - k];
            dbl_bits[7 - k] = uch;
            k++;
        } while (k < 4);

        return *(long double*)dbl_bits;
    }
}
