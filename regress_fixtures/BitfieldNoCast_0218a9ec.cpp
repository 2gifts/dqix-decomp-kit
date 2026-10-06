#include <globaldefs.h>

struct EntryA68 {
    short key;
    void* value;
};

struct TableA68 {
    struct EntryA68* entries;
    short count;
};

struct BattleField0218a9ec {
    unsigned int lo : 25;
    unsigned int flag : 1;
    unsigned int hi : 6;
};

extern "C" void* _ZN9GameState11GetInstanceEv();
int AppendFrameTag02041c08(char* dst, int a1, int a2, int a3, int a4, int a5);
int AppendCursorTag(char* dst, int cursor);
void* FindEntryByKey(struct TableA68* table, int key);
int AppendNameTag(char* dst, int n, const char* name);
int AppendString02042058(char* dst, const char* src);

// SCRATCH-USA: func_ov012_0218a9ec
extern "C" ARM void func_ov012_0218a9ec(unsigned char* self, char* dst, int c) {
    if (dst == 0) return;

    int cursor = *(int*)(self + 0x13f4) & 7;
    struct BattleField0218a9ec* fld =
        (struct BattleField0218a9ec*)((char*)_ZN9GameState11GetInstanceEv() + 0x569c);

    if (c != 0) {
        AppendFrameTag02041c08(dst, cursor, 8, 5, 5, 5);
    }
    AppendCursorTag(dst, cursor);

    int i;
    for (i = 0; i < 8; i++) {
        if (i != 0) {
            AppendString02042058(dst, *(char**)(self + 0x1378));
        }
        void* name = FindEntryByKey((struct TableA68*)(self + 0x133c),
                                    (short)(fld->flag + i * 2 + 0x7530));
        AppendNameTag(dst, i, (const char*)name);
    }
}
