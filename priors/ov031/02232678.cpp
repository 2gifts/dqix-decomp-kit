#include <globaldefs.h>

struct Obj0223bcc8;
extern "C" void _Z30SetEntryHalfword4Bits_0223bcc8P11Obj0223bcc8ii(Obj0223bcc8* obj, int idx, int val);
extern "C" ARM unsigned char _Z26GetFieldE7ByIndex_0223607ci(int idx);
extern "C" int func_ov031_0223be70(int a, int b, int c);
extern "C" void func_ov031_0223bbd8(void* obj, int idx, int a, int b);

struct S02232678 { unsigned char idx; char pad[3]; char* field4; };
extern S02232678 data_ov031_02290cc0;
struct Pair02232678 { unsigned short x, y; };
extern Pair02232678 data_ov031_02249082[];
extern unsigned char data_ov031_02249060[];

// USA: func_ov031_02232678
extern "C" ARM void func_ov031_02232678(void) {
	int i = 0;
	int zero = i;
	int negOne = -1;
	int three = 3;
	do {
		int v = _Z26GetFieldE7ByIndex_0223607ci(i);
		if (v == 0xff) {
			v = three;
		} else {
			*(int*)(data_ov031_02290cc0.field4 + i * 4 + 0x10) =
				func_ov031_0223be70(zero, 0x11, 1);
			func_ov031_0223bbd8(*(void**)(data_ov031_02290cc0.field4 + i * 4 + 0x10), negOne,
				data_ov031_02249082[i + 3].x, data_ov031_02249082[i + 3].y);
			_Z30SetEntryHalfword4Bits_0223bcc8P11Obj0223bcc8ii(
				(Obj0223bcc8*)*(void**)(data_ov031_02290cc0.field4 + i * 4 + 0x10), negOne, 3);
		}
		*(int*)(data_ov031_02290cc0.field4 + i * 4 + 4) =
			func_ov031_0223be70(0, data_ov031_02249060[v], 1);
		func_ov031_0223bbd8(*(void**)(data_ov031_02290cc0.field4 + i * 4 + 4), negOne,
			data_ov031_02249082[i].x, data_ov031_02249082[i].y);
		_Z30SetEntryHalfword4Bits_0223bcc8P11Obj0223bcc8ii(
			(Obj0223bcc8*)*(void**)(data_ov031_02290cc0.field4 + i * 4 + 4), negOne, 3);
		i++;
	} while (i < 3);
}
