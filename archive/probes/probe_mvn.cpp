#include <globaldefs.h>
struct E { unsigned char a[0x18]; };
extern "C" ARM void probe_mvn_a(E* e) { e->a[0xf] = -1; }
extern "C" ARM void probe_mvn_b(E* e) { e->a[0xf] = ~0; }
extern "C" ARM void probe_mvn_c(E* e) { int v = -1; e->a[0xf] = v; }
extern "C" ARM void probe_mvn_d(E* e) { e->a[0xf] = (unsigned char)-1; }
