# EasyVEHHookTemplate  
A simplely VEH hook template.Support x64 and x86.  
[![CodeFactor](https://www.codefactor.io/repository/github/moshuid/easyvehhooktemplate/badge)](https://www.codefactor.io/repository/github/moshuid/easyvehhooktemplate)
# Why I build it?  
I need a hook to get WeChat's received messages.  
# How to use it?  
First of all,choose one of the hook's method.If you want use page exception to hook change PAGE_EXCEPTION 0 to 1 and change INT3_BREAKPOINT 1 to 0.  
Next include VEH-Hook.h in your project and write a function which have you need method.  
`typedef void(Handler)(PCONTEXT);`  
And then new a "VEH" object and use VEH's function "SetHook".  
`bool SetHook(void* orgFunc, Handler* hookHandlerFunc);`  
The last my template is automaticly enable hook.  
