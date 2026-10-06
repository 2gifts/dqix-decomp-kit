#include <globaldefs.h>

extern "C" void _Z15WriteByteIfRoomP18ByteWriter020d3084i(void* sink, int c);
extern "C" void _Z15FillSinkClampedP12Sink020d30b4ci(void* sink, char value, int count);
extern "C" void _Z15CopySinkClampedP12Sink020d3108Pci(void* sink, char* src, int count);

struct FmtSink020d3160 {
    int remaining;
    char* cursor;
    char* start;
};

#define NEXT_ARG(T) (*(T*)((args += 4) - 4))

// USA: func_020d3160
extern "C" ARM int func_020d3160(char* buf, int limit, const char* fmt, char* args) {
    char digits[24];
    struct FmtSink020d3160 sink;
    char prefix[4];
    int leftAlign;
    sink.remaining = limit;
    sink.cursor = buf;
    sink.start = buf;

    while (*fmt != 0) {
        int c = *fmt;
        if ((((unsigned char)c ^ 0x20) - 0xa1) < 0x3cu) {
            _Z15WriteByteIfRoomP18ByteWriter020d3084i(&sink, c);
            fmt++;
            if (*fmt != 0) {
                _Z15WriteByteIfRoomP18ByteWriter020d3084i(&sink, *fmt);
                fmt++;
            }
            continue;
        }
        if (c != '%') {
            fmt++;
            _Z15WriteByteIfRoomP18ByteWriter020d3084i(&sink, c);
            continue;
        }

        int flags = 0;
        int width = 0;
        int prec = -1;
        const char* spec = fmt;
        int base = 10;
        int hexoff = 0x57;

        for (;;) {
            c = *++fmt;
            switch (c) {
            case ' ':
                flags |= 1;
                continue;
            case '+':
                if (fmt[-1] == ' ') {
                    flags |= 2;
                    continue;
                }
                break;
            case '-':
                flags |= 8;
                continue;
            case '0':
                flags |= 0x10;
                continue;
            }
            break;
        }

        if (c == '*') {
            width = NEXT_ARG(int);
            fmt++;
            if (width < 0) {
                width = -width;
                flags |= 8;
            }
        } else {
            while (*fmt >= '0' && *fmt <= '9') {
                width = width * 10 + *fmt++ - '0';
            }
        }

        if (*fmt == '.') {
            prec = 0;
            if (*++fmt == '*') {
                prec = NEXT_ARG(int);
                fmt++;
                if (prec < 0) prec = -1;
            } else {
                while (*fmt >= '0' && *fmt <= '9') {
                    prec = prec * 10 + *fmt++ - '0';
                }
            }
        }

        if (*fmt == 'h') {
            if (*++fmt == 'h') {
                fmt++;
                flags |= 0x100;
            } else {
                flags |= 0x40;
            }
        } else if (*fmt == 'l') {
            if (*++fmt == 'l') {
                fmt++;
                flags |= 0x80;
            } else {
                flags |= 0x20;
            }
        }

        c = *fmt;
        switch (c) {
        case 'o':
            flags |= 0x1000;
            base = 8;
            goto integer;
        case 'u':
            flags |= 0x1000;
            goto integer;
        case 'd':
        case 'i':
            goto integer;
        case 'p':
            flags |= 4;
            prec = 8;
            goto hex;
        case 'X':
            hexoff = 0x37;
            goto hex;
        case 'x':
        hex:
            flags |= 0x1000;
            base = 16;
            goto integer;
        case 'c': {
            if (prec >= 0) goto other;
            int ch = NEXT_ARG(int);
            if (flags & 8) {
                _Z15WriteByteIfRoomP18ByteWriter020d3084i(&sink, (char)ch);
                _Z15FillSinkClampedP12Sink020d30b4ci(&sink, ' ', width - 1);
            } else {
                char pad = (flags & 0x10) ? '0' : ' ';
                _Z15FillSinkClampedP12Sink020d30b4ci(&sink, pad, width - 1);
                _Z15WriteByteIfRoomP18ByteWriter020d3084i(&sink, (char)ch);
            }
            fmt++;
            break;
        }
        case 's': {
            char* str = NEXT_ARG(char*);
            int len = 0;
            if (prec < 0) {
                if (str[0] != 0) {
                    do {
                        len++;
                    } while (str[len] != 0);
                }
            } else {
                while (len < prec && str[len] != 0) {
                    len++;
                }
            }
            width -= len;
            if (flags & 8) {
                _Z15CopySinkClampedP12Sink020d3108Pci(&sink, str, len);
                _Z15FillSinkClampedP12Sink020d30b4ci(&sink, ' ', width);
            } else {
                char pad = (flags & 0x10) ? '0' : ' ';
                _Z15FillSinkClampedP12Sink020d30b4ci(&sink, pad, width);
                _Z15CopySinkClampedP12Sink020d3108Pci(&sink, str, len);
            }
            fmt++;
            break;
        }
        case 'n': {
            int n = sink.cursor - sink.start;
            if (!(flags & 0x100)) {
                if (flags & 0x40) {
                    *NEXT_ARG(short*) = n;
                } else if (!(flags & 0x80)) {
                    *NEXT_ARG(int*) = n;
                } else {
                    long long* p = NEXT_ARG(long long*);
                    *p = n;
                }
            }
            fmt++;
            break;
        }
        case '%':
            if (spec + 1 == fmt) {
                fmt++;
                _Z15WriteByteIfRoomP18ByteWriter020d3084i(&sink, c);
                break;
            }
            goto other;
        default:
        other:
            _Z15CopySinkClampedP12Sink020d3108Pci(&sink, (char*)spec, fmt - spec);
            break;
        integer: {
            int plen;
            int ndig;
            unsigned long long v;
            if (flags & 8) flags &= ~0x10;
            if (prec < 0) prec = 1; else flags &= ~0x10;
            plen = 0;
            if (flags & 0x1000) {
                if (flags & 0x100) {
                    v = NEXT_ARG(unsigned char);
                } else if (flags & 0x40) {
                    v = NEXT_ARG(unsigned short);
                } else if (!(flags & 0x80)) {
                    v = NEXT_ARG(unsigned int);
                } else {
                    args += 8;
                    v = *(unsigned long long*)(args - 8);
                }
                flags &= ~3;
                if (flags & 4) {
                    if (base == 16) {
                        if (v != 0) {
                            prefix[0] = hexoff + 0x21;
                            prefix[1] = '0';
                            plen = 2;
                        }
                    } else if (base == 8) {
                        prefix[0] = '0';
                        plen = 1;
                    }
                }
            } else {
                long long sv;
                if (flags & 0x100) {
                    sv = NEXT_ARG(signed char);
                } else if (flags & 0x40) {
                    sv = NEXT_ARG(short);
                } else if (!(flags & 0x80)) {
                    sv = NEXT_ARG(int);
                } else {
                    args += 8;
                    sv = *(long long*)(args - 8);
                }
                v = sv;
                if (v & 0x8000000000000000ULL) {
                    v = ~v + 1;
                    prefix[0] = '-';
                    plen = 1;
                } else if (v != 0 || prec != 0) {
                    if (flags & 2) {
                        prefix[0] = '+';
                        plen = 1;
                    } else if (flags & 1) {
                        prefix[0] = ' ';
                        plen = 1;
                    }
                }
            }

            ndig = 0;
            if (base == 8) {
                if (v != 0) {
                    do {
                        digits[ndig++] = (v & 7) + '0';
                        v >>= 3;
                    } while (v != 0);
                }
            } else if (base == 10) {
                if ((v >> 32) == 0) {
                    unsigned int lo = (unsigned int)v;
                    if (lo != 0) {
                        do {
                            unsigned int q = (unsigned int)(((unsigned long long)lo * 0xcccccccdU) >> 35);
                            digits[ndig++] = lo - q * 10 + '0';
                            lo = q;
                        } while (lo != 0);
                    }
                } else {
                    do {
                        unsigned long long q = v / 10;
                        digits[ndig++] = v - q * 10 + '0';
                        v = q;
                    } while (v != 0);
                }
            } else if (base == 16) {
                if (v != 0) {
                    do {
                        int d = v & 0xf;
                        v >>= 4;
                        digits[ndig++] = d < 10 ? d + '0' : d + hexoff;
                    } while (v != 0);
                }
            }

            if (plen > 0 && prefix[0] == '0') {
                digits[ndig++] = '0';
                plen = 0;
            }
            prec -= ndig;
            if (flags & 0x10) {
                int t = width - ndig - plen;
                if (prec < t) prec = t;
            }
            if (prec > 0) width -= prec;
            width -= plen + ndig;
            leftAlign = flags & 8;
            if (!leftAlign) {
                _Z15FillSinkClampedP12Sink020d30b4ci(&sink, ' ', width);
            }
            if (plen > 0) {
                char* p = prefix + plen;
                do {
                    _Z15WriteByteIfRoomP18ByteWriter020d3084i(&sink, *--p);
                    plen--;
                } while (plen > 0);
            }
            _Z15FillSinkClampedP12Sink020d30b4ci(&sink, '0', prec);
            if (ndig > 0) {
                char* p = digits + ndig;
                do {
                    _Z15WriteByteIfRoomP18ByteWriter020d3084i(&sink, *--p);
                    ndig--;
                } while (ndig > 0);
            }
            if (leftAlign) {
                _Z15FillSinkClampedP12Sink020d30b4ci(&sink, ' ', width);
            }
            fmt++;
        }
        }
    }

    if (sink.remaining != 0) {
        *sink.cursor = 0;
    } else if (limit != 0) {
        sink.start[limit - 1] = 0;
    }
    return sink.cursor - sink.start;
}
