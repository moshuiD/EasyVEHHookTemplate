#include <iostream>
#include"VEH-Hook.h"

int main()
{
	uintptr_t addr = (uintptr_t)GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxA");
	VEH* v = new VEH();
	v->SetHook((void*)addr, [](PCONTEXT info)
		{
			puts("HOOK成功");
		});
	MessageBoxA(NULL, "1111", "22", MB_OK);
	//v->Refresh((void*)addr);
	MessageBoxA(NULL, "1111", "22", MB_OK);
	v->UnHook((void*)addr);
	return 0;
}

