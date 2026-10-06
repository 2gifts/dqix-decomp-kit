#include <globaldefs.h>

struct Status_0221c5ac {
    int state;
};

struct Packet_0221c5ac {
    int sock;
    int length;
    int pad8;
    unsigned char data[0x5dc];
};

struct Net_0221c5ac {
    int f0;
    void* handle;                 // 0x04
    int f8;
    Status_0221c5ac* status;      // 0x0c
    int error;                    // 0x10
    Packet_0221c5ac* packet;      // 0x14
    char pad18[0x2c - 0x18];
    unsigned int flags;           // 0x2c
    int f30;
    unsigned int addr;            // 0x34
    unsigned int mask;            // 0x38
};

struct Link_0221c5ac {
    char pad0[0x1c];
    signed char connected;        // 0x1c
};

struct Socket_0221c5ac {
    int sock;
    int active;
};

struct Retry_0221c5ac {
    short tries;
    short wait;
};


struct Session_0221c5ac {
    int retries;
    int reuse;
    Retry_0221c5ac recv;
};

struct SockAddr_0221c5ac {
    unsigned char len;
    unsigned char family;
    unsigned short port;
    unsigned int addr;
};

struct TimeVal_0221c5ac {
    int sec;
    int usec;
};

struct FdSet_0221c5ac {
    int head;
    short count;
    short limit;
};

struct Request_0221c5ac {
    unsigned int gateway;
    int f4;
    int f8;
    int fc;
    unsigned int host;
};

struct Conn_0221c5ac {
    char pad0[0x106];
    short connTries;              // 0x106
    short connWait;               // 0x108
    short recvTries;              // 0x10a
    short recvWait;               // 0x10c
    short timeout;                // 0x10e
    char config[6];               // 0x110
    unsigned char state;          // 0x116
};

extern "C" void func_ov031_0221e52c(void* dst, int value, unsigned int length);
extern "C" void func_ov031_0221d008(Conn_0221c5ac* self);
extern "C" void func_ov031_0221d060(int value);
extern "C" void func_ov031_0221cfdc(void);
extern "C" int func_ov031_0221d384(int id);
extern "C" void func_ov031_0221c4ac(void* arg);
extern "C" int func_ov031_0221e964(void* out);
extern "C" int func_ov031_0221d294(void* handle);
extern "C" void func_ov031_0221ecac(unsigned int arg);
extern "C" int func_ov031_0221d310(void* obj);
extern "C" void* func_ov031_0221c49c(int arg);
extern "C" int func_ov031_0221eadc(void* obj, Status_0221c5ac* status);
extern "C" int func_ov031_0221e480(unsigned int addr, unsigned int mask, unsigned int gateway);
extern "C" void func_ov031_0221d354(int kind, void* buf, void* config);
extern "C" int func_ov031_0221e5b4(int domain, int type, int protocol);
extern "C" int func_ov031_0221e5b0(int sock, int level, int name, void* value, int size);
extern "C" unsigned int func_ov031_0221e5d0(unsigned int value);
extern "C" unsigned short func_ov031_0221e5f8(short value);
extern "C" int func_ov031_0221e5bc(int sock, SockAddr_0221c5ac* addr, int size);
extern "C" int func_ov031_0221e5c8(int sock);
extern "C" int func_ov031_0221e4d8(void);
extern "C" unsigned int func_ov031_0221cfc4(unsigned int a, unsigned int b);
extern "C" int func_ov031_0221dc68(int mode, Request_0221c5ac* req, void* buf, int sock);
extern "C" void func_ov031_0221e6d4(FdSet_0221c5ac* fds);
extern "C" void func_ov031_0221e6e0(int head, FdSet_0221c5ac* fds);
extern "C" int func_ov031_0221e54c(int nfds, FdSet_0221c5ac* rd, void* wr, void* ex, TimeVal_0221c5ac* tv);
extern "C" int func_ov031_0221e538(int sock, void* buf, int size, int flags, SockAddr_0221c5ac* from, int* fromlen);
extern "C" int func_ov031_0221e638(unsigned short value);
extern "C" int func_ov031_0221d3b0(int mode, Packet_0221c5ac* packet, int* retries, void* buf, int sock);
extern "C" int func_ov031_0221d06c(void);
extern "C" int func_ov031_0221d078(Conn_0221c5ac* self);

extern Retry_0221c5ac data_ov031_022460dc;
extern Net_0221c5ac data_ov031_0224e6e0;
extern int data_ov031_0224e6e4;
extern Socket_0221c5ac data_ov031_0224b070;
extern Link_0221c5ac data_ov031_0224e700;

// USA: func_ov031_0221c5ac
extern "C" THUMB int func_ov031_0221c5ac(Conn_0221c5ac* self) {
    unsigned char config[0x18];
    unsigned char info[0x3c];
    Request_0221c5ac req;
    Retry_0221c5ac conn = data_ov031_022460dc;
    Session_0221c5ac ses;
    SockAddr_0221c5ac from;
    int fromlen;
    FdSet_0221c5ac fds;
    TimeVal_0221c5ac tv;
    SockAddr_0221c5ac sa;
    int ret;
    int mode;
    int res;
    unsigned int addr;
    Packet_0221c5ac* packet;
    int sec;
    int usec;
    int timeout;
    int result;
    short i;
    short wait;
    int st;

    ses.recv.tries = 0;
    mode = 0;
    ses.recv.wait = 0;
    ses.reuse = 1;
    ses.retries = 0;
    addr = 0;
    func_ov031_0221e52c(config, 0, 0x18);

    conn.tries = self->connTries;
    if (conn.tries == -1) {
        conn.tries = 10;
    }
    ses.recv.tries = self->recvTries;
    if (ses.recv.tries == -1) {
        ses.recv.tries = 10;
    }
    conn.wait = self->connWait;
    if (conn.wait == -1) {
        conn.wait = 100;
    }
    ses.recv.wait = self->recvWait;
    if (ses.recv.wait == -1) {
        ses.recv.wait = 100;
    }
    timeout = self->timeout;
    if (timeout == -1) {
        timeout = 2000;
    }

    func_ov031_0221d008(self);
    if ((data_ov031_0224e6e0.flags & 1) != 1) {
        func_ov031_0221d060(0x13);
        self->state = 0xf;
        func_ov031_0221cfdc();
        return -1;
    }

    i = 0;
    func_ov031_0221d384(0);
    wait = conn.wait;
    for (;;) {
        if (data_ov031_0224e6e0.handle != 0) {
            func_ov031_0221c4ac(data_ov031_0224e6e0.handle);
            data_ov031_0224e6e0.handle = 0;
        }
        if (func_ov031_0221e964(&data_ov031_0224e6e4) == -1) {
            self->state = 0xf;
            func_ov031_0221cfdc();
            return -1;
        }
        st = func_ov031_0221d294(data_ov031_0224e6e0.handle);
        if (st == 4) {
            self->state = 2;
            func_ov031_0221cfdc();
            return -1;
        }
        if (st == 0) {
            break;
        }
        if (i >= conn.tries) {
            self->state = 1;
            func_ov031_0221cfdc();
            return -1;
        }
        func_ov031_0221ecac(wait);
        i++;
    }

    func_ov031_0221d384(1);
    func_ov031_0221e52c(info, 0, 0x3c);
    if (func_ov031_0221d310(info) != 0) {
        self->state = 0xf;
        func_ov031_0221cfdc();
        return -1;
    }
    data_ov031_0224e6e0.status = (Status_0221c5ac*)func_ov031_0221c49c(0x58);
    if (data_ov031_0224e6e0.status == 0) {
        self->state = 0xf;
        func_ov031_0221cfdc();
        return -1;
    }
    func_ov031_0221e52c(data_ov031_0224e6e0.status, 0, 0x58);
    i = 0;
    if (conn.tries > 0) {
        do {
            st = func_ov031_0221eadc(info, data_ov031_0224e6e0.status);
            if (st == -1) {
                self->state = 0xf;
                func_ov031_0221cfdc();
                return -1;
            }
            if (st == 0) {
                if (st != 0 || data_ov031_0224e6e0.status->state == 1) {
                    break;
                }
            }
            func_ov031_0221ecac(wait);
            i++;
        } while (i < conn.tries);
    }
    if (i == conn.tries) {
        self->state = 0xf;
        func_ov031_0221cfdc();
        return -1;
    }

    if (func_ov031_0221e480(0xc0a80b65, 0xffffff00, 0xc0a80b65) != 0) {
        func_ov031_0221d060(0xc);
        self->state = 0xf;
        func_ov031_0221cfdc();
        return -1;
    }
    func_ov031_0221cfdc();
    func_ov031_0221d354(3, config, self->config);
    data_ov031_0224b070.sock = func_ov031_0221e5b4(2, 2, 0);
    if (data_ov031_0224b070.sock < 0) {
        self->state = 0xf;
        func_ov031_0221cfdc();
        return -1;
    }
    if (func_ov031_0221e5b0(data_ov031_0224b070.sock, 0xffff, 1, &ses.reuse, 4) < 0) {
        func_ov031_0221d060(0xb);
        self->state = 0xf;
        func_ov031_0221cfdc();
        return -1;
    }
    func_ov031_0221e52c(&sa, 0, 8);
    sa.family = 2;
    sa.addr = func_ov031_0221e5d0(0xc0a80b65);
    sa.port = func_ov031_0221e5f8(0x5790);
    if (func_ov031_0221e5bc(data_ov031_0224b070.sock, &sa, 8) < 0) {
        self->state = 0xf;
        func_ov031_0221cfdc();
        return -1;
    }

    for (;;) {
        packet = data_ov031_0224e6e0.packet;
        func_ov031_0221e52c(&req, 0, 0x14);
        req.host = 0xc0a80b65;
        req.gateway = 0xc0a80b01;
        sec = timeout / 1000;
        usec = timeout % 1000 * 1000;
        for (;;) {
            if (mode == 1 && data_ov031_0224e700.connected != 1) {
                if (data_ov031_0224b070.sock != -1) {
                    func_ov031_0221e5c8(data_ov031_0224b070.sock);
                    data_ov031_0224b070.sock = -1;
                }
                if (func_ov031_0221e4d8() != 0) {
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                data_ov031_0224e6e0.handle = func_ov031_0221c49c(0x58);
                if (data_ov031_0224e6e0.handle == 0) {
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                for (;;) {
                    if (data_ov031_0224e6e0.handle != 0) {
                        func_ov031_0221c4ac(data_ov031_0224e6e0.handle);
                        data_ov031_0224e6e0.handle = 0;
                    }
                    ret = func_ov031_0221e964(&data_ov031_0224e6e4);
                    if (ret == -1) {
                        self->state = 0xf;
                        func_ov031_0221cfdc();
                        return -1;
                    }
                    st = func_ov031_0221d294(data_ov031_0224e6e0.handle);
                    if (st == 4) {
                        self->state = 2;
                        func_ov031_0221cfdc();
                        return -1;
                    }
                    if (st == 0) {
                        break;
                    }
                    if (i >= conn.tries) {
                        self->state = 1;
                        func_ov031_0221cfdc();
                        return -1;
                    }
                    func_ov031_0221ecac(wait);
                    i++;
                }
                if (ret == -1) {
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                data_ov031_0224e6e0.status = (Status_0221c5ac*)func_ov031_0221c49c(0x58);
                if (data_ov031_0224e6e0.status == 0) {
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                func_ov031_0221e52c(data_ov031_0224e6e0.status, 0, 0x58);
                i = 0;
                if (conn.tries > 0) {
                    do {
                        st = func_ov031_0221eadc(info, data_ov031_0224e6e0.status);
                        if (st == -1) {
                            self->state = 0xf;
                            func_ov031_0221cfdc();
                            return -1;
                        }
                        if (st == 0) {
                            if (st != 0 || data_ov031_0224e6e0.status->state == 1) {
                                break;
                            }
                        }
                        func_ov031_0221ecac(wait);
                        i++;
                    } while (i < conn.tries);
                }
                if (i == conn.tries) {
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                addr = func_ov031_0221cfc4(data_ov031_0224e6e0.addr, data_ov031_0224e6e0.mask);
                if (func_ov031_0221e480(addr, data_ov031_0224e6e0.mask, addr) != 0) {
                    func_ov031_0221d060(0xc);
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                data_ov031_0224e700.connected = 1;
                func_ov031_0221cfdc();
                data_ov031_0224b070.sock = func_ov031_0221e5b4(2, 2, 0);
                if (data_ov031_0224b070.sock < 0) {
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                if (func_ov031_0221e5b0(data_ov031_0224b070.sock, 0xffff, 1, &ses.reuse, 4) < 0) {
                    func_ov031_0221d060(0xb);
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                func_ov031_0221e52c(&sa, 0, 8);
                sa.family = 2;
                sa.addr = func_ov031_0221e5d0(addr);
                sa.port = func_ov031_0221e5f8(0x5790);
                if (func_ov031_0221e5bc(data_ov031_0224b070.sock, &sa, 8) < 0) {
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
            }

            if (func_ov031_0221dc68(mode, &req, config, data_ov031_0224b070.sock) == -1) {
                func_ov031_0221d060(mode + 0x1000);
                self->state = 0xf;
                func_ov031_0221cfdc();
                return -1;
            }
            func_ov031_0221e52c(packet, 0, 0x5f8);
            func_ov031_0221e6d4(&fds);
            func_ov031_0221e6e0(data_ov031_0224b070.sock, &fds);
            tv.sec = sec;
            tv.usec = usec;
            if (func_ov031_0221e54c(data_ov031_0224b070.sock + 1, &fds, 0, 0, &tv) > 0) {
                break;
            }
            if (++ses.retries > ses.recv.tries) {
                if (mode == 0) {
                    func_ov031_0221d060(0xf);
                } else if (mode == 1) {
                    func_ov031_0221d060(0x10);
                } else {
                    func_ov031_0221d060(0x11);
                }
                result = -1;
                goto end;
            }
            func_ov031_0221ecac(ses.recv.wait);
        }

        fromlen = 8;
        st = func_ov031_0221e538(data_ov031_0224b070.sock, packet->data, 0x5dc, 0, &from, &fromlen);
        packet->sock = data_ov031_0224b070.sock;
        packet->length = func_ov031_0221e638(st);
        res = func_ov031_0221d3b0(mode, packet, &ses.retries, config, data_ov031_0224b070.sock);
        if (res == 100) {
            result = 0;
            goto end;
        }
        if (res == -1) {
            result = -1;
            goto end;
        }
        if (mode != res) {
            if (res == 2) {
                if (data_ov031_0224b070.sock != -1) {
                    func_ov031_0221e5c8(data_ov031_0224b070.sock);
                    data_ov031_0224b070.sock = -1;
                }
                if (func_ov031_0221e4d8() != 0) {
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                i = 0;
                func_ov031_0221d384(4);
                for (;;) {
                    if (data_ov031_0224e6e0.handle != 0) {
                        func_ov031_0221c4ac(data_ov031_0224e6e0.handle);
                        data_ov031_0224e6e0.handle = 0;
                    }
                    if (func_ov031_0221e964(&data_ov031_0224e6e4) == -1) {
                        self->state = 0xf;
                        func_ov031_0221cfdc();
                        return -1;
                    }
                    st = func_ov031_0221d294(data_ov031_0224e6e0.handle);
                    if (st == 4) {
                        self->state = 2;
                        func_ov031_0221cfdc();
                        return -1;
                    }
                    if (st == 0) {
                        break;
                    }
                    if (i >= conn.tries) {
                        self->state = 1;
                        func_ov031_0221cfdc();
                        return -1;
                    }
                    func_ov031_0221ecac(wait);
                    i++;
                }
                data_ov031_0224e6e0.status = (Status_0221c5ac*)func_ov031_0221c49c(0x58);
                if (data_ov031_0224e6e0.status == 0) {
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                func_ov031_0221e52c(data_ov031_0224e6e0.status, 0, 0x58);
                i = 0;
                if (conn.tries > 0) {
                    do {
                        st = func_ov031_0221eadc(info, data_ov031_0224e6e0.status);
                        if (st == -1) {
                            self->state = 0xf;
                            func_ov031_0221cfdc();
                            return -1;
                        }
                        if (st == 0) {
                            if (st != 0 || data_ov031_0224e6e0.status->state == 1) {
                                break;
                            }
                        }
                        func_ov031_0221ecac(wait);
                        i++;
                    } while (i < conn.tries);
                }
                if (i == conn.tries) {
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                if (func_ov031_0221e480(addr, data_ov031_0224e6e0.mask, addr) != 0) {
                    func_ov031_0221d060(0xc);
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                func_ov031_0221cfdc();
                data_ov031_0224b070.sock = func_ov031_0221e5b4(2, 2, 0);
                if (data_ov031_0224b070.sock < 0) {
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                if (func_ov031_0221e5b0(data_ov031_0224b070.sock, 0xffff, 1, &ses.reuse, 4) < 0) {
                    func_ov031_0221d060(0xb);
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
                func_ov031_0221e52c(&sa, 0, 8);
                sa.family = 2;
                sa.addr = func_ov031_0221e5d0(addr);
                sa.port = func_ov031_0221e5f8(0x5790);
                if (func_ov031_0221e5bc(data_ov031_0224b070.sock, &sa, 8) < 0) {
                    self->state = 0xf;
                    func_ov031_0221cfdc();
                    return -1;
                }
        }
            mode = res;
            continue;
        }
        mode = res;
        if (ses.retries > ses.recv.tries) {
            if (mode == 0) {
                func_ov031_0221d060(0xf);
            } else if (mode == 1) {
                func_ov031_0221d060(0x10);
            } else {
                func_ov031_0221d060(0x11);
            }
            result = -1;
            goto end;
        }
        func_ov031_0221ecac(ses.recv.wait);
    }

end:
    if (data_ov031_0224b070.sock != -1) {
        func_ov031_0221e5c8(data_ov031_0224b070.sock);
        data_ov031_0224b070.sock = -1;
    }
    if (func_ov031_0221e4d8() != 0) {
        self->state = 0xf;
        func_ov031_0221cfdc();
        return -1;
    }
    if (result != 0) {
        unsigned char code;
        switch (func_ov031_0221d06c()) {
        case 15:
            code = 3;
            break;
        case 16:
            code = 4;
            break;
        case 17:
            code = 5;
            break;
        case 20:
            code = 7;
            break;
        case 21:
            code = 8;
            break;
        default:
            code = 0xf;
            break;
        }
        self->state = code;
        func_ov031_0221cfdc();
        return -1;
    }
    if (func_ov031_0221d078(self) != 0) {
        self->state = 6;
        func_ov031_0221cfdc();
        return -1;
    }
    return 0;
}
