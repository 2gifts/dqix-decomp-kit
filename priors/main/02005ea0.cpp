#include <globaldefs.h>

extern "C" ARM double func_0200ab28(double a, double b); // dadd
extern "C" ARM double func_0200b608(double a, double b); // dsub
extern "C" ARM double func_0200b0f0(double a, double b); // dmul
extern "C" ARM double _ddiv(double a, double b);          // ddiv
extern "C" ARM double func_0200b454(double x);            // sqrt
extern "C" ARM double func_02008f3c(double x);             // fabs
extern "C" ARM int func_0200bc78(double a, double b);      // a > b
extern "C" ARM int func_0200bd10(double a, double b);      // a <= b
extern "C" ARM double func_0200aae4(double val, int n);    // scalbn
extern "C" ARM double func_0200c578(void* p);               // domain-error handler
extern void* data_020eecc8;
extern int data_020f3390;
extern double data_020e6acc[2]; // bp
extern double data_020e6adc[2]; // dp_l
extern double data_020e6abc[2]; // dp_h

// USA: func_02005ea0
extern "C" ARM double func_02005ea0(double x, double y) {
    int hx = ((int*)&x)[1];
    int lx = ((int*)&x)[0];
    int hy = ((int*)&y)[1];
    int ly = ((int*)&y)[0];
    int ix = hx & 0x7fffffff;
    int iy = hy & 0x7fffffff;

    if ((iy | ly) == 0) return 1.0;

    if (ix > 0x7ff00000 || (ix == 0x7ff00000 && lx != 0) ||
        iy > 0x7ff00000 || (iy == 0x7ff00000 && ly != 0)) {
        return func_0200ab28(x, y);
    }

    int yisint = 0;
    if (hx < 0) {
        if (iy >= 0x43400000) {
            yisint = 2;
        } else if (iy >= 0x3ff00000) {
            int k = (iy >> 20) - 0x3ff;
            if (k > 20) {
                int j = (unsigned)ly >> (52 - k);
                if ((j << (52 - k)) == ly) yisint = 2 - (j & 1);
            } else if (ly == 0) {
                int j = iy >> (20 - k);
                if ((j << (20 - k)) == iy) yisint = 2 - (j & 1);
            }
        }
    }

    if (ly == 0) {
        if (iy == 0x7ff00000) {
            if (((ix - 0x3ff00000) | lx) == 0) {
                return func_0200b608(y, y);
            } else if (ix >= 0x3ff00000) {
                return (hy >= 0) ? y : 0.0;
            } else {
                return (hy < 0) ? func_0200b608(0.0, y) : 0.0;
            }
        }
        if (iy == 0x3ff00000) {
            if (hy < 0) return _ddiv(1.0, x); else return x;
        }
        if (hy == 0x40000000) return func_0200b0f0(x, x);
        if (hy == 0x3fe00000) {
            if (hx >= 0) return func_0200b454(x);
        }
    }

    double ax = func_02008f3c(x);
    if (lx == 0) {
        if (ix == 0x7ff00000 || ix == 0 || ix == 0x3ff00000) {
            double z = ax;
            if (hy < 0) z = _ddiv(1.0, z);
            if (hx < 0) {
                if (((ix - 0x3ff00000) | yisint) == 0) {
                    z = _ddiv(func_0200b608(z, z), func_0200b608(z, z));
                } else if (yisint == 1) {
                    z = func_0200b608(0.0, z);
                }
            }
            return z;
        }
    }

    if (((((unsigned)hx >> 31) - 1) | yisint) == 0) {
        data_020f3390 = 0x21;
        return func_0200c578(data_020eecc8);
    }

    double s = 1.0;
    if (((((unsigned)hx >> 31) - 1) | (yisint - 1)) == 0) s = -1.0;

    double t1, t2;
    if (iy > 0x41e00000) {
        if (iy > 0x43f00000) {
            if (ix <= 0x3fefffff) return (hy < 0) ? func_0200b0f0(1.0e300, 1.0e300) : func_0200b0f0(1.0e-300, 1.0e-300);
            if (ix >= 0x3ff00000) return (hy > 0) ? func_0200b0f0(1.0e300, 1.0e300) : func_0200b0f0(1.0e-300, 1.0e-300);
        }
        if (ix < 0x3fefffff) return (hy < 0) ? func_0200b0f0(1.0e300, 1.0e300) : func_0200b0f0(1.0e-300, 1.0e-300);
        if (ix > 0x3ff00000) return (hy > 0) ? func_0200b0f0(1.0e300, 1.0e300) : func_0200b0f0(1.0e-300, 1.0e-300);

        double t = func_0200b608(x, 1.0);
        double w = func_0200b0f0(func_0200b0f0(t, t),
                        func_0200b608(0.5, func_0200b0f0(t, func_0200b608(0.3333333333333333333333, func_0200b0f0(t, 0.25)))));
        double ivln2_h = 1.44269502162933349609e+00;
        double ivln2_l = 1.92596299112661746887e-08;
        double u = func_0200b0f0(ivln2_h, t);
        double v = func_0200b608(func_0200b0f0(t, ivln2_l), func_0200b0f0(w, 1.44269504088896338700e+00));
        t1 = func_0200ab28(u, v);
        ((int*)&t1)[0] = 0;
        t2 = func_0200b608(v, func_0200b608(t1, u));
    } else {
        int n = 0;
        double axl = ax;
        if (ix < 0x00100000) {
            axl = func_0200b0f0(ax, 9007199254740992.0);
            n -= 53;
            ix = ((int*)&axl)[1];
        }
        n += (ix >> 20) - 0x3ff;
        int j = ix & 0x000fffff;
        ix = j | 0x3ff00000;
        int k;
        if (j <= 0x3988E) k = 0;
        else if (j < 0xBB67A) k = 1;
        else { k = 0; n += 1; ix -= 0x00100000; }
        ((int*)&axl)[1] = ix;

        double u = func_0200b608(axl, data_020e6acc[k]);
        double v = _ddiv(1.0, func_0200ab28(axl, data_020e6acc[k]));
        double ss = func_0200b0f0(u, v);
        double s_h = ss;
        ((int*)&s_h)[0] = 0;
        double t_h = 0.0;
        ((int*)&t_h)[1] = ((ix >> 1) | 0x20000000) + 0x00080000 + (k << 18);
        double t_l = func_0200b608(axl, func_0200b608(t_h, data_020e6acc[k]));
        double s_l = func_0200b0f0(v, func_0200b608(func_0200b608(u, func_0200b0f0(s_h, t_h)), func_0200b0f0(s_h, t_l)));

        double s2 = func_0200b0f0(ss, ss);
        double rPoly = func_0200ab28(2.30660745775561754067e-01, func_0200b0f0(s2, 2.06975017800338417784e-01));
        rPoly = func_0200ab28(2.72728123808534006489e-01, func_0200b0f0(s2, rPoly));
        rPoly = func_0200ab28(3.33333329818377432918e-01, func_0200b0f0(s2, rPoly));
        rPoly = func_0200ab28(4.28571428578550184252e-01, func_0200b0f0(s2, rPoly));
        rPoly = func_0200ab28(5.99999999999994648725e-01, func_0200b0f0(s2, rPoly));
        double r = func_0200b0f0(func_0200b0f0(s2, s2), rPoly);
        r = func_0200ab28(r, func_0200b0f0(s_l, func_0200ab28(s_h, ss)));
        s2 = func_0200b0f0(s_h, s_h);
        t_h = func_0200ab28(3.0, func_0200ab28(s2, r));
        ((int*)&t_h)[0] = 0;
        t_l = func_0200b608(r, func_0200b608(func_0200b608(t_h, 3.0), s2));
        u = func_0200b0f0(s_h, t_h);
        v = func_0200ab28(func_0200b0f0(s_l, t_h), func_0200b0f0(t_l, ss));
        double p_h = func_0200ab28(u, v);
        ((int*)&p_h)[0] = 0;
        double p_l = func_0200b608(v, func_0200b608(p_h, u));
        double z_h = func_0200b0f0(9.61796700954437255859e-01, p_h);
        double z_l = func_0200ab28(func_0200ab28(func_0200b0f0(-7.02846165095275826516e-09, p_h),
                                    func_0200b0f0(p_l, 9.61796693925975554329e-01)), data_020e6adc[k]);
        double t = (double)n;
        t1 = func_0200ab28(func_0200ab28(func_0200ab28(z_h, z_l), data_020e6abc[k]), t);
        ((int*)&t1)[0] = 0;
        t2 = func_0200b608(z_l, func_0200b608(func_0200b608(func_0200b608(t1, t), data_020e6abc[k]), z_h));
    }

    double y1 = y;
    ((int*)&y1)[0] = 0;
    double p_l = func_0200ab28(func_0200b0f0(func_0200b608(y, y1), t1), func_0200b0f0(y, t2));
    double p_h = func_0200b0f0(y1, t1);
    double z = func_0200ab28(p_l, p_h);
    int j = ((int*)&z)[1];
    int i = ((int*)&z)[0];
    if (j >= 0x40900000) {
        if (((j - 0x40900000) | i) != 0) return func_0200b0f0(s, func_0200b0f0(1.0e300, 1.0e300));
        if (func_0200bc78(func_0200ab28(p_l, 8.0085662595372944372e-017), func_0200b608(z, p_h)))
            return func_0200b0f0(s, func_0200b0f0(1.0e300, 1.0e300));
    } else if ((j & 0x7fffffff) >= 0x4090cc00) {
        if (((j - 0xc090cc00) | i) != 0) return func_0200b0f0(s, func_0200b0f0(1.0e-300, 1.0e-300));
        if (func_0200bd10(p_l, func_0200b608(z, p_h)))
            return func_0200b0f0(s, func_0200b0f0(1.0e-300, 1.0e-300));
    }

    i = j & 0x7fffffff;
    int k2 = (i >> 20) - 0x3ff;
    int n = 0;
    if (i > 0x3fe00000) {
        n = j + (0x00100000 >> (k2 + 1));
        k2 = ((n & 0x7fffffff) >> 20) - 0x3ff;
        double t = 0.0;
        ((int*)&t)[1] = (n & ~(0x000fffff >> k2));
        n = ((n & 0x000fffff) | 0x00100000) >> (20 - k2);
        if (j < 0) n = -n;
        p_h = func_0200b608(p_h, t);
    }
    double t = func_0200ab28(p_l, p_h);
    ((int*)&t)[0] = 0;
    double u = func_0200b0f0(t, 6.93147182464599609375e-01);
    double v = func_0200ab28(func_0200b0f0(func_0200b608(p_l, func_0200b608(t, p_h)), 6.93147180559945286227e-01), func_0200b0f0(t, -1.90465429995776804525e-09));
    z = func_0200ab28(u, v);
    double w = func_0200b608(v, func_0200b608(z, u));
    t = func_0200b0f0(z, z);
    double t1Poly = func_0200ab28(-1.65339022054652515390e-06, func_0200b0f0(t, 4.13813679705723846039e-08));
    t1Poly = func_0200ab28(6.61375632143793436117e-05, func_0200b0f0(t, t1Poly));
    t1Poly = func_0200ab28(-2.77777777770155933842e-03, func_0200b0f0(t, t1Poly));
    t1Poly = func_0200ab28(1.66666666666666019037e-01, func_0200b0f0(t, t1Poly));
    double t1b = func_0200b608(z, func_0200b0f0(t, t1Poly));
    double r2 = func_0200b608(_ddiv(func_0200b0f0(z, t1b), func_0200b608(t1b, 2.0)), func_0200ab28(w, func_0200b0f0(z, w)));
    z = func_0200b608(1.0, func_0200b608(r2, z));
    j = ((int*)&z)[1];
    int combined = j + (n << 20);
    if ((combined >> 20) <= 0) {
        z = func_0200aae4(z, n);
    } else {
        ((int*)&z)[1] = combined;
    }
    return func_0200b0f0(s, z);
}
