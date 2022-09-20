#pragma once
#include <Windows.h>
#if _WIN64
#define XIP Rip
#define RegValue DWORD64
#else
#define RegValue DWORD
#define XIP Eip
#endif

class VEH
{
public:
	typedef void(Handler)(PCONTEXT);
	bool SetHook(void* orgFunc, Handler* hookHandlerFunc);
	bool UnHook();
	VEH();
	~VEH();
private:
	static void* orgFuncAddr;
	static Handler* HandlerFunc;
	static DWORD oldProtect;
	static LONG NTAPI ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo);
};

void* VEH::orgFuncAddr = nullptr;
VEH::Handler* VEH::HandlerFunc = nullptr;
DWORD VEH::oldProtect = 0;

LONG NTAPI VEH::ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
	{
		if (ExceptionInfo->ContextRecord->XIP == (RegValue)orgFuncAddr)
		{
			HandlerFunc(ExceptionInfo->ContextRecord);
			ExceptionInfo->ContextRecord->XIP = (RegValue)orgFuncAddr;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
bool VEH::SetHook(void* orgFunc, Handler* hookHandlerFunc)
{
	orgFuncAddr = orgFunc;
	HandlerFunc = hookHandlerFunc;
	AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler);
	return VirtualProtect(orgFunc, 1, PAGE_GUARD, &oldProtect);
}
bool VEH::UnHook()
{
	return VirtualProtect(orgFuncAddr, 1, oldProtect, &oldProtect);
}
VEH::VEH()
{
}

VEH::~VEH()
{
}