#include <globaldefs.h>
struct E { signed char a[0x18]; };
extern "C" ARM void probe_e(E* e) { e->a[0xf] = -1; }
struct F { unsigned char a[0x18]; };
extern "C" ARM void probe_f(F* e, int i) { e->a[0xf] = -1; e->a[0x14] = 0xff; }
extern "C" ARM void probe_g(F* e, int i) { e->a[0x14] = 0xff; e->a[0xf] = -1; }
