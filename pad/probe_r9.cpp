#include <globaldefs.h>

extern "C" int Sink(int);

extern "C" ARM int probe_r9(int a0, int a1) {
    int v0 = Sink(a0);
    int v1 = Sink(a0 + 1);
    int v2 = Sink(a0 + 2);
    int v3 = Sink(a0 + 3);
    int v4 = Sink(a0 + 4);
    int v5 = Sink(a0 + 5);
    int v6 = Sink(a0 + 6);
    int v7 = Sink(a0 + 7);
    return v0 + v1 + v2 + v3 + v4 + v5 + v6 + v7 + a1;
}
