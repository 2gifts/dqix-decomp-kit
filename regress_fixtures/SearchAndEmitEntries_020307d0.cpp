#include <globaldefs.h>

struct AlignedValueSource020309d0;
extern "C" extern int _Z27ComputeAlignedValue020309d0iP26AlignedValueSource020309d0(int unused, struct AlignedValueSource020309d0* p);

struct SearchTable02030a24;
extern "C" extern void* _Z25BinarySearchByKey02030a24P19SearchTable02030a24i(struct SearchTable02030a24* table, int key);

struct Struct02030a84;
extern "C" extern char* _Z32GetEntryPointerOrDefault02030a84P14Struct02030a84i(struct Struct02030a84* s, int idx);

struct FoundHandler02030a24 {
    int key;
    void (*handler)(void*, int);
};

struct InnerTable020307d0 {
    int field0;
    int field4;
    int field8;
};

struct Entry020307d0 {
    int type;
    void* value;
};

struct Struct020307d0 {
    int field0;
    void* field4;
    void (*fallbackHandler)(unsigned short, void*, int);
    void* fieldC;
    int field10;
    struct InnerTable020307d0* field14;
    unsigned char* field18;
    char* field1C;
    char* field20;
    unsigned char* cursor;
    int index;
    struct Entry020307d0 entries[128];
    unsigned char flag42c;
};

// USA: func_020307d0
extern "C" ARM void* func_020307d0(struct Struct020307d0* self, void* param1) {
    unsigned char* prevCursor;
    unsigned char* cursor;
    int shouldProcess;

    if (self->fieldC == 0 || self->field10 <= 0 || self->field14 == 0) {
        return NULL;
    }
    if (self->index >= self->field14->field0) {
        return NULL;
    }

    prevCursor = self->cursor;
    if (prevCursor == 0) {
        self->cursor = self->field18;
    } else {
        int base = _Z27ComputeAlignedValue020309d0iP26AlignedValueSource020309d0((int)self, (struct AlignedValueSource020309d0*)prevCursor);
        int total = base + prevCursor[2] * 4;
        int aligned = total + (4 - total % 4) % 4;
        self->cursor = prevCursor + aligned;
    }

    cursor = self->cursor;
    if (cursor != 0 && param1 != 0) {
        int key = *(unsigned short*)cursor;
        int count = cursor[2];
        struct FoundHandler02030a24* found = (struct FoundHandler02030a24*)_Z25BinarySearchByKey02030a24P19SearchTable02030a24i((struct SearchTable02030a24*)self, key);

        shouldProcess = 0;
        if (found != 0) {
            shouldProcess = 1;
        } else if (self->flag42c == 0) {
            if (self->fallbackHandler != 0) {
                shouldProcess = 1;
            }
        }

        if (shouldProcess != 0) {
            unsigned char* entry = cursor;
            unsigned char* bits = entry + 3;
            int headerSize = _Z27ComputeAlignedValue020309d0iP26AlignedValueSource020309d0((int)self, (struct AlignedValueSource020309d0*)entry);
            int* data = (int*)(entry + headerSize);
            struct Entry020307d0* out = self->entries;
            unsigned char bitsVal = 0;
            int i;

            for (i = 0; i < count; i++) {
                if (i >= 128) {
                    break;
                }
                if (i % 4 == 0) {
                    if (i > 0) {
                        bits++;
                    }
                    bitsVal = *bits;
                } else {
                    bitsVal = bitsVal >> 2;
                }
                switch (bitsVal & 3) {
                case 0:
                    out->type = 0;
                    out->value = _Z32GetEntryPointerOrDefault02030a84P14Struct02030a84i((struct Struct02030a84*)self, *data);
                    break;
                case 1:
                    out->type = 1;
                    out->value = (void*)*data;
                    break;
                case 2:
                    out->type = 2;
                    out->value = (void*)*data;
                    break;
                default:
                    continue;
                }
                data++;
                out++;
            }

            if (found != 0) {
                found->handler(self->entries, count);
            } else {
                self->fallbackHandler(key, self->entries, count);
            }
        }
    }

    self->index = self->index + 1;
    return self->cursor;
}
