#include <globaldefs.h>
#include "std_library_functions.h"

struct RingBuffer020457e8 {
    char unk0[0x186c];
    int slots[4];
    char unk187c[0x131];
    signed char cursor;
};

struct ByteStream02044974 {
    unsigned char* cursor;
};

extern "C" ARM char* func_020449a4(struct RingBuffer020457e8* rb, char* msg);
ARM void WriteHalfwordToStream02044974(struct ByteStream02044974* stream, int value);
ARM int StringStartsWithCI020d85dc(const char* str, const char* prefix);
extern "C" ARM int func_02005a94(char* s);
ARM void PushToRingBuffer020457e8(struct RingBuffer020457e8* rb, int value);

extern char data_020f00a3[];

// SCRATCH-USA: func_0204483c
ARM void AppendFormattedMessage_0204483c(struct RingBuffer020457e8* rb, char* msg, struct ByteStream02044974 stream) {
    char* close;
    if (msg == 0 || stream.cursor == 0) {
        return;
    }
    msg = func_020449a4(rb, msg);
    for (;;) {
        signed char c = *msg;
        if (c == 0) {
            break;
        }
        signed char next = msg[1];
        if ((c == '\\' && next == 'n') || (c == '\r' && next == '\n')) {
            WriteHalfwordToStream02044974(&stream, 0xff18);
            msg += 2;
            continue;
        }
        if (c == '\n') {
            WriteHalfwordToStream02044974(&stream, 0xff18);
            msg += 1;
            continue;
        }
        if (c == '<') {
            int isTag = 1;
            close = strchr(msg, '>');
            if (close == 0) {
                break;
            }
            if (StringStartsWithCI020d85dc(msg + 1, data_020f00a3)) {
                int val = func_02005a94(msg + 6);
                if (val < 0) {
                    val = 0;
                }
                PushToRingBuffer020457e8(rb, val);
                WriteHalfwordToStream02044974(&stream, 0xff1a);
            } else {
                isTag = 0;
            }
            if (isTag) {
                msg = close + 1;
                continue;
            }
        }
        unsigned char* p = stream.cursor;
        stream.cursor = p + 1;
        *p = *msg++;
    }
    WriteHalfwordToStream02044974(&stream, 0xff01);
}
