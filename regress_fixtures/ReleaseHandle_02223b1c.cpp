#include <globaldefs.h>

extern void* data_ov031_02250bf8;

extern "C" int _Z24TailCallForward_0223e2e0iPv(int arg0, void* arg1);
extern "C" int func_ov031_0223da58(void* obj);
extern "C" unsigned int func_ov031_0223cf70(void** a);

// USA: func_ov031_02223b1c
extern "C" ARM void func_ov031_02223b1c(void) {
	if (data_ov031_02250bf8 == NULL) {
		return;
	}
	void* field4 = *(void**)((char*)data_ov031_02250bf8 + 4);
	_Z24TailCallForward_0223e2e0iPv(0, field4);
	func_ov031_0223da58(*(void**)data_ov031_02250bf8);
	func_ov031_0223cf70(&data_ov031_02250bf8);
}
