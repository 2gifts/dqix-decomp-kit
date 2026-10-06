#include <globaldefs.h>
#include "System/Interrupts.h"
#include "System/Memory.h"

struct StreamCounter_02205b48 {
	unsigned char pad00[0xf8];
	int bytesConsumed;
	unsigned char padfc[2];
	signed char countingDisabled;
};

struct StreamContext_02205b48 {
	unsigned char pad00[0x64];
	struct StreamCounter_02205b48* counter;
	unsigned char pad68[8];
	short flags;
	unsigned char pad72;
	signed char encoding;
	unsigned short lastTag;
};

extern "C" void* func_ov031_02205c50(struct StreamContext_02205b48* ctx, int* available,
	unsigned short* tag, unsigned short* count, unsigned int* value);

// USA: func_ov031_02205b48  (semantic: ReadStreamBytes_02205b48)
extern "C" ARM int func_ov031_02205b48(struct StreamContext_02205b48* ctx, void* dst,
	int length, unsigned short* outCount, unsigned int* outValue) {
	unsigned short tag;
	unsigned short count;
	int available;
	unsigned int value;
	int result;
	int irqState = DisableIRQInterrupts();
	void* src = func_ov031_02205c50(ctx, &available, &tag, &count, &value);

	if (src != NULL) {
		result = available;
		if (result == 0) {
			result = -6;
		} else {
			signed char encoding = ctx->encoding;
			int wholeChunk = 1;
			if (length > result) {
				length = result;
			}
			if (encoding != 0 && encoding != 4) {
				wholeChunk = 0;
			}
			if (wholeChunk) {
				result = length;
			}
			VectorizedInvertedMemcpy(src, dst, length);
			struct StreamCounter_02205b48* counter = ctx->counter;
			if (counter->countingDisabled == 0) {
				counter->bytesConsumed = counter->bytesConsumed + result;
			}
		}
	} else {
		result = (available == 0) ? 0 : -28;
		ctx->flags = ctx->flags & ~6;
	}

	if (result >= 0) {
		if (outCount != NULL && outValue != NULL) {
			*outCount = count;
			*outValue = value;
		}
		if (ctx->lastTag == 0) {
			ctx->lastTag = tag;
		}
	}
	SetIRQInterruptState(irqState);
	return result;
}
