# EasyVEHHookTemplate  
A simplely VEH hook template  
# Why I build it?  
I need a hook to get WeChat's received messages.  
# How to use it?  
First of all,include VEH-Hook.h in your project and write a function which have you need method.  
`typedef void(Handler)(PCONTEXT);`  
Secondly new a "VEH" object and use VEH's function "SetHook".  
`bool SetHook(void* orgFunc, Handler* hookHandlerFunc);`  
The last my template is automaticly set hook.  
