// Which source knob decides WHICH callee-saved register a PARAMETER lands in?
// 0201d044: ROM puts param a in r5 and count in r4; ours is reversed, and 20+ variants
// (decl hoists/reorders, param rebinds, pragmas, all 9 mwccarm 2.0.x builds) moved nothing.

extern "C" int sink(int);

// baseline: both params live across two calls
extern "C" int c_plain(int a, int count)
{
    sink(0);
    sink(a);
    sink(count);
    return a + count;
}

// first use reversed
extern "C" int c_userev(int a, int count)
{
    sink(0);
    sink(count);
    sink(a);
    return a + count;
}

// rebound to locals, declared a-then-count
extern "C" int c_bind_ac(int a, int count)
{
    int la = a;
    int lc = count;
    sink(0);
    sink(la);
    sink(lc);
    return la + lc;
}

// rebound to locals, declared count-then-a
extern "C" int c_bind_ca(int a, int count)
{
    int lc = count;
    int la = a;
    sink(0);
    sink(la);
    sink(lc);
    return la + lc;
}

// rebound count-then-a AND used count-first
extern "C" int c_bind_ca_userev(int a, int count)
{
    int lc = count;
    int la = a;
    sink(0);
    sink(lc);
    sink(la);
    return la + lc;
}
