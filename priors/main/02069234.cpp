#include <globaldefs.h>
#include "std_library_functions.h"
#include "Combat/Main/BattleList.h"
struct BattleStruct {
    int unk0;
    int unk4;
    struct CombatantStruct* combatantList[0xe9];
};
struct CombatantStruct {
    unsigned short flags;
    char unk[0x132];
    struct BaseCombatStats* baseStats;
    struct ModifiableCombatStats* currentStats;
};
extern "C" struct CombatantStruct* _Z25GetCombatantWithFlag0x100P9GameStatei(struct BattleStruct* battleStruct, int combatantId);
extern "C" struct BattleStruct* _ZN9GameState11GetInstanceEv();

extern "C" char* FindUnescapedAngleBracket(char* str);
extern "C" int IsPrefixMatch020d857c(signed char* a, signed char* b);
extern "C" int StringLength(const char* s);
extern "C" int func_02005a94(void* p);
extern "C" int func_02001aec(void* a, void* b, int n);
extern "C" char* func_020e4958(char** dst, void* val, int a2, int a3, int a4, int a5);

typedef void (*TagHandler02069234)(char* tag, char** dst, void* sb, int index);

struct PrefixEntry02069234 {
    char* prefix;
    TagHandler02069234 handler;
};

struct OptEntry02069234 {
    char* key;
    signed char opcode;
    signed char value;
    char pad[2];
};

struct OptEntryBlock02069234 {
    int w0, w1, w2, w3;
};

struct StatusEffectEntry02069234 {
    void* f0;
    char* fmt;
    unsigned int kind : 4;
    unsigned int : 28;
    char pad2[0xc];
    short duration;
};

extern PrefixEntry02069234 data_020e7e8c[];
extern OptEntry02069234 data_020e7f04[16];
extern char data_020f0904[4];
extern char data_020f0909[6];
extern char data_020f0910[];

// SCRATCH-USA: func_02069234
extern "C" ARM void func_02069234(void* sb, char* str, char* dst) {
    char* cursor;
    if (str == 0 || dst == 0) {
        return;
    }
    cursor = dst;

    for (;;) {
        char c = *str;
        if (c == 0) {
            break;
        }
        if (c == '<') {
            char* gt = FindUnescapedAngleBracket(str);
            if (gt == 0) {
                goto copy_char;
            }
            char* tag = str + 1;
            int handled = 0;
            struct PrefixEntry02069234* rec = data_020e7e8c;
            for (; rec->prefix != 0; rec++) {
                if (!IsPrefixMatch020d857c((signed char*)tag, (signed char*)rec->prefix)) {
                    continue;
                }
                if (rec->handler != 0) {
                    int len = StringLength(rec->prefix);
                    int index = 0;
                    signed char d = tag[len];
                    if (d >= '1' && d <= '9') {
                        index = func_02005a94(tag + len) - 1;
                    }
                    rec->handler(tag, &cursor, sb, index);
                }
                str = gt + 1;
                handled = 1;
                break;
            }
            if (handled) {
                continue;
            }

            int local14 = -1;
            int local10 = 0;
            struct OptEntry02069234 opts[16];
            int localSl = -1;
            int* blkDst = (int*)opts;
            int* blkSrc = (int*)data_020e7f04;
            int localC = -1;
            for (int i = 8; i != 0; i--) {
                int t0 = blkSrc[0];
                int t2 = blkSrc[2];
                int t1 = blkSrc[1];
                int t3 = blkSrc[3];
                blkDst[0] = t0;
                blkDst[1] = t1;
                blkDst[2] = t2;
                blkDst[3] = t3;
                blkDst += 4;
                blkSrc += 4;
            }

            void* val = 0;
            int found = 0;
            struct OptEntry02069234* opt = opts;
            for (; opt->key != 0; opt++) {
                int len = StringLength(opt->key);
                if (func_02001aec(tag, opt->key, len) != 0) {
                    continue;
                }
                tag += len;
                int index = 0;
                found = 1;
                if (opt->value < 0) {
                    signed char d = *tag;
                    if (d >= '1' && d <= '9') {
                        index = func_02005a94(tag) - 1;
                    }
                }
                switch (opt->opcode) {
                    case 0: local14 = opt->value; break;
                    case 1: local10 = opt->value; break;
                    case 2: localC = opt->value; break;
                    case 3: localSl = opt->value; break;
                    case 4: val = *(void**)((char*)sb + 0x18 + index * 4); break;
                    case 5: val = *(void**)((char*)sb + 0x20 + index * 4); break;
                    case 6: val = *(void**)((char*)sb + index * 4); break;
                    case 7: val = *(void**)((char*)sb + 0x10 + index * 4); break;
                    case 8: val = *(void**)((char*)sb + 8 + index * 4); break;
                    case 9: val = *(void**)((char*)sb + 0x2c); break;
                }
            }

            if (val != 0) {
                cursor = func_020e4958(&cursor, val, local14, local10, localC, localSl);
            }
            if (!found) {
                goto copy_char;
            }
            str = gt + 1;
            continue;
        }
        if (c == '[') {
            if (func_02001aec(str, data_020f0904, 4) == 0) {
                str += 4;
                struct BattleStruct* bs = _ZN9GameState11GetInstanceEv();
                signed char id = *((signed char*)sb + 0x1880);
                struct CombatantStruct* combatant = _Z25GetCombatantWithFlag0x100P9GameStatei(bs, id);
                if (combatant != 0) {
                    char* table = *(char**)((char*)combatant + 0x150);
                    struct StatusEffectEntry02069234* eff = (struct StatusEffectEntry02069234*)(table + 0x294);
                    if (eff != 0) {
                        if (eff->duration > 0) {
                            if (eff->kind == 0) {
                                cursor += sprintf(cursor, eff->fmt);
                            }
                        }
                    }
                }
                goto skip_bracket;
            }
            if (func_02001aec(str, data_020f0909, 6) == 0) {
                str += 6;
                cursor += sprintf(cursor, (char*)sb + 0x3ac);
                goto skip_bracket;
            }
            *cursor++ = c;
            str++;
            continue;
        }
    copy_char:
        *cursor++ = *str++;
        continue;
    skip_bracket:
        while (*str != ']' && *str != 0) {
            str++;
        }
        if (*str == 0) {
            cursor += sprintf(cursor, data_020f0910);
            break;
        }
        str++;
    }
    *cursor = 0;
}
