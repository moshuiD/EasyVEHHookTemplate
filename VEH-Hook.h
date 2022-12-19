#pragma once
#include <Windows.h>
#include <vector>
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
	void UnHook(void* oldFunc);
	VEH();
	~VEH();
	struct Hook
	{
		void* orgFuncAddr;
		Handler* HandlerFunc;
		DWORD oldProtect;
#if INT3_BREAKPOINT
		byte orgByte[];
#endif
	};
private:
	
	static std::vector<Hook>HookList;
	static void End() { return; };
	static LONG NTAPI ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo)
	{
#if PAGE_EXCEPTION
		if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
		{
#else
		if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
		{
#endif

			for (Hook x : HookList)
			{
				if ((RegValue)x.orgFuncAddr == ExceptionInfo->ContextRecord->XIP)
				{
					
#if PAGE_EXCEPTION
					VirtualProtect(x.orgFuncAddr, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &x.oldProtect);
#else
					x.HandlerFunc(ExceptionInfo->ContextRecord);					
#endif
					ExceptionInfo->ContextRecord->XIP = (RegValue)End;
					return EXCEPTION_CONTINUE_EXECUTION;
				}
			}
		}
		
		return EXCEPTION_CONTINUE_SEARCH;
	}
};


std::vector<VEH::Hook>VEH::HookList;

bool VEH::SetHook(void* orgFunc, Handler* hookHandlerFunc)
{
	Hook h{ orgFunc ,hookHandlerFunc };
	AddVectoredExceptionHandler(1, ExceptionHandler);
#if PAGE_EXCEPTION
	bool retV = VirtualProtect(orgFunc, 1, PAGE_EXECUTE_READ | PAGE_GUARD, &h.oldProtect);
	HookList.push_back(h);
	return retV;
#else
	VirtualProtect(orgFunc, 1, PAGE_EXECUTE_READWRITE, &h.oldProtect);
	HookList.push_back(h);
	return *(byte*)orgFunc = 0xCC;
#endif
}
void VEH::UnHook(void* orgFunc)
{
	std::vector<Hook>::iterator it = std::remove_if(HookList.begin(), HookList.end(), [&](const Hook& h) 
		{
		return h.orgFuncAddr == orgFunc;
		});
	if (it != HookList.end()) {
		VirtualProtect(it->orgFuncAddr, 1, it->oldProtect, &it->oldProtect);
		HookList.erase(it);
	}

}
VEH::VEH()
{
}

VEH::~VEH()
{
}
