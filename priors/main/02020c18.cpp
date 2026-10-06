#include <globaldefs.h>
#include "Filesystem/BackgroundLoader.h"

struct ResetStruct;
extern "C" int _ZN6Script10InitializeEv(struct ResetStruct* s);

struct StreamHeader;
struct StreamState;
extern "C" int _ZN6Script4LoadEPKvj(struct StreamState* s, struct StreamHeader* buffer, int length);

struct Struct02030774;
extern "C" int _ZN6Script7ExecuteEv(struct Struct02030774* p);

extern "C" void _ZN6Script15SetOpcodeLookupEPNS_17OpcodeLookupEntryE(void* state, void* dataPtr);

struct CtxParam02020c18 {
    int field0;
};

struct Global020fdc4c {
    char pad0[8];
    void* field8;
    char pad1[0x10 - 0xc];
    void* field10;
};
extern Global020fdc4c data_020fdc4c;
extern char data_020ef510[];

// USA: func_02020c18
ARM void RunBufferedScript02020c18(void* dst, CtxParam02020c18* ctx, void* p3) {
    char local[0x430];
    int inst = (int)BackgroundLoader::GetInstance();
    int bufferVal = 0;
    int lengthVal = 0;
    ((BackgroundLoader*)(inst))->GetLoadedFileByID(ctx->field0, (void**)&bufferVal, (unsigned int*)&lengthVal);
    data_020fdc4c.field10 = dst;
    int buffer = bufferVal;
    int length = lengthVal;
    data_020fdc4c.field8 = p3;
    _ZN6Script10InitializeEv((struct ResetStruct*)local);
    _ZN6Script15SetOpcodeLookupEPNS_17OpcodeLookupEntryE(local, data_020ef510);
    _ZN6Script4LoadEPKvj((struct StreamState*)local, (struct StreamHeader*)buffer, length);
    _ZN6Script7ExecuteEv((struct Struct02030774*)local);
    inst = (int)BackgroundLoader::GetInstance();
    ((BackgroundLoader*)(inst))->RemoveTask(ctx->field0);
    ctx->field0 = -1;
}
