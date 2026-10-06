extern "C" int sink(int);

extern "C" int c3(int a, int b, int c)
{
    sink(0);
    sink(a);
    sink(b);
    sink(c);
    return a + b + c;
}

extern "C" int c3_locals(int a, int b, int c)
{
    int x = a + 1;
    int y = b + 2;
    int z = c + 3;
    sink(0);
    sink(x);
    sink(y);
    sink(z);
    return x + y + z;
}

extern "C" int c_mixed(int* p, int n)
{
    sink(0);
    sink(p[0]);
    sink(n);
    return n;
}
