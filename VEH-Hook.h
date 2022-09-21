#pragma once
#include <Windows.h>

#define PAGE_EXCEPTION 0
#define INT3_BREAKPOINT 1 //vs调试器可能会优先处理，因为异常处理时，调试器>VEH>SEH


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
#if INT3_BREAKPOINT
	static byte orgByte;
#endif
	static LONG NTAPI ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo);
};

void* VEH::orgFuncAddr = nullptr;
VEH::Handler* VEH::HandlerFunc = nullptr;
DWORD VEH::oldProtect = 0;
#if INT3_BREAKPOINT
byte VEH::orgByte=0x00;
#endif
LONG NTAPI VEH::ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
#if PAGE_EXCEPTION
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
	{
#else
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
	{
#endif
		if (ExceptionInfo->ContextRecord->XIP == (RegValue)orgFuncAddr)
		{
			HandlerFunc(ExceptionInfo->ContextRecord);
			ExceptionInfo->ContextRecord->XIP = (RegValue)orgFuncAddr;
#if INT3_BREAKPOINT
			* (byte*)orgFuncAddr = orgByte;
			VirtualProtect(orgFuncAddr, 1, oldProtect, &oldProtect);
#endif
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
bool VEH::SetHook(void* orgFunc, Handler * hookHandlerFunc)
{
	orgFuncAddr = orgFunc;
	HandlerFunc = hookHandlerFunc;
	AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler);
#if PAGE_EXCEPTION
	return VirtualProtect(orgFunc, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &oldProtect);
#else
	orgByte = *(byte*)orgFunc;
	VirtualProtect(orgFunc, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
	return *(byte*)orgFunc = 0xCC;
#endif
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