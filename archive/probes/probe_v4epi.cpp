// Can a PRAGMA give the ARMv4T epilogue (`pop {..,lr}` + `bx lr`) under -proc arm946e?
// If yes this needs no build change at all; if no, cc_overrides.txt has to carry flags.

typedef unsigned long long u64;

extern "C" u64 e_base(u64 a, u64 b) { return a * b; }

#pragma interworking on
extern "C" u64 e_interwork(u64 a, u64 b) { return a * b; }
#pragma interworking reset

#pragma thumb off
extern "C" u64 e_thumboff(u64 a, u64 b) { return a * b; }
#pragma thumb reset

#pragma ARM_conform on
extern "C" u64 e_conform(u64 a, u64 b) { return a * b; }
#pragma ARM_conform reset
