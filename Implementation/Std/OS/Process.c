#include "../../../Interface/Std/OS/Process.h"
#include "../../../Platform/Platform.def"

struct _Process
{
#if IS_PLATFORM_WINDOWS
	HANDLE process_handle;
	HANDLE thread_handle;
#elif IS_PLATFORM_LINUX
#else
#error "Not implemented yet!"
#endif
};

process_t* process_create(charseq_t invocation)
{
	process_t process = { 0 };

#if IS_PLATFORM_WINDOWS

#elif IS_PLATFORM_LINUX
#else
#error "Not implemted yet!"
#endif
}