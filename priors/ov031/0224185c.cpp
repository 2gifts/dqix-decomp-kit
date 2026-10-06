#include <globaldefs.h>

struct Struct022407e4;
struct Obj022407f8;

struct HttpBlock_0224185c {
    HttpBlock_0224185c* next;
    signed char data[0x200];
};

struct HttpResp_0224185c {
    volatile int hdrLen;
    volatile int bodyLen;
    int ok;
    int bodyCap;
    char pad10[0x1c - 0x10];
    HttpBlock_0224185c* blocks;
    signed char hdr[0x400];
};

struct HttpHeader_0224185c {
    HttpHeader_0224185c* prev;
    HttpHeader_0224185c* next;
    char* name;
    char* value;
};

struct HttpParam_0224185c {
    HttpParam_0224185c* prev;
    HttpParam_0224185c* next;
    char* name;
    char* value;
    int valueLen;
    int isFile;
};

struct HttpReq_0224185c {
    int unk0;
    int canceled;
    int secure;
    int hostEnd;
    int pathStart;
    int contentLength;
    int unk18;
    int unk1c;
    void* sslCtx;
    char* url;
    int port;
    void* cbArg;
    HttpResp_0224185c* resp;
    HttpHeader_0224185c* headers;
    HttpParam_0224185c* params;
    void (*callback)(int, HttpResp_0224185c*, void*);
    int method;
    char boundary[0x16];
};

struct HttpJob_0224185c {
    void* prev;
    void* next;
    int id;
    HttpReq_0224185c* req;
    int sock;
};

struct PollFd_0224185c {
    int fd;
    short events;
    short revents;
};

struct Ring_0224185c { int head; int tail; };

void* AcquireData022918a4_02240310(void);
HttpJob_0224185c* CallFf38WithData_022402e0(void);
void ReleaseAllocatorRef_02240324(void);
int QueryGlobal022918bc_022403d8(void);
int TailCallCheckSlot_022413b8(int unused, int id);
void ResetAndInit32_02241318(void);
int IsFieldCLessOrEqual_022407e4(Struct022407e4* obj, unsigned int val);
int CallField30Adjusted_022407f8(Obj022407f8* obj, int b, int c, int d);

extern "C" int func_ov031_02240fc8(HttpReq_0224185c* req);
extern "C" int func_ov031_0220783c(PollFd_0224185c* fds, int n, int timeout, int flags);
extern "C" void func_ov031_0224175c(int sock, void** ctx);
extern "C" int func_ov031_02241338(HttpReq_0224185c* req);
extern "C" int func_ov031_02241404(HttpReq_0224185c* req, int sock, int addr, int port);
extern "C" void func_ov031_02241720(void* ctx);
extern "C" int func_ov031_022415a8(const char* s);
extern "C" int func_ov031_022415e0(HttpReq_0224185c* req, int sock, int* pos, const char* buf, int count);
extern "C" HttpHeader_0224185c* func_ov031_0223ff38(HttpHeader_0224185c** list);
extern "C" int func_ov031_02242b5c(const char* s);
extern "C" int func_ov031_02242dd4(signed char* out, int n);
extern "C" int func_ov031_02242bc4(signed char* out, int c);
extern "C" int func_ov031_022414cc(HttpReq_0224185c* req, int sock, char* buf, int count, int flags);
extern "C" int func_ov031_02241480(HttpReq_0224185c* req, int sock, void* buf, int count, int flags);
extern "C" int func_ov031_022406f8(HttpResp_0224185c* resp, char* dst, int start, int len);
extern "C" int func_ov031_02241540(const char* a, const char* b, int n);
extern "C" int func_ov031_02242d3c(char* str, int maxLen);
extern "C" int func_ov031_022404e8(HttpResp_0224185c* resp, int start, int end, int* outColon);
extern "C" int func_ov031_022410d8(HttpResp_0224185c* resp, const char* name, int* outStart);
extern "C" int func_ov031_02240628(HttpResp_0224185c* resp, int start, int end, const char* word, int delim);
extern "C" int func_ov031_02241694(HttpReq_0224185c* req, int sock);
extern "C" int func_ov031_02242c54(char* str, int len);
extern "C" int func_ov031_02240820(HttpReq_0224185c* req, int sock, int pos, int count, int flags);
extern "C" int func_ov031_02240e70(HttpReq_0224185c* req);
extern "C" void func_ov031_022417fc(void);

extern Ring_0224185c data_ov031_022919bc;
extern int data_ov031_02290fd0;
extern HttpJob_0224185c* data_ov031_02290fcc;
extern "C" void (* volatile data_ov031_02290fc8)(void*);
extern "C" void* (* volatile data_ov031_02290fc4)(int size, int align);
extern int data_ov031_02290fc0;
extern char data_ov031_02291a04[0x400];
extern char data_ov031_02291a0d[];
extern char data_ov031_0224c794[];
extern char data_ov031_0224c79c[];
extern char data_ov031_0224c7a4[];
extern char data_ov031_0224c7ac[];
extern char data_ov031_0224c7b0[];
extern char data_ov031_0224c7bc[];
extern char data_ov031_0224c7c4[];
extern char data_ov031_0224c7c8[];
extern char data_ov031_0224c7cc[];
extern char data_ov031_0224c7fc[];
extern char data_ov031_0224c830[];
extern char data_ov031_022494d8[];
extern char data_ov031_0224c844[];
extern char data_ov031_02249500[];
extern char data_ov031_0224c848[];
extern char data_ov031_0224c850[];
extern char data_ov031_0224c854[];
extern char data_ov031_0224c858[];
extern char data_ov031_0224c860[];
extern char data_ov031_0224c870[];
extern char data_ov031_0224c87c[];
extern char data_ov031_0224c888[];
extern char data_ov031_0224c89c[];

#define HTTP_SEND(str, len)                                                 \
    {                                                                       \
        int sent = func_ov031_022415e0(req, sock, &pos, (str), (len));      \
        if (sent < 0) goto done;                                            \
        if (sent != 0) {} else goto retry;                                  \
    }

// USA: func_ov031_0224185c
extern "C" ARM void func_ov031_0224185c(void) {
    signed char buf[12];
    void* ctx;
    int pos;
    int tmp;
    int colon;
    PollFd_0224185c pfd;
    char ver[3];
    HttpReq_0224185c* req;
    HttpResp_0224185c* resp;
    HttpJob_0224185c* job;
    int id;
    int addr;
    int lastAddr;
    int lastPort;
    int hasFile;
    int lastSecure;
    int err;
    int keepAlive;
    int sock;
    int same;
    int len;
    int total;
    int chunked;
    int chunkLen;
    int n;
    int i;

    sock = -1;
    ctx = 0;
    data_ov031_022919bc.head = 0;
    data_ov031_022919bc.tail = 0;
    hasFile = 0;
    lastAddr = -1;
    lastPort = -1;
    keepAlive = 0;
    lastSecure = 0;
    if (data_ov031_02290fd0 == 0) {
        do {
            AcquireData022918a4_02240310();
            job = CallFf38WithData_022402e0();
            if (job == 0) {
                id = -1;
            } else {
                id = job->id;
                req = job->req;
                data_ov031_02290fcc = job;
            }
            ReleaseAllocatorRef_02240324();
            if (id < 0) {
                QueryGlobal022918bc_022403d8();
                continue;
            }
            resp = req->resp;
            if (req->canceled != 0) goto done;
            addr = func_ov031_02240fc8(req);
            if (addr == 0) {
                err = 4;
                goto done;
            }
            same = 0;
            if (addr == lastAddr && req->port == lastPort && req->secure == lastSecure) same = 1;
            lastPort = req->port;
            lastSecure = req->secure;
            lastAddr = addr;
            keepAlive = keepAlive & same;
        retry:
            err = 0;
            if (sock >= 0) {
                pfd.events = 9;
                pfd.fd = sock;
                if (func_ov031_0220783c(&pfd, 1, 0xcc8d, 0) <= 0) {
                    keepAlive = 0;
                } else if (pfd.revents & 0xe0) {
                    keepAlive = 0;
                }
            }
            if (keepAlive == 0) {
                if (sock >= 0) {
                    if (TailCallCheckSlot_022413b8((int)req, sock) < 0) err = 10;
                    func_ov031_0224175c(sock, &ctx);
                    sock = -1;
                    if (err != 0) goto done;
                }
                sock = func_ov031_02241338(req);
                if (sock < 0) {
                    err = 3;
                    goto done;
                }
                if (req->secure != 0) {
                    ctx = req->sslCtx;
                    req->sslCtx = 0;
                }
                AcquireData022918a4_02240310();
                data_ov031_02290fcc->sock = sock;
                ReleaseAllocatorRef_02240324();
                if (req->canceled != 0) goto done;
                if (func_ov031_02241404(req, sock, addr, req->port) >= 0) {
                    keepAlive = 1;
                } else {
                    keepAlive = 0;
                }
            } else {
                if (req->secure != 0) {
                    func_ov031_02241720(req->sslCtx);
                    req->sslCtx = 0;
                }
                ResetAndInit32_02241318();
                AcquireData022918a4_02240310();
                data_ov031_02290fcc->sock = sock;
                ReleaseAllocatorRef_02240324();
            }
            if (req->canceled != 0) goto done;
            if (keepAlive == 0) {
                err = 5;
                goto done;
            }
            pos = 0;
            keepAlive = 0;
            len = func_ov031_022415a8(req->url);
            err = 10;
            switch (req->method) {
            case 0:
                HTTP_SEND(data_ov031_0224c794, 4);
                break;
            case 1:
                HTTP_SEND(data_ov031_0224c79c, 5);
                break;
            case 2:
                HTTP_SEND(data_ov031_0224c7a4, 5);
                break;
            }
            if (len > req->pathStart) {
                n = len - req->pathStart;
                if (n != 0) HTTP_SEND(req->url + req->pathStart, n);
            } else {
                HTTP_SEND(data_ov031_0224c7ac, 1);
            }
            HTTP_SEND(data_ov031_0224c7b0, 11);
            tmp = req->secure != 0 ? 8 : 7;
            HTTP_SEND(data_ov031_0224c7bc, 6);
            n = req->hostEnd - tmp;
            if (n != 0) HTTP_SEND(req->url + tmp, n);
            HTTP_SEND(data_ov031_0224c7c4, 2);
            {
                HttpHeader_0224185c* h = func_ov031_0223ff38(&req->headers);
                if (h != 0) do {
                    n = func_ov031_022415a8(h->name);
                    if (n != 0) HTTP_SEND(h->name, n);
                    HTTP_SEND(data_ov031_0224c7c8, 2);
                    n = func_ov031_022415a8(h->value);
                    if (n != 0) HTTP_SEND(h->value, n);
                    HTTP_SEND(data_ov031_0224c7c4, 2);
                    data_ov031_02290fc8(h);
                    h = func_ov031_0223ff38(&req->headers);
                } while (h != 0);
            }
            if (req->method == 1) {
                HttpParam_0224185c* list;
                HttpParam_0224185c* p;
                total = 0;
                list = req->params;
                hasFile = total;
                p = list;
                if (p != 0) do {
                    if (p->isFile != 0) {
                        hasFile = 1;
                        break;
                    }
                    if (p == list->prev) break;
                    p = p->next;
                } while (p != 0);
                if (hasFile != 0) {
                    if (list != 0) do {
                        total += 0x16;
                        total += func_ov031_022415a8(list->name) + 0x29;
                        if (list->isFile != 0) total += 0x4b;
                        total += 2;
                        total += list->valueLen;
                        total += 2;
                        if (list == req->params->prev) break;
                        list = list->next;
                    } while (list != 0);
                    total += 0x18;
                    HTTP_SEND(data_ov031_0224c7cc, 0x2c);
                    HTTP_SEND(req->boundary + 2, 0x12);
                    HTTP_SEND(data_ov031_0224c7c4, 2);
                } else {
                    if (list != 0) do {
                        total = total + func_ov031_02242b5c(list->name) + 1;
                        total += func_ov031_02242b5c(list->value);
                        if (list == req->params->prev) break;
                        list = list->next;
                        total++;
                    } while (list != 0);
                    HTTP_SEND(data_ov031_0224c7fc, 0x31);
                }
                HTTP_SEND(data_ov031_0224c830, 0x10);
                tmp = func_ov031_02242dd4(buf, total);
                if (tmp != 0) HTTP_SEND((char*)buf, tmp);
                HTTP_SEND(data_ov031_0224c7c4, 2);
            }
            HTTP_SEND(data_ov031_0224c7c4, 2);
            if (req->method == 1) {
                if (hasFile != 0) {
                    HttpParam_0224185c* p;
                    p = req->params;
                    if (p != 0) do {
                        HTTP_SEND(req->boundary, 0x14);
                        HTTP_SEND(data_ov031_0224c7c4, 2);
                        HTTP_SEND(data_ov031_022494d8, 0x26);
                        n = func_ov031_022415a8(p->name);
                        if (n != 0) HTTP_SEND(p->name, n);
                        HTTP_SEND(data_ov031_0224c844, 3);
                        if (p->isFile != 0) HTTP_SEND(data_ov031_02249500, 0x4b);
                        HTTP_SEND(data_ov031_0224c7c4, 2);
                        if (p->valueLen != 0) HTTP_SEND(p->value, p->valueLen);
                        HTTP_SEND(data_ov031_0224c7c4, 2);
                        if (p == req->params->prev) break;
                        p = p->next;
                    } while (p != 0);
                    HTTP_SEND(req->boundary, 0x14);
                    HTTP_SEND(data_ov031_0224c848, 4);
                } else {
                    HttpParam_0224185c* q;
                    q = req->params;
                    if (q != 0) do {
                        i = 0;
                        if (q->name[0] != 0) do {
                            tmp = func_ov031_02242bc4(buf, q->name[i]);
                            if (tmp != 0) HTTP_SEND((char*)buf, tmp);
                            i++;
                        } while (q->name[i] != 0);
                        HTTP_SEND(data_ov031_0224c850, 1);
                        i = 0;
                        if (q->value[0] != 0) do {
                            tmp = func_ov031_02242bc4(buf, q->value[i]);
                            if (tmp != 0) HTTP_SEND((char*)buf, tmp);
                            i++;
                        } while (q->value[i] != 0);
                        if (q == req->params->prev) break;
                        HTTP_SEND(data_ov031_0224c854, 1);
                        q = q->next;
                    } while (q != 0);
                }
            }
            if (pos > 0) {
                int sent = func_ov031_022414cc(req, sock, data_ov031_02291a04, pos, 0);
                if (sent < 0) goto done;
                if (sent == 0) goto retry;
            }
            resp->hdrLen = 0;
            buf[0] = 0;
            buf[1] = 0;
            buf[2] = 0;
            buf[3] = 0;
            {
                HttpBlock_0224185c* blk = resp->blocks;
                err = 7;
                pos = 0;
                for (;;) {
                    int got;
                    if (req->canceled != 0) goto done;
                    if (pos < 0x400) {
                        got = func_ov031_02241480(req, sock, resp->hdr + pos, 1, 0);
                        buf[pos & 3] = resp->hdr[pos];
                    } else {
                        int off = pos & 0x1ff;
                        if (off == 0) {
                            if (blk != 0) {
                                blk->next = (HttpBlock_0224185c*)data_ov031_02290fc4(0x204, 4);
                                blk = blk->next;
                            } else {
                                blk = (HttpBlock_0224185c*)data_ov031_02290fc4(0x204, 4);
                                resp->blocks = blk;
                            }
                            if (blk == 0) {
                                err = 1;
                                goto done;
                            }
                            blk->next = 0;
                        }
                        got = func_ov031_02241480(req, sock, (signed char*)blk + 4 + off, 1, 0);
                        buf[pos & 3] = blk->data[off];
                    }
                    if (got <= 0) {
                        err = 10;
                        goto done;
                    }
                    pos += got;
                    if (buf[(pos - 4) & 3] == '\r' && buf[(pos - 3) & 3] == '\n' &&
                        buf[(pos - 2) & 3] == '\r' && buf[(pos - 1) & 3] == '\n') {
                        resp->hdrLen = pos;
                        break;
                    }
                }
            }
            if (resp->hdrLen == 0) goto done;
            if (func_ov031_022406f8(resp, data_ov031_02291a04, 0, 0xe) == 0) goto done;
            if (func_ov031_02241540(data_ov031_02291a04, data_ov031_0224c858, 5) != 0 || data_ov031_02291a04[8] != ' ') goto done;
            if (func_ov031_02242d3c(data_ov031_02291a0d, 3) < 0) goto done;
            if (func_ov031_022404e8(resp, 0xc, resp->hdrLen, &colon) < 0) goto done;
            len = func_ov031_022410d8(resp, data_ov031_0224c860, &tmp);
            if (len == 0) {
                err = 0;
                goto done;
            }
            if (len > 0x400) goto done;
            if (len > 0) {
                if (func_ov031_022406f8(resp, data_ov031_02291a04, tmp, len) == 0) goto done;
                len = func_ov031_02242d3c(data_ov031_02291a04, len);
                if (len < 0) goto done;
                req->contentLength = len;
            } else {
                req->contentLength = -1;
            }
            n = func_ov031_022410d8(resp, data_ov031_0224c870, &tmp);
            keepAlive = n;
            if (n == 0) goto done;
            if (n < 0) {
                ver[0] = data_ov031_02291a04[5];
                ver[1] = data_ov031_02291a04[7];
                ver[2] = 0;
                keepAlive = func_ov031_02242d3c(ver, 2) >= 11;
            } else if (n > 0x400) {
                keepAlive = 0;
            } else {
                keepAlive = func_ov031_02240628(resp, tmp, tmp + keepAlive, data_ov031_0224c87c, 0) == 0;
            }
            n = func_ov031_022410d8(resp, data_ov031_0224c888, &tmp);
            if (n == 0) goto done;
            if (n > 0x400) {
                chunked = 0;
            } else if (n > 0) {
                chunked = func_ov031_02240628(resp, tmp, tmp + n, data_ov031_0224c89c, ';') == 0;
            } else {
                chunked = 0;
            }
            if (req->method == 2) goto done;
            if (len >= 0) {
                while (len > 0 && IsFieldCLessOrEqual_022407e4((Struct022407e4*)resp, resp->bodyLen) == 0) {
                    int got = func_ov031_02240820(req, sock, resp->bodyLen, len, 0);
                    if (got < 0) goto done;
                    if (got == 0) break;
                    len -= got;
                    resp->bodyLen += got;
                }
                if (len != 0) {
                    err = IsFieldCLessOrEqual_022407e4((Struct022407e4*)resp, resp->bodyLen) != 0 ? 6 : 10;
                    goto done;
                }
                err = 0;
                goto done;
            }
            err = 10;
            if (chunked != 0) {
                for (;;) {
                    signed char c;
                    buf[0] = 0;
                    buf[1] = 0;
                    pos = 0;
                    do {
                        if (func_ov031_02241480(req, sock, data_ov031_02291a04 + pos, 1, 0) < 0) goto done;
                        c = data_ov031_02291a04[pos];
                        buf[pos & 1] = c;
                        if (c == ';' || (c == 0xa && buf[(pos - 1) & 1] == 0xd)) {
                            if (c == 0xa) {
                                tmp = pos - 1;
                            } else {
                                tmp = pos;
                                if (func_ov031_02241694(req, sock) <= 0) goto done;
                            }
                            if (tmp == 0) goto done;
                            chunkLen = func_ov031_02242c54(data_ov031_02291a04, tmp);
                            if (chunkLen >= 0) break;
                            goto done;
                        }
                        pos++;
                    } while (pos < 0x400);
                    if (pos == 0x400) {
                        err = 7;
                        goto done;
                    }
                    if (chunkLen <= 0) break;
                    if (chunkLen > 0) do {
                        int got = func_ov031_02240820(req, sock, resp->bodyLen, chunkLen, 0);
                        if (got <= 0) goto done;
                        resp->bodyLen += got;
                        chunkLen -= got;
                        if (chunkLen == 0) {
                            if (func_ov031_02241480(req, sock, data_ov031_02291a04, 1, 0) <= 0) goto done;
                            if (func_ov031_02241480(req, sock, data_ov031_02291a04, 1, 0) <= 0) goto done;
                        }
                    } while (chunkLen > 0);
                }
                func_ov031_02241694(req, sock);
                err = 0;
                goto done;
            }
            if (IsFieldCLessOrEqual_022407e4((Struct022407e4*)resp, resp->bodyLen) == 0) {
                do {
                    int got = CallField30Adjusted_022407f8((Obj022407f8*)req, sock, resp->bodyLen, 0);
                    if (got < 0) goto done;
                    if (got == 0) {
                        err = 0;
                        goto done;
                    }
                    resp->bodyLen += got;
                    if (IsFieldCLessOrEqual_022407e4((Struct022407e4*)resp, resp->bodyLen) != 0) {
                        got = func_ov031_02241480(req, sock, data_ov031_02291a04, 1, 0);
                        if (got < 0) goto done;
                        if (got != 0) {
                            err = 6;
                            goto done;
                        }
                    }
                } while (IsFieldCLessOrEqual_022407e4((Struct022407e4*)resp, resp->bodyLen) == 0);
            }
        done:
            AcquireData022918a4_02240310();
            data_ov031_02290fc8(data_ov031_02290fcc);
            data_ov031_02290fcc = 0;
            ReleaseAllocatorRef_02240324();
            if (req->canceled != 0) err = 8;
            if (sock >= 0) {
                if (keepAlive == 0 || err != 0) {
                    if (TailCallCheckSlot_022413b8((int)req, sock) < 0) err = 10;
                    func_ov031_0224175c(sock, &ctx);
                    sock = -1;
                    keepAlive = 0;
                }
            }
            if (err == 0) {
                resp->ok = 1;
            } else {
                resp->ok = 0;
                data_ov031_02290fc0 = err;
            }
            {
                void* cbArg = req->cbArg;
                void (*cb)(int, HttpResp_0224185c*, void*) = req->callback;
                func_ov031_02240e70(req);
                cb(err, resp, cbArg);
            }
        } while (data_ov031_02290fd0 == 0);
    }
    if (sock >= 0) {
        TailCallCheckSlot_022413b8((int)req, sock);
        func_ov031_0224175c(sock, &ctx);
    }
    func_ov031_022417fc();
}
