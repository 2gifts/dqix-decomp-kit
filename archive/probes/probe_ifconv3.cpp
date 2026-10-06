// core.md line 69 calls `optimize_for_size off` "the lever when the target BRANCHES and you emit
// predicated instructions". Two workers (020055e4, 0200af44) report it inert. Measure it.

extern "C" void s4(int x, int *d)
{
    if (x & 0x20) { d[0] = 1; d[1] = 2; }
}

#pragma optimize_for_size off

extern "C" void s4_ofs_off(int x, int *d)
{
    if (x & 0x20) { d[0] = 1; d[1] = 2; }
}

#pragma optimization_level 1

extern "C" void s4_o1(int x, int *d)
{
    if (x & 0x20) { d[0] = 1; d[1] = 2; }
}

#pragma optimization_level 2
#pragma optimize_for_size on

extern "C" void s3(int x, int *d)
{
    if (x & 0x20) { d[0] = 1; }
}

extern "C" void s5(int x, int *d)
{
    if (x & 0x20) { d[0] = 1; d[1] = 2; d[2] = x; }
}

extern "C" void s4_else(int x, int *d)
{
    if (x & 0x20) { d[0] = 1; d[1] = 2; } else { d[0] = 3; d[1] = 4; }
}
