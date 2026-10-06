// Re-test the two skiplist reasons that tonight's rules call into question.
//   0200f29c "mwcc dedupes identical pool words unconditionally; target has two"
//   020ca3ec "memset diamond whose bottom branch reuses the top cmp flags"

typedef unsigned int u32;
extern "C" void sink(u32);
extern "C" void a_side(void);
extern "C" void b_side(void);

// (1) two uses of the SAME 32-bit constant: one pool word or two?
extern "C" void pooldup(void)
{
    sink(0x12345678u);
    sink(0x12345678u);
}

// (2) can C reuse one cmp's flags for two conditional branches?
extern "C" void flagreuse(int n)
{
    if (n == 0) {
        a_side();
    } else if (n < 0) {
        b_side();
    }
}
