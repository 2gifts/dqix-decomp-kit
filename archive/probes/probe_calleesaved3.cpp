// 02038138: "target keeps the single parameter in r4 as expected, mine doesn't" — with SEVERAL
// callee-saved values competing, which register does the parameter get, and what moves it?
// probe_calleesaved.cpp only measured functions whose parameters were the ONLY long-lived values.

extern "C" int sink(int);
extern "C" int load(int, int);

// param used directly, four locals defined after it
extern "C" int m_direct(int obj)
{
    int a = load(obj, 1);
    int b = load(obj, 2);
    int c = load(obj, 3);
    int d = load(obj, 4);
    sink(obj);
    sink(a);
    sink(b);
    sink(c);
    sink(d);
    return a + b + c + d;
}

// param rebound to a local FIRST, then the same four locals
extern "C" int m_rebound(int p)
{
    int obj = p;
    int a = load(obj, 1);
    int b = load(obj, 2);
    int c = load(obj, 3);
    int d = load(obj, 4);
    sink(obj);
    sink(a);
    sink(b);
    sink(c);
    sink(d);
    return a + b + c + d;
}

// param rebound LAST, after the other locals are declared
extern "C" int m_rebound_late(int p)
{
    int a = load(p, 1);
    int b = load(p, 2);
    int c = load(p, 3);
    int d = load(p, 4);
    int obj = p;
    sink(obj);
    sink(a);
    sink(b);
    sink(c);
    sink(d);
    return a + b + c + d;
}
