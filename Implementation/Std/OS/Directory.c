#include "../../../Interface/Std/OS/Directory.h"
#include "../../../Platform/Platform.def"

errcode_t
create_directory(charseq_t pathname)
{
	if ( !pathname ) return PURE_ERROR_NULL_POINTER;
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
delete_directory(charseq_t pathname)
{
	if ( !pathname ) return PURE_ERROR_NULL_POINTER;
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