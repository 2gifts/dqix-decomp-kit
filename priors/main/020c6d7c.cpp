#include <globaldefs.h>
#include "System/GamecardBusOwnership.h"

// KEEP-NAME: the ROM symbol is the mangled C++ name, not a func_ tag.
// SCRATCH-USA: func_020c6d7c

extern int data_021112dc;

#define REG_EXTMEMCTRL (*(volatile unsigned short*)0x04000204)
#define EXTMEMCTRL_FLAG_RELINQUISH_GBA_BUS (1 << 7)
#define EXTMEMCTRL_FLAG_RELINQUISH_NDS_BUS (1 << 11)

#define ADDR_REGISTERED_OWNERS_LOW 0x027fffb0
#define REGISTERED_OWNER_FLAGS ((unsigned int*)ADDR_REGISTERED_OWNERS_LOW)

#define PTR_UNKNOWN_BUS_LOCK ((GamecardBusLock*)0x027ffff0)

extern "C" {
    void WaitByLoop(int);
    // aligned memset clone
    void func_020ca3ec(int val, void* dst, unsigned len);
}

int WeakLockGamecardBusLock(unsigned short owner, GamecardBusLock* lock, void (*onLock)());
int WeakUnlockGamecardBusLock(unsigned short owner, GamecardBusLock* lock, void (*onUnlock)());

ARM void InitializeGamecardBusOwnership() {
    if (data_021112dc) {
        return;
    }

    GamecardBusLock* ndsLock = PTR_UNKNOWN_BUS_LOCK;
    data_021112dc = true;
    ndsLock->atomic = 0;

    WeakLockGamecardBusLock(126, ndsLock, NULL);

    if (ndsLock->unknown_6) {
        do {
            WaitByLoop(0x400);
        } while (ndsLock->unknown_6);
    }

    REGISTERED_OWNER_FLAGS[0] = 0xffffffff;
    REGISTERED_OWNER_FLAGS[1] = 0xffff0000;

    func_020ca3ec(0, (void*)0x027fffc0, 0x28);
    REG_EXTMEMCTRL |= EXTMEMCTRL_FLAG_RELINQUISH_NDS_BUS;
    REG_EXTMEMCTRL |= EXTMEMCTRL_FLAG_RELINQUISH_GBA_BUS;

    WeakUnlockGamecardBusLock(126, ndsLock, NULL);
    WeakLockGamecardBusLock(127, ndsLock, NULL);
}
