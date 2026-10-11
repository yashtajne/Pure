#include "../../../Interface/Std/OS/Directory.h"
#include "../../../Platform/Platform.def"

errcode_t
_create_directory(charseq_t pathname)
{
#if IS_PLATFORM_WINDOWS
	if ( !CreateDirectoryA( pathname, NULL ) )
		return PURE_ERROR_DIRECTORY_CREATE_FAILED;
	return PURE_OK;
#elif IS_PLATFORM_LINUX
	if ( linux_mkdir( pathname, 0777 ) != 0 )
		return PURE_ERROR_DIRECTORY_CREATE_FAILED;
	return PURE_OK;
#else
#error "yet to implement"
#endif
}

errcode_t
_delete_directory(charseq_t pathname)
{
#if IS_PLATFORM_WINDOWS
	if ( !RemoveDirectoryA( pathname ) )
		return PURE_ERROR_DIRECTORY_DELETE_FAILED;
	return PURE_OK;
#elif IS_PLATFORM_LINUX
	if ( linux_rmdir( pathname ) != 0 )
		return PURE_ERROR_DIRECTORY_DELETE_FAILED;
	return PURE_OK;
#else
#error "yet to implement"
#endif
}

void
create_directory(charseq_t pathname)
{
	if ( !pathname )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	Self.LastError = _create_directory(pathname);
}

void
delete_directory(charseq_t pathname)
{
	if ( !pathname )
	{
		Self.LastError = PURE_ERROR_NULL_POINTER;
		return;
	}

	Self.LastError = _delete_directory( pathname );
}