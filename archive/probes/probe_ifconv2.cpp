// Where is mwcc's if-conversion threshold, and what disables it?

extern "C" void q4(int x, int *d)
{
    if (x & 0x20) { d[0] = 1; d[1] = 2; }
}

extern "C" void q5(int x, int *d)
{
    if (x & 0x20) { d[0] = 1; d[1] = 2; d[2] = 3; }
}

extern "C" void q6(int x, int *d)
{
    if (x & 0x20) { d[0] = 1; d[1] = 2; d[2] = 3; d[3] = 4; }
}

extern "C" void q8(int x, int *d)
{
    if (x & 0x20) { d[0] = 1; d[1] = 2; d[2] = 3; d[3] = 4; d[4] = 5; d[5] = 6; }
}

extern "C" int helper(int);

extern "C" void qcall(int x, int *d)
{
    if (x & 0x20) { d[0] = helper(x); d[1] = 2; }
}

extern "C" void qloop(int x, int *d)
{
    if (x & 0x20) { int i; for (i = 0; i < 3; i++) d[i] = i; }
}

extern "C" void qdiv(int x, int *d)
{
    if (x & 0x20) { d[0] = x / 7; d[1] = 2; }
}

extern "C" void qvol(int x, volatile int *d)
{
    if (x & 0x20) { d[0] = 1; d[1] = 2; }
}
