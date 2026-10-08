#include "../../Interface/Std/Self.h"
#include "PlatformInc.def"

void exit(uint8_t exitcode)
{
#if IS_PLATFORM_WINDOWS
	ExitProcess( exitcode );
#elif IS_PLATFORM_LINUX
	linux_exit( exitcode );
#else
	#error "platform not supported!"
#endif
}