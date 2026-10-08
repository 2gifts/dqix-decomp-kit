#include <globaldefs.h>

#include "Filesystem/FileIO.h"
#include "std_library_functions.h"

extern "C" void *__clear(void *destination, int count);
extern const char data_020f0049[24];
extern const char data_020f0061[24];

struct Entry0204254c {
    int val;
    char pad4;
    signed char field5 : 6;
    signed char unused2 : 2;
    char pad6[2];
};

struct KeyEnt020425e4 {
    unsigned short key;
    signed char val;
    unsigned char pad;
};

struct FieldIndexHeader {
    unsigned char unknown0[4];
    unsigned int entryCount;
    unsigned int keyCount;
    KeyEnt020425e4 *keys;
    Entry0204254c *entries;
    void *unknown14;
};

struct FieldDataHeader {
    unsigned char unknown0[12];
    void *data;
};

// USA: func_02042804
extern "C" ARM void func_02042804(void *receiver, const char *name, int *firstOutput, int *secondOutput) {
    SafeAllocator *allocator = static_cast<SafeAllocator *>(receiver);
    char path[64];
    __clear(path, sizeof(path));
    sprintf(path, data_020f0049, name);
    unsigned int fileSize = 0;
    LoadFileIntoMemory(path, data_0211e33c, &fileSize);
    if (fileSize != 0) {
        *firstOutput = reinterpret_cast<int>(allocator->Allocate(fileSize));
        memcpy(reinterpret_cast<void *>(*firstOutput), data_0211e33c, fileSize);
        FieldIndexHeader *header = reinterpret_cast<FieldIndexHeader *>(*firstOutput);
        char *base               = reinterpret_cast<char *>(header);
        header->keys             = reinterpret_cast<KeyEnt020425e4 *>(base + reinterpret_cast<int>(header->keys));
        header->entries          = reinterpret_cast<Entry0204254c *>(base + reinterpret_cast<int>(header->entries));
        header->unknown14        = base + reinterpret_cast<int>(header->unknown14);
        for (unsigned int i = 0; i < header->entryCount; ++i) {
            header->entries[i].val = reinterpret_cast<int>(base + header->entries[i].val);
        }
    }

    memset(path, 0, sizeof(path));
    sprintf(path, data_020f0061, name);
    fileSize = 0;
    LoadFileIntoMemory(path, data_0211e33c, &fileSize);
    if (fileSize != 0) {
        *secondOutput = reinterpret_cast<int>(allocator->Allocate(fileSize));
        memcpy(reinterpret_cast<void *>(*secondOutput), data_0211e33c, fileSize);
        FieldDataHeader *header = reinterpret_cast<FieldDataHeader *>(*secondOutput);
        header->data            = reinterpret_cast<char *>(header) + reinterpret_cast<int>(header->data);
    }
}
