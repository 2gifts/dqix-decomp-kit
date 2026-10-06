extern "C" void *func_02012fe4(void);

struct Msg64 {
    unsigned char cmd;
    unsigned char pad1;
    unsigned short p1;
};

struct Foo {
    char unk[0x1840];
    struct Bar {
        char unk[0xb4c];
        unsigned int num;
    } bar;
};

// x1: member chain straight off the call result, no named pointer
extern "C" int x1(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    ((Foo *)func_02012fe4())->bar.num |= (one << bit);
    return one;
}

// x2: same, but the inner struct reached by reference in one expression
extern "C" int x2(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    Foo::Bar &b = ((Foo *)func_02012fe4())->bar;
    b.num |= (one << bit);
    return one;
}

// x3: the outer struct is a union of the two views
union FooU {
    char raw[0x238c];
    Foo f;
};
extern "C" int x3(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    ((FooU *)func_02012fe4())->f.bar.num |= (one << bit);
    return one;
}

// x4: field offset expressed so the peel must leave 0xb4c -- a second member AFTER num,
// far enough that mwcc cannot fold the whole displacement
struct BarBig {
    char unk[0xb4c];
    unsigned int num;
    char tail[0x2000];
};
struct FooBig {
    char unk[0x1840];
    BarBig bar;
};
extern "C" int x4(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    ((FooBig *)func_02012fe4())->bar.num |= (one << bit);
    return one;
}
