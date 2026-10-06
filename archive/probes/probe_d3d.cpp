extern "C" void *func_02012fe4(void);

struct Msg64 {
    unsigned char cmd;
    unsigned char pad1;
    unsigned short p1;
};

struct FooV {
    char unk[0x1840];
    struct BarV {
        char unk[0xb4c];
        volatile unsigned int num;
    } bar;
};

// y1: volatile field, member chain off the call
extern "C" int y1(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    ((FooV *)func_02012fe4())->bar.num |= (one << bit);
    return one;
}

struct Foo {
    char unk[0x1840];
    struct Bar {
        char unk[0xb4c];
        unsigned int num;
    } bar;
};

// y2: named char* base, explicit offset
extern "C" int y2(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    char *base = (char *)func_02012fe4();
    ((Foo::Bar *)(base + 0x1840))->num |= (one << bit);
    return one;
}

// y3: the address handed to an inline helper -- a real value with a call-shaped live range
static inline void setbit(Foo::Bar *b, int one, int bit) { b->num |= (one << bit); }
extern "C" int y3(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    setbit(&((Foo *)func_02012fe4())->bar, one, bit);
    return one;
}

// y4: the object is an array of the inner struct; element 0 at +0x1840 via a cast of the array
struct Outer {
    char unk[0x1840];
    Foo::Bar rows[2];
};
extern "C" int y4(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    ((Outer *)func_02012fe4())->rows[0].num |= (one << bit);
    return one;
}

// y5: read and write as separate statements through the member chain off the call, value named
extern "C" int y5(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    Foo *foo = (Foo *)func_02012fe4();
    unsigned int v = foo->bar.num | (one << bit);
    foo->bar.num = v;
    return one;
}
