// BEST: 15 bytes, size exact (0x128). Residual = uniform r6<->r7 swap: target keeps the
// loop counter in r7 and the per-half value (total / active.records base) in r6; mine is
// reversed in both halves. Ruled out: colorsweep x3 (converged), 14 pragmas, i decl/assign
// placement (entry/per-half/before-after the count store), total as int, ptr-add operand
// order, hoisted src/dst bases, separate counters per half, condition flips.
// #pragma opt_propagation off + "int mode = 1;" INSIDE each loop body is what fixed the
// missing second "mov r5,#1" (was 4 bytes SHORT without it).
//
// 08-20, opus, 18 further variants (batches in vtry form). MECHANISM FOUND, NOT YET CLOSED.
// The callee-saved band ROTATES WITH THE NUMBER OF CALL-SURVIVING VALUES, one register per
// value. Measured twice, independently: hoisting a destination base (`hoist_dst`, one half)
// and hoisting both bases (`add_survivor_both`) each moved the counter r6 -> r5 (22 and 23
// bytes). Removing a value that does NOT survive the call does nothing: `drop_n` and
// `drop_mode` left the counter in r6. So reaching r7 needs ONE FEWER genuine survivor --
// that direction is inferred from the rotation, never achieved here, because every
// size-neutral way to drop a survivor is now disproven:
//   - definition order, all 4 permutations of {counter, invariant, count-store}: 35 bytes
//   - counter as unsigned int: 16 bytes; as unsigned char: 0x130, too long
//   - invariant as int AND defined first: 35 bytes
//   - invariant left in the condition so it never becomes live (`no_invariant`): 143 bytes
//   - struct assignment instead of memcpy, both halves: 0x130 (+8). memcpy IS correct.
//   - struct assignment + no null test: 0x12c (+4)
//   - dropping the redundant-looking `rec != NULL`, both halves: 0x124, four bytes SHORT.
//     The null test is genuinely in the target -- keep it.
//   - pointer induction instead of an index: 90 bytes
// Note for whoever picks this up: batches 1 and 2 edited only the FIRST loop while the
// residual spans both (9 runs), so those numbers understate what a symmetric change does.
// The remaining idea is to remove a survivor without changing code size -- the 0x40 memcpy
// size pinned in fp is the best candidate and struct-copy was the wrong way to attack it.
#pragma opt_propagation off
#include <globaldefs.h>
#include "std_library_functions.h"

struct Record_0209ed0c {
    unsigned int words[0x10];
};

struct RecordList_0209ed0c {
    int count;
    Record_0209ed0c records[0x10];
};

struct RecordOwner_0209ed0c {
    int unk0;
    char* data;
    char unk8[0x90];
    RecordList_0209ed0c active;
    RecordList_0209ed0c pending;
};

extern "C" int func_0209e1cc(RecordOwner_0209ed0c* owner, Record_0209ed0c* rec, int mode, int arg);

// USA: func_0209ed0c
extern "C" ARM void func_0209ed0c(RecordOwner_0209ed0c* owner, int rebuild, int arg)
{
    unsigned char total;
    int i;

    if (rebuild == 0) {
        i = 0;
        owner->active.count = 0;
        total = *(unsigned char*)(owner->data + 0x8e07);
        while (total > i) {
            int mode = 1;
            Record_0209ed0c* rec = (Record_0209ed0c*)(owner->data + 0x5c60) + i;
            if (rec != NULL && func_0209e1cc(owner, rec, mode, arg)) {
                int n = owner->active.count;
                owner->active.count = owner->active.count + 1;
                memcpy(&owner->active.records[n], rec, 0x40);
            }
            i++;
        }
        return;
    }

    owner->pending.count = 0;
    memset(owner->pending.records, 0, 0x400);
    i = 0;
    while (owner->active.count > i) {
        Record_0209ed0c* rec = &owner->active.records[i];
        int mode = 1;
        if (func_0209e1cc(owner, rec, mode, arg)) {
            int n = owner->pending.count;
            owner->pending.count = owner->pending.count + 1;
            memcpy(&owner->pending.records[n], rec, 0x40);
        }
        i++;
    }
    owner->active.count = owner->pending.count;
    memcpy(owner->active.records, owner->pending.records, 0x400);
}
