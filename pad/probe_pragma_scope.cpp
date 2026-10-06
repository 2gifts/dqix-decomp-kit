// Does a pragma bracketed INSIDE a function body do anything? Workers keep reporting pragmas
// "inert when applied per-case, tightly bracketed with reset" (020055e4, 0209a218, 0200af44).

extern "C" void t_outside(int x, int *d)
{
    if (x & 0x20) { d[0] = 1; d[1] = 2; }
    if (x & 0x40) { d[2] = 3; d[3] = 4; }
}

extern "C" void t_inside(int x, int *d)
{
    if (x & 0x20) { d[0] = 1; d[1] = 2; }
#pragma optimize_for_size off
    if (x & 0x40) { d[2] = 3; d[3] = 4; }
#pragma optimize_for_size reset
}
