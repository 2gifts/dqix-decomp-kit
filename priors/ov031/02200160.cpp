// NEAR-MISS: 9 bytes, 1 run, at 0x10-0x1a. Size exact (0x104). Everything else byte-identical,
// including both pool loads of 0x0224c980, all register numbers, both loop shapes and the pool.
//
// Residue is one instruction-order rotation in the prologue:
//   target: ldr r1,[r1,#0x50] ; str r0,[r2,#0xc] ; cmp r1,#0 ; mov r1,#0
//   ours:   ldr r1,[r1,#0x50] ; cmp r1,#0 ; mov r1,#0 ; str r0,[r2,#0xc]
// The naive (unscheduled) order is [ldr][cmp][str][mov] -- confirmed with -proc arm7ej, which
// emits exactly that. arm946e hoists `mov r1,#0` one slot; the ROM hoists `str r0,[r2,#0xc]`
// one slot instead. Same instruction set, one adjacent swap.
//
// Ruled out by measurement (do NOT redo):
// - colorsweep.py --apply: 80 compiles, no change.
// - flagsweep.sh, and -opt speed / speed,level=4 / nospace / nocse / nopropagation / nolifetimes /
//   nopeephole / noschedule, -ipa file, -inline all|none, -constpool, -rostr, -nointerworking,
//   -sdatathreshold 0, -str reuse, -enum min, -char unsigned: all 9 bytes.
// - ccsweep.sh (all 24 mwccarm builds) with default flags AND with "-opt speed": every 2.0 build
//   gives 9; 1.2 builds overgen, dsi builds undergen.
// - every -proc value: arm946e/arm9ej/arm926ej/arm966e/v5te = 9; arm7ej = 22; v5t/arm1020e/
//   arm1022e/XScale/arm1026ej = 50 (unscheduled); v4/v4t/arm9xxt/dbmx* overgen.
// - 25 pragmas: optimize_for_size, opt_common_subs, scheduling on|off, opt_propagation,
//   peephole, opt_lifetimes, global_optimizer, opt_dead_assignments, opt_dead_code,
//   register_coloring, opt_strength_reduction, inline, opt_pointer_analysis,
//   optimization_level 2|3|4, opt_unroll_loops, opt_loop_invariants, opt_arithtransformation,
//   ARM_conform, load_store_elimination: all byte-identical.
// - -lang c and -lang c99 (full C rewrite of this file): also 9.
// - ~60 source shapes for the prologue. The three families and why each fails:
//     read-first + separate store stmt (this file)  -> two pool loads + both movne/moveq: CORRECT
//        allocation, but the scheduler leaves `str` after `mov r1,#0`. 9 bytes.
//     store-first (any spelling, incl. char* cast, local ptr, volatile field50, inline setter)
//        -> mwcc CSEs the base: one pool load, store uses the read's base. 31 bytes.
//     read-into-temp then store then compare (the exact statement order the ROM emits)
//        -> mwcc CSEs the base AND folds the zero constant into the bool register, dropping
//        `moveq` entirely: 0x100 emitted, 4 bytes SHORT. 186 bytes.
//   Also inert: bool/uint/short types for wasActive, `!!x`, `x?1:0`, `0 != x`, `!(x==0)`,
//   `(bool)x`, guard as `!wasActive` vs `== 0` vs wrapping the whole body in `if (wasActive)`,
//   void*/int field50, volatile on field50 or field0c, const/register param, 2-4 dummy params,
//   nested-struct spelling for field0c, references, inline getter/setter helpers (both orders),
//   comma-expression sequencing (mwcc always emits the assignment side effect first),
//   declaration hoisting/splitting for wasActive, i, slot and ctx.
//
// Store SOURCE ORDER is confirmed correct: permuting the six zero-stores permutes the emitted
// stores 1:1 (15 bytes), and moving field0c to second gives 10 bytes.
//
// Next lever to try: something that changes what is live across the `ldr r1,[r1,#0x50]` so the
// scheduler prefers the store for the load-delay slot. Nothing at the statement level reaches it.

#include <globaldefs.h>
#include "System/ProcessorContext.h"

struct GlobalStruct0224c980_02200160 {
    char pad0[0xc];
    void* field0c;
    char pad1[0x1c - 0x10];
    unsigned int field1c;
    unsigned int field20;
    char pad2[0x2c - 0x24];
    unsigned int field2c;
    char pad3[0x40 - 0x30];
    void (*field40)(unsigned int);
    char pad4[0x50 - 0x44];
    unsigned int field50;
    char pad5[0x60 - 0x54];
    unsigned int field60;
    unsigned int field64;
};

struct PendingRequest_02200160 {
    void* context;
    int type;
    unsigned char status;
};

struct EventSlot_02200160 {
    char pad0[4];
    unsigned short active;
    char pad1[0x34 - 6];
    unsigned int payload;
};

extern GlobalStruct0224c980_02200160 data_ov031_0224c980;
extern unsigned int data_ov031_0224ca00[0x18];
extern EventSlot_02200160 data_ov031_0224cca8[8];

extern "C" void VectorizedMemset(void* dst, unsigned int value, unsigned int size);
void ZeroGlobalBuffer0224e3b8();

// USA: func_ov031_02200160  (semantic: ResetPendingRequestsAndSlots_02200160)
extern "C" ARM void func_ov031_02200160(void* owner) {
    int wasActive = data_ov031_0224c980.field50 != 0;

    data_ov031_0224c980.field0c = owner;
    data_ov031_0224c980.field50 = 0;
    data_ov031_0224c980.field1c = 0;
    data_ov031_0224c980.field2c = 0;
    data_ov031_0224c980.field60 = 0;
    data_ov031_0224c980.field64 = 0;
    data_ov031_0224c980.field20 = 0;
    if (!wasActive) {
        return;
    }

    VectorizedMemset(data_ov031_0224ca00, 0, 0x60);

    ProcessorContext* ctx = data_02111304.firstContext;
    if (ctx != NULL) {
        do {
            PendingRequest_02200160* p = (PendingRequest_02200160*)ctx->unknown_A4;
            if (p != NULL && p->context != NULL) {
                if (p->status != 0xa && p->status != 0xb) {
                    p->status = 0;
                }
                if (p->type != 0) {
                    p->type = 0;
                    MarkContextReadyAndSwitch((ProcessorContext*)p->context);
                }
            }
            ctx = ctx->pNext;
        } while (ctx != NULL);
    }

    int i = 0;
    EventSlot_02200160* slot = data_ov031_0224cca8;
    do {
        if (slot->active != 0) {
            data_ov031_0224c980.field40(slot->payload);
            slot->active = 0;
        }
        i++;
        slot++;
    } while (i < 8);

    ZeroGlobalBuffer0224e3b8();
}
