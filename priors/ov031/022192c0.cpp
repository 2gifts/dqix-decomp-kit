#include <globaldefs.h>
#include "std_library_functions.h"

struct ProcessorContext;

typedef void* (*AllocFn_022192c0)(const char* tag, unsigned int size);
typedef void (*FreeFn_022192c0)(const char* tag, void* p, int flags);

struct Base_022192c0 {
    char pad0[0x1004];
    int state;
    char pad1008[0x1108 - 0x1008];
    AllocFn_022192c0 alloc;
    FreeFn_022192c0 free;
    char pad1110[0x1114 - 0x1110];
    char* buf114;
    char* buf118;
};

struct Conn_022192c0 {
    char pad0[0x1020];
    int state;
    char pad1024[0x112c - 0x1024];
    int f112c;
    char pad1130[0x1a08 - 0x1130];
    char* location;
    char pad1a0c[0x1b38 - 0x1a0c];
    char proc[0x1ba4 - 0x1b38];
    int busy;
};

struct Request_022192c0 {
    const char* url;
    int method;
    int bufSize;
    AllocFn_022192c0 alloc;
    FreeFn_022192c0 free;
    int secure;
    int timeout;
};

struct Global_022192c0 {
    Base_022192c0* base;
    int redirected;
    int f8;
    int fc;
    int f10;
    Conn_022192c0* conn;
    Request_022192c0 req;
};

struct Session_022192c0 {
    unsigned short f0;
    char pad2[0x34 - 2];
    unsigned char f34;
    char pad35[0x40 - 0x35];
    AllocFn_022192c0 alloc;
    FreeFn_022192c0 free;
};

struct Clock_022192c0 {
    unsigned long long ticks;
    int pad[3];
};

struct Sched_022192c0 {
    int f0;
    ProcessorContext* proc;
};

extern "C" int func_ov031_02217c00(Conn_022192c0* conn, Request_022192c0* req);
extern "C" void func_ov031_022183b4(Conn_022192c0* conn);
extern "C" void func_ov031_0221a1d4(int result);
extern "C" int func_ov031_02217d1c(Conn_022192c0* conn);
extern "C" unsigned int _Z18GetContextPriorityP16ProcessorContext(ProcessorContext* ctx);
extern "C" void func_ov031_02217db4(Conn_022192c0* conn, int priority);
extern "C" void _Z22AwaitContextCompletionP16ProcessorContext(void* ctx);
extern "C" int func_ov031_02218b8c(Conn_022192c0* conn, int flag);
extern "C" char* func_ov031_02218e2c(Conn_022192c0* conn, const char* key);
extern "C" int func_02005a94(const char* s);
extern "C" int _Z28BuildAndApplyBuffer_022175dcPv(void* buf);
extern "C" int func_ov031_022178b4(Conn_022192c0* conn, void* buf, int flag);
extern "C" int func_ov031_02218604(Conn_022192c0* conn, const char* name, const char* value, int len);
extern "C" void func_ov031_0221ae00(Clock_022192c0* out);
extern "C" int func_ov031_022167c8(Session_022192c0* session, Conn_022192c0* conn);
extern "C" void _Z22AwaitIfActive_02216a84v(void);
extern "C" int _Z30GetFieldAndReleaseRef_02216ab4v(void);
extern "C" void func_ov031_02216b00(int* out);
extern "C" void _Z30InvokeHandlerAndClear_02216a2cv(void);
extern "C" int _Z25FindAndTerminate_02218e84PviPci(void* self, int key, char* buf, int size);
extern "C" int func_ov031_02218710(Conn_022192c0* conn, char* body);
extern "C" void _Z19SleepCurrentContextj(unsigned int ms);

extern Global_022192c0 data_ov031_0224e60c;
extern Request_022192c0 data_ov031_0224e624;
extern Session_022192c0 data_ov031_0224e640;
extern const char* data_ov031_0224a010;
extern const char* data_ov031_02249b54;
extern Sched_022192c0 data_02111304;
extern int data_020f3390;
extern char data_ov031_0224a124[];
extern char data_ov031_0224a130[];
extern char data_ov031_0224a150[];
extern char data_ov031_0224a158[];
extern char data_ov031_0224a160[];
extern char data_ov031_0224a170[];
extern char data_ov031_0224a178[];
extern char data_ov031_0224a180[];
extern char data_ov031_0224a18c[];
extern char data_ov031_0224a190[];
extern char data_ov031_0224a198[];
extern char data_ov031_0224a06c[];
extern char data_ov031_0224a088[];
extern char data_ov031_0224a0bc[];
extern char data_ov031_0224a0d8[];
extern char data_ov031_0224a0e4[];
extern char data_ov031_0224a0f4[];
extern char data_ov031_0224a104[];
extern char data_ov031_0224a1a0[];
extern char data_ov031_0224a1ac[];
extern char data_ov031_0224a1b8[];

#define G data_ov031_0224e60c

#define FAIL(code)                          \
    {                                       \
        func_ov031_022183b4(G.conn);        \
        func_ov031_0221a1d4(code);          \
        goto done;                          \
    }

// USA: func_ov031_022192c0
extern "C" ARM void func_ov031_022192c0(void) {
    int code;
    AllocFn_022192c0 alloc = G.base->alloc;
    FreeFn_022192c0 free = G.base->free;
    char* user = 0;
    char* pass = 0;
    char* waitStr = 0;
    unsigned int expected = 0;
    char* location;
    int userLen;
    int passLen;
    unsigned int wait;
    char* copy114;
    char* copy118;
    int waitLen;
    int status;
    char* redirectUrl;
    char* savedUrl;
    char* nextLocation;
    int info[0x71];
    char body[0x94];
    Clock_022192c0 now;
    char numBuf[4];

    for (;;) {
        G.req.url = data_ov031_0224a010;
        G.req.method = 1;
        G.req.bufSize = 0x1000;
        G.req.alloc = alloc;
        G.req.free = free;
        G.req.timeout = 20000;
        G.base->state = -2;
        if (func_ov031_02217c00(G.conn, &data_ov031_0224e624) != 0) FAIL(1);
        if (func_ov031_02217d1c(G.conn) != 0) FAIL(1);
        func_ov031_02217db4(G.conn, _Z18GetContextPriorityP16ProcessorContext(data_02111304.proc) - 1);
        if (G.conn->busy != 0) _Z22AwaitContextCompletionP16ProcessorContext(G.conn->proc);
        switch (G.conn->state) {
        case 2:
            G.base->state = -1;
        default:
            FAIL(3);
        case 8:
            break;
        }
        if (func_ov031_02218b8c(G.conn, 0) != 1) FAIL(2);

        code = func_02005a94(func_ov031_02218e2c(G.conn, data_ov031_0224a124));
        if (data_020f3390 == 0x22) {
            func_ov031_0221a1d4(2);
            goto done;
        }
        switch (code) {
        case 200:
            G.f8 = G.conn->f112c;
            break;
        case 302:
            G.redirected = 1;
            if (G.base->buf118 != 0) {
                G.base->state = -6;
                func_ov031_022183b4(G.conn);
                G.req.url = data_ov031_02249b54;
                G.req.method = 0;
                G.req.bufSize = 0x200;
                G.req.alloc = alloc;
                G.req.free = free;
                G.req.timeout = 20000;
                if (strcmp(data_ov031_02249b54, data_ov031_0224a130) != 0) G.req.secure = 1;
                if (func_ov031_02217c00(G.conn, &data_ov031_0224e624) != 0) FAIL(1);
                if (_Z28BuildAndApplyBuffer_022175dcPv(body) == 0 || func_ov031_022178b4(G.conn, body, 1) == 0) FAIL(8);
                if (func_ov031_02218604(G.conn, data_ov031_0224a150, data_ov031_0224a158, 7) != 0 ||
                    (redirectUrl = G.base->buf118,
                     func_ov031_02218604(G.conn, data_ov031_0224a160, redirectUrl, strlen(redirectUrl)) != 0)) FAIL(8);
                free(data_ov031_0224a088, G.base->buf118, 0);
                G.base->buf118 = 0;
                if (func_ov031_02217d1c(G.conn) != 0) FAIL(1);
                func_ov031_02217db4(G.conn, _Z18GetContextPriorityP16ProcessorContext(data_02111304.proc) - 1);
                if (G.conn->busy != 0) _Z22AwaitContextCompletionP16ProcessorContext(G.conn->proc);
                switch (G.conn->state) {
                case 2:
                    G.base->state = -1;
                default:
                    FAIL(3);
                case 8:
                    FAIL(7);
                }
            } else {
                location = G.conn->location;
                if (location == 0) FAIL(2);
                G.base->buf114 = (char*)alloc(data_ov031_0224a0bc, strlen(location) + 1);
                copy114 = G.base->buf114;
                if (copy114 == 0) FAIL(4);
                strncpy(copy114, location, strlen(location) + 1);
            }
            break;
        default:
            FAIL(10);
        }

        func_ov031_022183b4(G.conn);
        func_ov031_0221ae00(&now);
        if (now.ticks == expected) {
            G.base->state = -3;
            data_ov031_0224e640.f0 = 0;
            data_ov031_0224e640.f34 = 0;
            data_ov031_0224e640.alloc = G.base->alloc;
            data_ov031_0224e640.free = G.base->free;
            if (func_ov031_022167c8(&data_ov031_0224e640, G.conn) != 0) {
                func_ov031_0221a1d4(5);
                goto done;
            }
            _Z22AwaitIfActive_02216a84v();
            if (_Z30GetFieldAndReleaseRef_02216ab4v() != 0x15) {
                if (_Z30GetFieldAndReleaseRef_02216ab4v() == 9) {
                    G.base->state = -1;
                } else {
                    func_ov031_02216b00(info);
                    if (G.fc == 1 && (info[0] == (int)0xffffa4fa || _Z30GetFieldAndReleaseRef_02216ab4v() == 0xb)) {
                        G.base->state = 0;
                        _Z30InvokeHandlerAndClear_02216a2cv();
                        func_ov031_0221a1d4(0xb);
                        goto done;
                    }
                    G.base->state = info[0];
                }
                _Z30InvokeHandlerAndClear_02216a2cv();
                func_ov031_0221a1d4(6);
                goto done;
            }
            _Z30InvokeHandlerAndClear_02216a2cv();
        }
        if (code == 200) {
            G.base->state = 0;
            func_ov031_0221a1d4(0xb);
            goto done;
        }

        G.base->state = -4;
        G.req.url = data_ov031_02249b54;
        G.req.method = 0;
        G.req.bufSize = 0x1000;
        G.req.alloc = alloc;
        G.req.free = free;
        G.req.timeout = 40000;
        if (strcmp(data_ov031_02249b54, data_ov031_0224a130) != 0) G.req.secure = 1;
        if (func_ov031_02217c00(G.conn, &data_ov031_0224e624) != 0) FAIL(1);
        if (_Z28BuildAndApplyBuffer_022175dcPv(body) == 0 || func_ov031_022178b4(G.conn, body, 1) == 0) FAIL(8);
        if (func_ov031_02218604(G.conn, data_ov031_0224a150, data_ov031_0224a170, 5) != 0 ||
            (savedUrl = G.base->buf114,
             func_ov031_02218604(G.conn, data_ov031_0224a178, savedUrl, strlen(savedUrl)) != 0)) FAIL(8);
        free(data_ov031_0224a06c, G.base->buf114, 0);
        G.base->buf114 = 0;
        if (func_ov031_02217d1c(G.conn) != 0) FAIL(1);
        func_ov031_02217db4(G.conn, _Z18GetContextPriorityP16ProcessorContext(data_02111304.proc) - 1);
        if (G.conn->busy != 0) _Z22AwaitContextCompletionP16ProcessorContext(G.conn->proc);
        switch (G.conn->state) {
        case 3:
            func_ov031_022183b4(G.conn);
            if (G.fc == 1) {
                G.base->state = 0;
                func_ov031_0221a1d4(0xb);
            } else {
                func_ov031_0221a1d4(3);
            }
            goto done;
        case 2:
            G.base->state = -1;
        default:
            FAIL(3);
        case 8:
            break;
        }
        if (func_ov031_02218b8c(G.conn, 0) != 1) FAIL(2);

        code = func_02005a94(func_ov031_02218e2c(G.conn, data_ov031_0224a124));
        if (data_020f3390 == 0x22) FAIL(2);
        if (code != 200) {
            func_ov031_022183b4(G.conn);
            if (G.fc == 1 && code == 302) {
                G.base->state = 0;
                func_ov031_0221a1d4(0xb);
            } else {
                func_ov031_0221a1d4(2);
            }
            goto done;
        }

        if (_Z25FindAndTerminate_02218e84PviPci(G.conn, (int)data_ov031_0224a180, numBuf, 4) <= 0) FAIL(9);
        status = func_02005a94(numBuf);
        if (data_020f3390 == 0x22) FAIL(9);
        if (G.fc == 1 && status == 0x72) {
            G.base->state = 0;
            func_ov031_0221a1d4(0xb);
            goto done;
        }
        if (status >= 100) FAIL(6);

        userLen = _Z25FindAndTerminate_02218e84PviPci(G.conn, (int)data_ov031_0224a18c, 0, 0);
        if (userLen <= 0) FAIL(9);
        passLen = _Z25FindAndTerminate_02218e84PviPci(G.conn, (int)data_ov031_0224a190, 0, 0);
        if (passLen <= 0) FAIL(9);
        waitLen = _Z25FindAndTerminate_02218e84PviPci(G.conn, (int)data_ov031_0224a198, 0, 0);
        user = (char*)alloc(data_ov031_0224a0d8, userLen + 1);
        if (user == 0) FAIL(4);
        pass = (char*)alloc(data_ov031_0224a0e4, passLen + 1);
        if (pass == 0) FAIL(4);
        if (waitLen > 0) {
            waitStr = (char*)alloc(data_ov031_0224a0f4, waitLen + 1);
            if (waitStr == 0) FAIL(4);
        }
        code = _Z25FindAndTerminate_02218e84PviPci(G.conn, (int)data_ov031_0224a18c, user, userLen + 1);
        if (code < 0) FAIL(9);
        user[code] = 0;
        code = _Z25FindAndTerminate_02218e84PviPci(G.conn, (int)data_ov031_0224a190, pass, passLen + 1);
        if (code < 0) FAIL(9);
        pass[code] = 0;
        wait = 0;
        if (waitLen > 0) {
            code = _Z25FindAndTerminate_02218e84PviPci(G.conn, (int)data_ov031_0224a198, waitStr, waitLen + 1);
            if (code < 0) FAIL(9);
            waitStr[code] = 0;
            code = func_02005a94(waitStr);
            if (data_020f3390 == 0x22) FAIL(9);
            wait = code * 1000;
            if ((int)wait > 180000) wait = 180000;
        }
        func_ov031_022183b4(G.conn);

        G.base->state = -5;
        G.req.url = user;
        G.req.method = 0;
        G.req.bufSize = 0x1000;
        G.req.alloc = alloc;
        G.req.free = free;
        G.req.timeout = 120000;
        if (func_ov031_02217c00(G.conn, &data_ov031_0224e624) != 0) FAIL(1);
        if (func_ov031_02218710(G.conn, pass) != 0) FAIL(8);
        if (func_ov031_02217d1c(G.conn) != 0) FAIL(1);
        func_ov031_02217db4(G.conn, _Z18GetContextPriorityP16ProcessorContext(data_02111304.proc) - 1);
        if (G.conn->busy != 0) _Z22AwaitContextCompletionP16ProcessorContext(G.conn->proc);
        switch (G.conn->state) {
        case 2:
            G.base->state = -1;
        default:
            FAIL(3);
        case 8:
            break;
        }
        if (func_ov031_02218b8c(G.conn, 1) != 1) FAIL(2);
        nextLocation = G.conn->location;
        if (nextLocation == 0) FAIL(2);
        G.base->buf118 = (char*)alloc(data_ov031_0224a104, strlen(nextLocation) + 1);
        copy118 = G.base->buf118;
        if (copy118 == 0) FAIL(4);
        strncpy(copy118, nextLocation, strlen(nextLocation) + 1);
        func_ov031_022183b4(G.conn);
        _Z19SleepCurrentContextj(wait);
    }

done:
    if (user != 0) free(data_ov031_0224a1a0, user, 0);
    if (pass != 0) free(data_ov031_0224a1ac, pass, 0);
    if (waitStr == 0) return;
    free(data_ov031_0224a1b8, waitStr, 0);
}
