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

// w1: faithful to the real case body -- bit comes from a memory load, not a parameter
extern "C" int w1(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    ((Foo::Bar *)((char *)func_02012fe4() + 0x1840))->num |= (one << bit);
    return one;
}

// w2: same, named pointer
extern "C" int w2(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    Foo *foo = (Foo *)func_02012fe4();
    Foo::Bar *bar = &foo->bar;
    bar->num |= (one << bit);
    return one;
}

// w3: the outer offset as an ARRAY index rather than a member offset
struct Cell {
    char unk[0x1840];
};
struct Bar2 {
    char unk[0xb4c];
    unsigned int num;
};
extern "C" int w3(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    Cell *c = (Cell *)func_02012fe4();
    ((Bar2 *)&c[1])->num |= (one << bit);
    return one;
}

// w4: outer offset through a typed pointer step, inner as the member
extern "C" int w4(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    Bar2 *b = (Bar2 *)((Cell *)func_02012fe4() + 1);
    b->num |= (one << bit);
    return one;
}

// w5: the value named, address explicit, store through the same named pointer
extern "C" int w5(Msg64 *msg) {
    int bit = msg->p1;
    int one = 1;
    unsigned int *np = (unsigned int *)((char *)func_02012fe4() + 0x1840);
    np[0xb4c / 4] |= (one << bit);
    return one;
}
