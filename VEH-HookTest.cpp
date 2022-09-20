#include <iostream>
#include"VEH-Hook.h"
void Handler(PCONTEXT info)
{
	printf("HOOK成功");
}
int main()
{
	uintptr_t addr = (uintptr_t)GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxA");
	VEH* v = new VEH();
	v->SetHook((void*)addr, Handler);
	MessageBoxA(NULL, "1111", "22", MB_OK);
	v->UnHook();
	MessageBoxA(NULL, "1111", "22", MB_OK);
	return 0;
}

