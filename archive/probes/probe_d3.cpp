extern "C" void *func_02012fe4(void);

struct Foo {
    char unk[0x1840];
    struct Bar {
        char unk[0xb4c];
        unsigned int num;
    } bar;
};

// v1: explicit two-step address, anonymous
extern "C" int v1(int bit) {
    int one = 1;
    ((Foo::Bar *)((char *)func_02012fe4() + 0x1840))->num |= (one << bit);
    return one;
}

// v2: named pointer local
extern "C" int v2(int bit) {
    int one = 1;
    Foo *foo = (Foo *)func_02012fe4();
    Foo::Bar *bar = &foo->bar;
    bar->num |= (one << bit);
    return one;
}

// v3: member chain, compiler picks the split
extern "C" int v3(int bit) {
    int one = 1;
    Foo *foo = (Foo *)func_02012fe4();
    foo->bar.num |= (one << bit);
    return one;
}

// v4: named value, chain address
extern "C" int v4(int bit) {
    int one = 1;
    Foo *foo = (Foo *)func_02012fe4();
    unsigned int v = foo->bar.num;
    foo->bar.num = v | (one << bit);
    return one;
}

// v5: two-step address, value read and written through the SAME anonymous expression twice
extern "C" int v5(int bit) {
    int one = 1;
    Foo::Bar *bar = (Foo::Bar *)((char *)func_02012fe4() + 0x1840);
    unsigned int v = bar->num;
    bar->num = v | (one << bit);
    return one;
}

// v6: shift operand order flipped
extern "C" int v6(int bit) {
    int one = 1;
    ((Foo::Bar *)((char *)func_02012fe4() + 0x1840))->num =
        (one << bit) | ((Foo::Bar *)((char *)func_02012fe4() + 0x1840))->num;
    return one;
}
