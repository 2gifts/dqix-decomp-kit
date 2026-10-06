#include <globaldefs.h>
#include "System/Interrupts.h"
#include "System/ProcessorContext.h"

struct RingQueue020c7f44 {
    BlockedContextList notFullQueue;
    BlockedContextList notEmptyQueue;
    int* buffer;
    int capacity;
    int writeIndex;
    int producedCount;
};

// USA: func_020c7f44
ARM int PushToRingQueue020c7f44(RingQueue020c7f44* q, int value, int wait) {
    int w;
    int state = DisableIRQInterrupts();
    int cap = q->capacity;
    if (cap <= q->producedCount) {
        w = wait & 1;
        for (;;) {
            if (!w) {
                SetIRQInterruptState(state);
                return 0;
            }
            BlockCurrentContext(&q->notFullQueue);
            cap = q->capacity;
            if (cap > q->producedCount) break;
        }
    }
    int idx = (q->writeIndex + cap - 1) % cap;
    q->writeIndex = idx;
    q->buffer[idx] = value;
    q->producedCount = q->producedCount + 1;
    UnblockContexts(&q->notEmptyQueue);
    SetIRQInterruptState(state);
    return 1;
}
