#include "../../../Interface/Std/OS/Directory.h"
#include "../../../Platform/Platform.def"

void
create_directory(charseq_t pathname)
{
	if ( !pathname )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}
#if IS_PLATFORM_WINDOWS
	if ( !CreateDirectoryA( pathname, NULL ) )
	{
		Self.LastError = PURE_ERROR_DIRECTORY_CREATE_FAILED;
		return;
	}
	Self.LastError = PURE_OK;
	return;
#elif IS_PLATFORM_LINUX
	if ( linux_mkdir( pathname, 0777 ) != 0 )
	{
		Self.LastError = PURE_ERROR_DIRECTORY_CREATE_FAILED;
		return;
	}
	Self.LastError = PURE_OK;
	return;
#else
#error "yet to implement"
#endif
}

void
delete_directory(charseq_t pathname)
{
	if ( !pathname )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}
#if IS_PLATFORM_WINDOWS
	if ( !RemoveDirectoryA( pathname ) )
	{
		Self.LastError = PURE_ERROR_DIRECTORY_DELETE_FAILED;
		return;
	}
	Self.LastError = PURE_OK;
#elif IS_PLATFORM_LINUX
	if ( linux_rmdir( pathname ) != 0 )
	{
		Self.LastError = PURE_ERROR_DIRECTORY_DELETE_FAILED;
		return;
	}
	Self.LastError = PURE_OK;
#else
#error "yet to implement"
#endif
}