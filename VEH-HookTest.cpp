#include <iostream>
#include"VEH-Hook.h"

int main()
{
	uintptr_t addr = (uintptr_t)GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxA");
	VEH* v = new VEH();
	v->SetHook((void*)addr, [](PCONTEXT info)
		{
			printf("HOOK成功");
		});
	MessageBoxA(NULL, "1111", "22", MB_OK);
	v->UnHook();
	MessageBoxA(NULL, "1111", "22", MB_OK);
	return 0;
}

