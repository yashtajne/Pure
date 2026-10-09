#include "../../../Interface/Std/IO.h"
#include "../../../Platform/Platform.def"

struct _Stream
{
#if IS_PLATFORM_WINDOWS
	HANDLE handle;
#elif IS_PLATFORM_LINUX
	int32_t descriptor;
#endif
};